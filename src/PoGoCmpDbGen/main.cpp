#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../PoGoCmp/PoGoCmpDb.h"
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

std::string DateTimeOffsetString(std::time_t timestampS)
{
    std::stringstream ss;
    auto tm = std::localtime(&timestampS);
    ss << std::put_time(tm, "%F %T%z"); // ISO 8601 with offset
    return ss.str();
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Output path missing. Usage: PoGoCmpDbGen <inputJsonPath> <outputhPath>\n";
        return EXIT_FAILURE;
    }

    /// @todo Windows Unicode
    std::string inputPath{ argv[1] };
    std::ifstream input{ inputPath };
    if (!input.is_open())
    {
        std::cerr << "Failed to open " << inputPath << "\n";
        return EXIT_FAILURE;
    }

    /// @todo Windows Unicode
    std::string outputPath{ argv[2] };
    std::ofstream output{ outputPath };
    if (!output.is_open())
    {
        std::cerr << "Failed to open " << outputPath << "\n";
        return EXIT_FAILURE;
    }

    using namespace PoGoCmp;
    std::map<uint16_t, PokemonSpecie> pokemonTable;
    std::set<std::string> pokemonTypes;
    std::string dateTimeOffset;

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
            if (std::regex_match(templateId, matches, typePattern))
            {
                pokemonTypes.insert(matches[1]);
            }
            else if (std::regex_match(templateId, matches, idPattern))
            {
                assert(!pokemonTypes.empty());

                const auto& settings = itemTemplate["pokemonSettings"];
                PokemonSpecie pkm{};
                pkm.number = (uint16_t)std::stoi(matches[1]);
                pkm.name = settings["pokemonId"];//matches[2];
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
                /// @todo most of the baby Pokemon can be identified using buddySize but not all
                //auto buddySize = settings.find("buddySize"); // exists only for Pokemon with special buddy placement
                if (it != settings.end() && *it == "POKEMON_RARITY_LEGENDARY")
                {
                    pkm.rarity = PokemonRarity::NORMAL;
                }
                else if (it != settings.end() && *it == "POKEMON_RARITY_MYTHIC")
                {
                    pkm.rarity = PokemonRarity::MYTHIC;
                }
                //else if (buddySize != settings.end() && *buddySize == "BUDDY_BABY")
                //    pkm.rarity = PokemonRarity::BABY;
                else
                {
                    assert(pkm.rarity == PokemonRarity::NORMAL);
                }

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
                    std::cout << pkm.number << " " << pkm.name << " ignored\n";
                }
            }
        }
    }
    catch (const detail::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    output  << "/** @file " << outputPath << "\n"
            << "    @brief C++ API for PoGoCmp. For C one can use PoGoCmp.h.\n"
            << "\n"
            << "    Input file's timestamp " << dateTimeOffset << " */\n";

    output <<
R"(
#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <map>
#ifndef _WIN32
#include <strings.h> // strcasecmp()
#endif

namespace PoGoCmp {

)";
    /// @todo Could the following be deduced somehow?
    /// Seasonal - e.g. Delibird?
    /// Regional - all region-exclusive?
    /// Unrelease/Unobtainable - Pokemon not yet released
    /// Baby - Obtainable only from eggs.
    /// If not, maybe specify them manually.

    output <<
R"(enum class PokemonRarity : uint8_t {
    /// Can be obtained normally in the wild or from eggs, some are raid-exlusive though.
    NORMAL,
    /// Obtainable only from raids, cannot be placed in gyms.
    LEGENDARY,
    /// Obtainability unclear, no mythic Pokemon released yet.
    MYTHIC
};

)";

    const std::string indent{"    "};
    const std::string typeNone{ "NONE" };

    output << "enum class PokemonType : uint8_t {\n";
    output << indent << typeNone << ",\n";
    for (auto type : pokemonTypes)
    {
        output << indent << type << ",\n";
    }
    output << "};\n\n";

    output <<
R"(struct PokemonSpecie
{
    /// Pokédex number.
    uint16_t number;
    /// Base attack.
    uint16_t baseAtk;
    /// Base defence.
    uint16_t baseDef;
    /// Base stamina (a.k.a. HP).
    uint16_t baseSta;
    /// Pokémon's specie name, uppercase with underscores.
    /// Used as an idenfiter for Pokemon (pokemonId) in the input file.
    /// There are only a handful of Pokémon with special character's in their names:
    /// - Mr. Mime -> MR_MIME
    /// - Farfetch'd -> FARFETCHD
    /// - Ho-Oh -> HO_OH
    /// - Mime Jr. -> Unknown at the moment, probably MIME_JR
    /// - Flabébé -> Unknown at the moment, probably FLABEBE
    /// - Nidoran♂  & Nidoran♀ -> NIDORAN_MALE & NIDORAN_FEMALE
    /// The longest name (Crabominable) currently (in a distant PoGO future) has 12 characters,
    /// but as Nidoran♀ is translated into NIDORAN_FEMALE the longest name has 14 characters.
    /// https://bulbapedia.bulbagarden.net/wiki/List_of_Pokémon_by_name
    std::string name;
    /// Primary type.
    PokemonType type;
    /// Secondary type, if applicable.
    PokemonType type2;
    /// Rarity type.
    PokemonRarity rarity;
};

)";

    auto writePokemon = [&output](const PokemonSpecie& pkm)
    {
        output << "{ " << pkm.number << ", " << pkm.baseAtk << ", " << pkm.baseDef << ", " << pkm.baseSta
            << ", " << std::quoted(pkm.name) << ", PokemonType::" << PokemonTypeToString(pkm.type)
            << ", PokemonType::" << PokemonTypeToString(pkm.type2) << ", PokemonRarity::"
            << PokemonRarityToString(pkm.rarity) << " }";
    };

    //auto writePokemonMap = [&](const auto& key, const PokemonSpecie& pkm)
    //{
    //    output << indent << "{ " << key << ", " << writePokemon(pkm) << " },\n";
    //};

    //output << "static const std::map<uint16_t, PokemonSpecie> PokemonByNumber {\n";
    output << "/// Pokedex number - 1 can be used as the index to the array.\n";
    // NOTE double-brace syntax needed for the array's initializer list ctor
    output << "static const std::array<PokemonSpecie, " << pokemonTable.size() << "> PokemonByNumber {{\n";
    for (const auto& it : pokemonTable)
    {
        output << indent;
        writePokemon(it.second);
        output << ",\n";
        //writePokemonMap(it.second.number, it.second);
    }
    output << "}};\n\n";

    output <<
