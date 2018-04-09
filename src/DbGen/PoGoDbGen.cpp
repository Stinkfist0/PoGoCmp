#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../Lib/PoGoDb.h"
#ifdef _MSC_VER
// Disable couple MSVC's static analyser warnings coming from json.hpp
// The C28020 in particular is probably a false positive.
#pragma warning(push)
#pragma warning(disable : 6031 28020)
#endif
#include "../ThirdParty/nlohmann/json/json.hpp"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <regex>
#include <iomanip>
#include <set>
#include <ctime>
#include <map>

using namespace std::string_literals;

std::string DateTimeOffsetString(std::time_t timestampS)
{
    std::stringstream ss;
    auto tm = std::localtime(&timestampS);
    ss << std::put_time(tm, "%F %T%z"); // ISO 8601 with offset
    return ss.str();
}

int main(int argc, char **argv)
{
    auto args = Utf8::ParseArguments(argc, argv);
    if (args.size() != 2)
    {
        std::cerr << "Exactly two arguments required. Usage: PoGoDbGen <inputJsonPath> <outputCppPath>\n";
        return EXIT_FAILURE;
    }

    const auto& inputPath = args[0];
    Utf8::ifstream input{ inputPath };
    if (!input.is_open())
    {
        Utf8::PrintLine("Failed to open " + inputPath, Utf8::OutputStream::Err);
        return EXIT_FAILURE;
    }

    const auto& outputPath = args[1];
    Utf8::ofstream output{ outputPath };
    if (!output.is_open())
    {
        Utf8::PrintLine("Failed to open " + outputPath, Utf8::OutputStream::Err);
        return EXIT_FAILURE;
    }

    using namespace PoGoCmp;
    std::map<uint16_t, PokemonSpecie> pokemonTable;
    std::set<std::string> pokemonTypes;
    std::string dateTimeOffset;

    // When serializing structs, std::vectors are written as std::arrays.
    struct PlayerLevelSettingsTemp
    {
        // rankNum skipped, seems uninteresting for now
        std::vector<int/*uint32_t*/> requiredExperience;
        std::vector<float> cpMultiplier;
        size_t/*uint8_t*/ maxEggPlayerLevel{0};
        size_t/*uint8_t*/ maxEncounterPlayerLevel{0};
    } playerLevel;

    struct PokemonUpgradeSettingsTemp
    {
        int/*uint8_t*/ upgradesPerLevel;
        int/*uint8_t*/ allowedLevelsAbovePlayer;
        std::vector<int/*uint8_t*/> candyCost;
        std::vector<int/*uint16_t*/> stardustCost;
    } pokemonUpgrades;

    using namespace nlohmann;
    try
    {
        json jsonInput;
        input >> jsonInput;

        std::string timestampMs = jsonInput["timestampMs"];
        auto timestampS = (time_t)std::stoll(timestampMs.c_str()) / 1000;
        dateTimeOffset = DateTimeOffsetString(timestampS);
        std::cout << "Input file's timestamp " << dateTimeOffset << "\n";

        const std::regex idPattern{"V(\\d{4})_POKEMON_(\\w+)"}; // e.g. "V0122_POKEMON_MR_MIME"
        const std::regex typePattern{"POKEMON_TYPE_(\\w+)"}; // e.g. "POKEMON_TYPE_PSYCHIC"

        for (auto& itemTemplate : jsonInput["itemTemplates"])
        {
            std::string templateId = itemTemplate["templateId"];
            std::smatch matches;
            if (templateId == "PLAYER_LEVEL_SETTINGS")
            {
                auto settings = itemTemplate["playerLevel"];
                auto requiredExperience = settings["requiredExperience"];
                playerLevel.requiredExperience.assign(requiredExperience.begin(), requiredExperience.end());
                auto cpMultiplier = settings["cpMultiplier"];
                playerLevel.cpMultiplier.assign(cpMultiplier.begin(), cpMultiplier.end());
                playerLevel.maxEggPlayerLevel = settings["maxEggPlayerLevel"];
                playerLevel.maxEncounterPlayerLevel = settings["maxEncounterPlayerLevel"];
            }
            else if (templateId == "POKEMON_UPGRADE_SETTINGS")
            {
                //"templateId": "POKEMON_UPGRADE_SETTINGS",
                //"pokemonUpgrades": {
                //  "upgradesPerLevel": 2,
                //  "allowedLevelsAbovePlayer": 2,
                //  "candyCost": [ ... ],
                //  "stardustCost": [ ... ]
                //}
                auto settings = itemTemplate["pokemonUpgrades"];
                pokemonUpgrades.upgradesPerLevel = settings["upgradesPerLevel"];
                pokemonUpgrades.allowedLevelsAbovePlayer = settings["allowedLevelsAbovePlayer"];
                auto candyCost = settings["candyCost"];
                pokemonUpgrades.candyCost.assign(candyCost.begin(), candyCost.end());
                auto stardustCost = settings["stardustCost"];
                pokemonUpgrades.stardustCost.assign(stardustCost.begin(), stardustCost.end());
            }
            else if (std::regex_match(templateId, matches, typePattern))
            {
                pokemonTypes.insert(matches[1]);
            }
            else if (std::regex_match(templateId, matches, idPattern))
            {
                assert(!pokemonTypes.empty());

                const auto& settings = itemTemplate["pokemonSettings"];
                PokemonSpecie pkm{};
                pkm.number = (uint16_t)std::stoi(matches[1]);
                pkm.id = settings["pokemonId"];//matches[2];
                pkm.baseAtk = settings["stats"]["baseAttack"];
                pkm.baseDef = settings["stats"]["baseDefense"];
                pkm.baseSta = settings["stats"]["baseStamina"];
                // Primary type
                std::string type = settings["type"];
                std::regex_match(type, matches, typePattern);
                pkm.type = StringToPokemonType(matches[1].str().c_str());
                assert(pkm.type != PokemonType::NONE);
                // Secondary type, if applicable
                auto it = settings.find("type2");
                if (it != settings.end())
                {
                    std::string type2 = *it;
                    std::regex_match(type2, matches, typePattern);
                    pkm.type2 = StringToPokemonType(matches[1].str().c_str());
                    assert(pkm.type2 != PokemonType::NONE);
                }
                else
                {
                    assert(pkm.type2 == PokemonType::NONE);
                }
                // Rarity
                it = settings.find("rarity"); // exists only for legendary and mythic Pokemon
                //! @todo most of the baby Pokemon can be identified using buddySize but not all
                //auto buddySize = settings.find("buddySize"); // exists only for Pokemon with special buddy placement
                if (it != settings.end() && *it == "POKEMON_RARITY_LEGENDARY")
                {
                    pkm.rarity = PokemonRarity::LEGENDARY;
                }
                else if (it != settings.end() && *it == "POKEMON_RARITY_MYTHIC")
                {
                    pkm.rarity = PokemonRarity::MYTHIC;
                }
                //else if (buddySize != settings.end() && *buddySize == "BUDDY_BABY")
                //    pkm.rarity = PokemonRarity::BABY;
                else
                {
                    pkm.rarity = PokemonRarity::NORMAL;
                }
                assert(pkm.rarity != PokemonRarity::NONE);

                // TODO Need to take forms into consideration. For now only insert the main entry.
                // Unown (29, same stats, 27 released currently)
                // Castform (4, same stas, different moves)
                // Deoxys (4, completely different)
                if (pokemonTable.find(pkm.number) == pokemonTable.end())
                {
                    pokemonTable[pkm.number] = pkm;
                }
                else
                {
                    std::cout << pkm.number << " " << pkm.id << " ignored\n";
                }
            }
        }
    }
    catch (const detail::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    const std::string indent{"    "};

    output  << "/** @file " << outputPath << "\n"
            << "    @brief C++ API for PoGoCmp. For C one can use PoGoCmp.h.\n"
            << "\n"
            << "    Input file's timestamp " << dateTimeOffset << " */\n";

    output <<
R"(
#pragma once

#include "Utf8.h"

#include <cstdint>
#include <string>
#include <array>
#include <map>
#ifndef _WIN32
#include <strings.h> // strcasecmp()
#endif

namespace PoGoCmp {

)";
    //! @todo Could the following be deduced somehow?
    //! Seasonal - e.g. Delibird?
    //! Regional - all region-exclusive?
    //! Unrelease/Unobtainable - Pokemon not yet released
    //! Baby - Obtainable only from eggs.
    //! If not, maybe specify them manually.

    output <<
R"(enum class PokemonRarity : uint8_t
{
    //! Unspecified/invalid.
    NONE,
    //! Can be obtained normally in the wild or from eggs, some are raid-exlusive though.
    NORMAL,
    //! Obtainable only from raids, cannot be placed in gyms.
    LEGENDARY,
    //! Obtainability unclear, no mythic Pokemon released yet.
    MYTHIC
};

)";

    auto writeArray = [&](const char* valueType, const char* name, const auto& vec, int valuesPerRow, const char* literalPostfix)
    {
        output << indent << "std::array<" << valueType << ", " << vec.size() << "> " << name << "{{";
        auto size = vec.size();
        for (size_t i = 0; i < size; ++i)
        {
            if (i % valuesPerRow == 0) output << '\n' << indent << indent;
            else output << ' ';
            output << vec[i] << literalPostfix;
            if (i < size - 1) output << ',';
        }
        output << '\n' << indent << "}};\n";
    };

    output << "const struct PlayerLevelSettings\n";
    output << "{\n";
    output << indent << "//! The required amount of XP to level up.\n";
    writeArray("uint32_t", "requiredExperience", playerLevel.requiredExperience, 10, "");

    output << indent << "//! Level cap for Pokémon from eggs.\n";
    output << indent << "uint8_t  maxEggPlayerLevel{" << playerLevel.maxEggPlayerLevel << "};\n";

    output << indent << "//! Level cap for Pokémon in the wild.\n";
    output << indent << "//! Additional WeatherBonus.cpBaseLevelBonus can be added to this for wild encounters.\n";
    output << indent << "uint8_t  maxEncounterPlayerLevel{" << playerLevel.maxEncounterPlayerLevel << "};\n";

    output << indent << "//! Combat point (CP) multipliers for different Pokémon levels.\n";
    writeArray("float", "cpMultiplier", playerLevel.cpMultiplier, 10, "f");

    output << "} PlayerLevel;\n\n";

    auto vectorToString = [&](const auto& vec, int valuesPerRow, const char* literalPostfix)
    {
        std::stringstream ss;
        ss << "{{";
        auto size = vec.size();
        for (size_t i = 0; i < size; ++i)
        {
            if (i % valuesPerRow == 0) ss << '\n' << indent << indent;
            else ss << ' ';
            ss << vec[i] << literalPostfix;
            if (i < size - 1) ss << ',';
        }
        ss << '\n' << indent << "}};";
        return ss.str();
    };

    output <<
        "const struct PokemonUpgradeSettings\n" +
        "{\n"s +
        indent + "//! How many power-ups a a level consists of.\n" +
        indent + "uint8_t upgradesPerLevel{" + std::to_string(pokemonUpgrades.upgradesPerLevel) + "};\n" +
        indent + "//! Trainer level + allowedLevelsAbovePlayer is the maximum level for the Pokémon.\n" +
        indent + "uint8_t allowedLevelsAbovePlayer{" + std::to_string(pokemonUpgrades.allowedLevelsAbovePlayer) + "};\n" +
        indent + "//! The candy cost to upgrade from one level to the next one.\n" +
        indent + "std::vector<uint8_t> candyCost" + vectorToString(pokemonUpgrades.candyCost, 10, "") + "\n" +
        indent + "//! The stardust cost to upgrade from the one level to the next one.\n" +
        indent + "std::vector<uint16_t> stardustCost" + vectorToString(pokemonUpgrades.stardustCost, 10, "") + "\n" +
        "} PokemonUpgrades;\n\n";

    const std::string typeNone{ "NONE" };

    output << "enum class PokemonType : uint8_t\n{\n";
    output << indent << typeNone << ",\n";
    for (auto type : pokemonTypes)
    {
        output << indent << type << ",\n";
    }
    output << "};\n\n";

    output <<
R"(struct PokemonSpecie
{
    //! Pokédex number.
    uint16_t number;
    //! Base attack.
    uint16_t baseAtk;
    //! Base defence.
    uint16_t baseDef;
    //! Base stamina (a.k.a. HP).
    uint16_t baseSta;
    //! Pokémon's ID/specie name, uppercase with underscores.
    //! Use PokemonIdToName() to translate this into a proper name.
    //! The longest name (Crabominable) currently (in a distant PoGO future) has 12 characters,
    //! but as Nidoran♀ is translated into NIDORAN_FEMALE the longest name has 14 characters.
    //! https://bulbapedia.bulbagarden.net/wiki/List_of_Pokémon_by_name
    std::string id;
    //! Primary type.
    PokemonType type;
    //! Secondary type, if applicable.
    PokemonType type2;
    //! Rarity type.
    PokemonRarity rarity;
};

)";

    auto writePokemon = [&output](const PokemonSpecie& pkm)
    {
        output << "{ " << pkm.number << ", " << pkm.baseAtk << ", " << pkm.baseDef << ", " << pkm.baseSta
            << ", " << std::quoted(pkm.id) << ", PokemonType::" << PokemonTypeToString(pkm.type)
            << ", PokemonType::" << PokemonTypeToString(pkm.type2) << ", PokemonRarity::"
            << PokemonRarityToString(pkm.rarity) << " }";
    };

    //auto writePokemonMap = [&](const auto& key, const PokemonSpecie& pkm)
    //{
    //    output << indent << "{ " << key << ", " << writePokemon(pkm) << " },\n";
    //};

    //output << "static const std::map<uint16_t, PokemonSpecie> PokemonByNumber {\n";
    output << "//! Pokedex number - 1 can be used as the index to the array.\n";
    // NOTE double-brace syntax needed for the array's initializer list ctor
    output << "static const std::array<PokemonSpecie, " << pokemonTable.size() << "> PokemonByNumber{{\n";
    for (const auto& it : pokemonTable)
    {
        output << indent;
        writePokemon(it.second);
        output << ",\n";
        //writePokemonMap(it.second.number, it.second);
    }
    output << "}};\n\n";

    output <<