R"(/// Case-insensitive string comparison.
static inline int StrCmpI(const char* str1, const char* str2)
{
#ifdef _WIN32
    return _stricmp(str1, str2);
#else
    return strcasecmp(str1, str2);
#endif
}

)";

    output << "/// case-insensitive\n";
    output << "static inline PokemonType StringToPokemonType(const char* str)\n";
    output << "{\n";
    for (auto type : pokemonTypes)
        output << indent << "if (StrCmpI(str, " << std::quoted(type) << ") == 0) return PokemonType::" << type << ";\n";
    output << indent << "return PokemonType::NONE;\n";
    output << "}\n\n";

    output << "/// Returns all-uppercase name\n";
    output << "static inline const char* PokemonTypeToString(PokemonType type)\n";
    output << "{\n";
    for (auto type : pokemonTypes)
        output << indent << "if (type == PokemonType::" << type << ") return " << std::quoted(type) << ";\n";
    output << indent << "return " << std::quoted(typeNone) << ";\n";
    output << "}\n";

    output <<
R"(
/// Returns all-uppercase name
static inline const char* PokemonRarityToString(PokemonRarity rarity)
{
    if (rarity == PokemonRarity::LEGENDARY) return "LEGENDARY";
    if (rarity == PokemonRarity::MYTHIC) return "MYTHIC";
    return "NORMAL";
}

struct StringLessThanI
{
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return StrCmpI(lhs.c_str(), rhs.c_str()) < 0;
    }
};

)";

    output << "static const std::map<std::string, const PokemonSpecie*, StringLessThanI> PokemonByName {\n";
    for (const auto& it : pokemonTable)
    {
        //writePokemon(std::quoted(it.second.name), it.second);
        output << indent << "{ " << std::quoted(it.second.name) << ", &PokemonByNumber[" << it.first - 1 << "] },\n" ;
    }
    output << "};";
    output <<
R"(

static const std::string MrMimeName{ "Mr. Mime" };
static const std::string FarfetchdName{ "Farfetch'd" };
static const std::string HoOhName{ "Ho-Oh" };
static const std::string NidoranFemaleName{ "Nidoran Female" /** @todo u8"Nidoran♀"*/ };
static const std::string NidoranMaleName{ "Nidoran Male" /** @todo u8"Nidoran♂"*/ };
// - Mime Jr. -> Unknown at the moment, probably MIME_JR
// - Flabébé -> Unknown at the moment, probably FLABEBE
static const std::string EmptyString;

/// Returns ID name corresponding the Pokémon's proper name.
/// @todo Windows Unicode support
static inline const std::string& PokemonNameToId(const std::string& name)
{
    // PokemonByNumber[29-1]    // "NIDORAN_FEMALE
    // PokemonByNumber[32-1]    // "NIDORAN_MALE"
    // PokemonByNumber[83-1]    // "FARFETCHD"
    // PokemonByNumber[122-1]   // "MR_MIME"
    // PokemonByNumber[250-1]   // "HO_OH"
    if (StrCmpI(name.c_str(), u8"Nidoran♀") == 0 || StrCmpI(name.c_str(), "Nidoran Female") == 0) { return PokemonByNumber[29-1].name; }
    else if (StrCmpI(name.c_str(), u8"Nidoran♂") == 0 || StrCmpI(name.c_str(), "Nidoran Male") == 0) { return PokemonByNumber[32-1].name; }
    else if (StrCmpI(name.c_str(), "Farfetch'd") == 0) { return PokemonByNumber[83-1].name; }
    else if (StrCmpI(name.c_str(), "Mr. Mime") == 0 || StrCmpI(name.c_str(), "Mr Mime") == 0 /*UK English check just in case*/) { return PokemonByNumber[122-1].name; }
    else if (StrCmpI(name.c_str(), "Ho-Oh") == 0 || StrCmpI(name.c_str(), "Ho Oh") == 0) { return PokemonByNumber[250-1].name; }
    else
    {
        auto it = PokemonByName.find(name);
        return it != PokemonByName.end() ? it->second->name : EmptyString;
    }
}

/// Returns proper name corresponding the Pokémon's ID name.
/// @todo Nidoran♀ & Nidoran♂ not yet supported (returned as Nidoran Female and Nidoran Male)
static inline const std::string& PokemonIdToName(const std::string& name)
{
    if (StrCmpI(name.c_str(), "NIDORAN_FEMALE") == 0) return NidoranFemaleName;
    else if (StrCmpI(name.c_str(), "NIDORAN_MALE") == 0) return NidoranMaleName;
    else if (StrCmpI(name.c_str(), "FARFETCHD") == 0) return FarfetchdName;
    else if (StrCmpI(name.c_str(), "MR_MIME") == 0) return MrMimeName;
    else if (StrCmpI(name.c_str(), "HO_OH") == 0) return HoOhName;
    else return name;
}

)";
    output << "}\n"; // ~namespace PoGoCmp
}