R"(//! Case-insensitive string comparison.
static inline int CompareI(const char* str1, const char* str2)
{
#ifdef _WIN32
    return _stricmp(str1, str2);
#else
    return strcasecmp(str1, str2);
#endif
}

)";

    output << "//! case-insensitive\n";
    output << "static inline PokemonType StringToPokemonType(const char* str)\n";
    output << "{\n";
    for (auto type : pokemonTypes)
        output << indent << "if (CompareI(str, " << std::quoted(type) << ") == 0) return PokemonType::" << type << ";\n";
    output << indent << "return PokemonType::NONE;\n";
    output << "}\n\n";

    output << "//! Returns all-uppercase name\n";
    output << "static inline const char* PokemonTypeToString(PokemonType type)\n";
    output << "{\n";
    for (auto type : pokemonTypes)
        output << indent << "if (type == PokemonType::" << type << ") return " << std::quoted(type) << ";\n";
    output << indent << "return " << std::quoted(typeNone) << ";\n";
    output << "}\n";

    output <<
R"(
//! Case-insensitive.
//! @return PokemonRarity::NONE if unknown string passed.
static inline PokemonRarity StringToPokemonRarity(const char* str)
{
    if (CompareI(str, "LEGENDARY") == 0) return PokemonRarity::LEGENDARY;
    if (CompareI(str, "MYTHIC") == 0) return PokemonRarity::MYTHIC;
    if (CompareI(str, "NORMAL") == 0) return PokemonRarity::NORMAL;
    return PokemonRarity::NONE;
}

//! Returns all-uppercase name.
static inline const char* PokemonRarityToString(PokemonRarity rarity)
{
    if (rarity == PokemonRarity::LEGENDARY) return "LEGENDARY";
    if (rarity == PokemonRarity::MYTHIC) return "MYTHIC";
    if (rarity == PokemonRarity::NORMAL) return "NORMAL";
    if (rarity == PokemonRarity::NONE) return "NONE";
    return "";
}

struct StringLessThanI
{
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return CompareI(lhs.c_str(), rhs.c_str()) < 0;
    }
};

//! Case-insensitive.
//! @sa PokemonNameToId, PokemonIdToName
static const std::map<std::string, const PokemonSpecie*, StringLessThanI> PokemonByIdName {
)";
    for (const auto& it : pokemonTable)
    {
        //writePokemon(std::quoted(it.second.name), it.second);
        output << indent << "{ " << std::quoted(it.second.id) << ", &PokemonByNumber[" << it.first - 1 << "] },\n" ;
    }
    output << "};";
    output <<
R"(

// Pokémon whose names don't directly match the ID name.
static const std::string MrMimeName{ "Mr. Mime" };
static const std::string FarfetchdName{ "Farfetch'd" };
static const std::string HoOhName{ "Ho-Oh" };
// Cannot use the actual ♀ & ♂ symbols in the literals as those would get corrupted
// on MSVC unless the file would be saved as UTF-16 LE BOM which I don't want to do.
static const Utf8::String NidoranFemaleName{ u8"Nidoran\u2640" };
static const Utf8::String NidoranMaleName{ u8"Nidoran\u2642" };
// - Mime Jr. -> Unknown at the moment, probably MIME_JR
// - Flabébé -> Unknown at the moment, probably FLABEBE
static const std::string EmptyString;

//! Returns ID name corresponding the Pokémon's proper name, case-insensitive.
//! @note "Nidoran Female", "Nidoran Male", "Mr Mime" and "Ho Oh" accepted also.
static inline const std::string& PokemonNameToId(const Utf8::String& name)
{
    // PokemonByNumber[29-1]    // "NIDORAN_FEMALE
    // PokemonByNumber[32-1]    // "NIDORAN_MALE"
    // PokemonByNumber[83-1]    // "FARFETCHD"
    // PokemonByNumber[122-1]   // "MR_MIME"
    // PokemonByNumber[250-1]   // "HO_OH"
    if (Utf8::CompareI(name.c_str(), NidoranFemaleName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Nidoran Female") == 0) { return PokemonByNumber[29-1].id; }
    else if (Utf8::CompareI(name.c_str(), NidoranMaleName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Nidoran Male") == 0) { return PokemonByNumber[32-1].id; }
    else if (Utf8::CompareI(name.c_str(), FarfetchdName.c_str()) == 0) { return PokemonByNumber[83-1].id; }
    else if (Utf8::CompareI(name.c_str(), MrMimeName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Mr Mime") == 0) { return PokemonByNumber[122-1].id; }
    else if (Utf8::CompareI(name.c_str(), HoOhName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Ho Oh") == 0) { return PokemonByNumber[250-1].id; }
    else
    {
        auto it = PokemonByIdName.find(name);
        return it != PokemonByIdName.end() ? it->second->id : EmptyString;
    }
}

//! Returns proper name corresponding the Pokémon's ID name.
//! There are only a handful of Pokémon with special character's in their names:
//! - Mr. Mime -> MR_MIME
//! - Farfetch'd -> FARFETCHD
//! - Ho-Oh -> HO_OH
//! - Mime Jr. -> Unknown at the moment, probably MIME_JR
//! - Flabébé -> Unknown at the moment, probably FLABEBE
//! - Nidoran♂  & Nidoran♀ -> NIDORAN_MALE & NIDORAN_FEMALE
static inline const Utf8::String& PokemonIdToName(const std::string& name)
{
    if (CompareI(name.c_str(), "NIDORAN_FEMALE") == 0) return NidoranFemaleName;
    else if (CompareI(name.c_str(), "NIDORAN_MALE") == 0) return NidoranMaleName;
    else if (CompareI(name.c_str(), "FARFETCHD") == 0) return FarfetchdName;
    else if (CompareI(name.c_str(), "MR_MIME") == 0) return MrMimeName;
    else if (CompareI(name.c_str(), "HO_OH") == 0) return HoOhName;
    else return name;
}

)";
    output << "}\n"; // ~namespace PoGoCmp
}
