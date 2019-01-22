#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

///! @todo try to remove the self-dependency
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

//! @todo shared utility file for these
bool Equals(float a, float b, float eps = 1e-5f) { return std::abs(a - b) < eps; }
bool IsZero(float a, float eps = 1e-5f) { return Equals(a, 0, eps); }

std::string FloatLiteralToString(float f)
{
    std::stringstream ss;
    ss << f;
    if (ss.str().find_first_of(".") != std::string::npos)
        ss << "f";
    return ss.str();
}

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

    struct PokemonSpecieTemp
    {
        PokedexNumber number;
        uint16_t baseAtk;
        uint16_t baseDef;
        uint16_t baseSta;
        std::string id;
        // Not in main PokemonSpecie struct yet, T.B.D.
        std::string formId;
        PokemonType type;
        PokemonType type2;
        PokemonRarity rarity;
        uint8_t buddyDistance;
        float malePercent;
        float femalePercent;
    };

    using PokemonTable = std::multimap<uint16_t, PokemonSpecieTemp>;
    PokemonTable pokemonTable;
    std::set<std::string> pokemonTypes;
    std::string dateTimeOffset;

    // When serializing structs, std::vectors are written as std::arrays.
    struct PLAYER_LEVEL_SETTINGS
    {
        // rankNum skipped, seems uninteresting for now
        std::vector<int/*uint32_t*/> requiredExperience;
        std::vector<float> cpMultiplier;
        size_t/*uint8_t*/ maxEggPlayerLevel;
        size_t/*uint8_t*/ maxEncounterPlayerLevel;
    } playerLevel{};

    struct POKEMON_UPGRADE_SETTINGS
    {
        int/*uint8_t*/ upgradesPerLevel;
        int/*uint8_t*/ allowedLevelsAbovePlayer;
        std::vector<int/*uint8_t*/> candyCost;
        std::vector<int/*uint16_t*/> stardustCost;
    } pokemonUpgrades{};

    struct WEATHER_BONUS_SETTINGS
    {
        int /*uint8_t*/ cpBaseLevelBonus;
        int /*uint8_t*/ guaranteedIndividualValues;
        float stardustBonusMultiplier;
        float attackBonusMultiplier;
        int /*uint8_t*/ raidEncounterCpBaseLevelBonus;
        int /*uint8_t*/ raidEncounterGuaranteedIndividualValues;
    } weatherBonus{};

    const size_t numTypes = 18;
    //float typeEffectiveness[numTypes][numTypes]{};
    std::array<std::array<float, numTypes>, numTypes> typeEffectiveness{{}};

    using namespace nlohmann;
    try
    {
        json jsonInput;
        input >> jsonInput;

        std::string timestampMs = jsonInput["timestampMs"];
        auto timestampS = (time_t)std::stoll(timestampMs.c_str()) / 1000;
        dateTimeOffset = DateTimeOffsetString(timestampS);
        std::cout << "Input file's timestamp " << dateTimeOffset << "\n";

        const std::regex formPattern{"FORMS_V(\\d{4})_POKEMON_(\\w+)"}; // e.g. "FORMS_V0122_POKEMON_MR_MIME"
        const std::regex spawnIdPattern{"SPAWN_V(\\d{4})_POKEMON_(\\w+)"}; // e.g. "SPAWN_V0122_POKEMON_MR_MIME"
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
                auto settings = itemTemplate["pokemonUpgrades"];
                pokemonUpgrades.upgradesPerLevel = settings["upgradesPerLevel"];
                pokemonUpgrades.allowedLevelsAbovePlayer = settings["allowedLevelsAbovePlayer"];
                auto candyCost = settings["candyCost"];
                pokemonUpgrades.candyCost.assign(candyCost.begin(), candyCost.end());
                auto stardustCost = settings["stardustCost"];
                pokemonUpgrades.stardustCost.assign(stardustCost.begin(), stardustCost.end());
            }
            else if (templateId == "WEATHER_BONUS_SETTINGS")
            {
                auto settings = itemTemplate["weatherBonusSettings"];
                weatherBonus.cpBaseLevelBonus = settings["cpBaseLevelBonus"];
                weatherBonus.guaranteedIndividualValues = settings["guaranteedIndividualValues"];
                weatherBonus.stardustBonusMultiplier = settings["stardustBonusMultiplier"];
                weatherBonus.attackBonusMultiplier = settings["attackBonusMultiplier"];
                weatherBonus.raidEncounterCpBaseLevelBonus = settings["raidEncounterCpBaseLevelBonus"];
                weatherBonus.raidEncounterGuaranteedIndividualValues = settings["raidEncounterGuaranteedIndividualValues"];
            }
            else if (std::regex_match(templateId, matches, typePattern))
            {
                const auto type = StringToPokemonType(matches[1].str().c_str());
                std::vector<float> attackScalar = itemTemplate["typeEffective"]["attackScalar"];
                for (size_t j = 0; j < numTypes; ++j)
                    typeEffectiveness[(size_t)type][j] = attackScalar[j];

                pokemonTypes.insert(matches[1]);
            }
            else if (std::regex_match(templateId, matches, formPattern))
            {
                //"templateId": "FORMS_V0351_POKEMON_CASTFORM",
                //"formSettings": {
                //  "pokemon": "CASTFORM",
                //  "forms": [{
                //    "form": "CASTFORM_NORMAL",
                //    "assetBundleValue": 11
                //  }, {
                //    "form": "CASTFORM_SUNNY",
                //    "assetBundleValue": 12
                //  }, {
                //    "form": "CASTFORM_RAINY",
                //    "assetBundleValue": 13
                //  }, {
                //    "form": "CASTFORM_SNOWY",
                //    "assetBundleValue": 14
                //  }]

                // Form information comes first. Initialize the specie/form data in pokemonTable.
                PokemonSpecieTemp pkm{};
                pkm.type = pkm.type2 = PokemonType::NONE;
                pkm.number = (uint16_t)std::stoi(matches[1]);
                auto formSettings = itemTemplate["formSettings"];
                pkm.id = formSettings["pokemon"];
                auto forms = formSettings.find("forms");
                if (forms != formSettings.end())
                {
                    for (const auto& form : *forms)
                    {
                        pkm.formId = form["form"];
                        pokemonTable.insert({ pkm.number, pkm });
                    }
                }
                else
                {
                    pokemonTable.insert({ pkm.number, pkm });
                }
            }
            else if (std::regex_match(templateId, matches, spawnIdPattern))
            {
                // "Spawn" (gender) information comes before the main information.
                // There seems to be extraneous information for every Pokémon with multiple forms,
                // e.g. RATTATA, RATTATA_ALOLA and RATTATA_NORMAL. Discard the ones without form postfix.
                auto number = (uint16_t)std::stoi(matches[1]);
                auto forms = pokemonTable.equal_range(number);
                const std::string formId = matches[2];
                auto it = std::distance(forms.first, forms.second) == 1
                    ? forms.first
                    : std::find_if(forms.first, forms.second, [&formId](const auto& form) { return formId == form.second.formId; });
                if (it == pokemonTable.end())
                {
                    std::cout << "skip " << number << " " << formId << "\n";
                    continue;
                }

                auto& pkm = it->second;
                auto gender = itemTemplate["genderSettings"]["gender"];
                auto malePercent = gender.find("malePercent");
                if (malePercent != gender.end())
                    pkm.malePercent = *malePercent;
                auto femalePercent = gender.find("femalePercent");
                if (femalePercent != gender.end())
                    pkm.femalePercent = *femalePercent;
                auto genderlessPercent = gender.find("genderlessPercent");
                if (genderlessPercent != gender.end())
                    pkm.malePercent = pkm.femalePercent = 0.f;
                pkm.malePercent *= 100;
                pkm.femalePercent *= 100;
            }
            else if (std::regex_match(templateId, matches, idPattern))
            {
                const auto& settings = itemTemplate["pokemonSettings"];
                auto number = (uint16_t)std::stoi(matches[1]);
                auto id = settings["pokemonId"];//matches[2];
                std::string idStr = id;
                auto form = settings.find("form");
                const std::string formName = form != settings.end() ? *form : id;
                auto forms = pokemonTable.equal_range(number);
                for(auto formIt = forms.first; formIt != forms.second; ++formIt)
                {
                    auto& pkm = formIt->second;
                    // There seems to be extraneous information for every Pokémon with multiple forms,
                    // e.g. RATTATA, RATTATA_ALOLA and RATTATA_NORMAL. Discard the ones without form postfix.
                    // Also e.g. Unown and Spinda are missing the "form" and have only a single data entry for all forms.
                    if ((!pkm.formId.empty() && formName == pkm.formId) ||
                        formName == pkm.id ||
                        pkm.formId.find(formName) == 0)
                    {
                        //if (!pkm.formId.empty())
                        //    pkm.id = pkm.formId;
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
                            const std::string type2 = *it;
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

                        // kmBuddyDistance is stored as double but is always a whole number
                        double buddyDistance = settings["kmBuddyDistance"];
                        pkm.buddyDistance = (uint8_t)buddyDistance;
                        assert(pkm.buddyDistance == buddyDistance);
                    }
                }
            }
        }
    }
    catch (const detail::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    const auto numLevels = playerLevel.cpMultiplier.size();
    assert(numLevels == playerLevel.requiredExperience.size());
    assert(numLevels == pokemonUpgrades.candyCost.size());
    assert(numLevels == pokemonUpgrades.stardustCost.size());

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
#include <algorithm>
#include <locale>
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
        "\n"
        "const struct PlayerLevelSettings\n" +
        "{\n"s +
        indent << "//! The required amount of XP to level up.\n" +
        indent + "std::array<uint32_t, " + std::to_string(numLevels) + "> requiredExperience" + vectorToString(playerLevel.requiredExperience, 10, "") + "\n" +
        indent + "//! Level cap for Pokémon from eggs.\n" +
        indent + "uint8_t  maxEggPlayerLevel{" << playerLevel.maxEggPlayerLevel << "};\n" +
        indent + "//! Level cap for Pokémon in the wild.\n" +
        indent + "//! Additional WeatherBonus.cpBaseLevelBonus can be added to this for wild encounters.\n" +
        indent + "uint8_t  maxEncounterPlayerLevel{" << playerLevel.maxEncounterPlayerLevel << "};\n" +
        indent + "//! Combat point (CP) multipliers for different Pokémon levels.\n" +
        indent + "std::array<float, " + std::to_string(numLevels) + "> cpMultiplier" + vectorToString(playerLevel.cpMultiplier, 10, "f") + "\n" +
        "} PlayerLevel;\n";

    output <<
        "\n"
        "const struct PokemonUpgradeSettings\n" +
        "{\n"s +
        indent + "//! How many power-ups a level consists of.\n" +
        indent + "uint8_t upgradesPerLevel{" + std::to_string(pokemonUpgrades.upgradesPerLevel) + "};\n" +
        indent + "//! Trainer level + allowedLevelsAbovePlayer is the maximum level for the Pokémon.\n" +
        indent + "uint8_t allowedLevelsAbovePlayer{" + std::to_string(pokemonUpgrades.allowedLevelsAbovePlayer) + "};\n" +
        indent + "//! The candy cost to upgrade from one level to the next one.\n" +
        indent + "std::array<uint8_t, " + std::to_string(numLevels) + "> candyCost" + vectorToString(pokemonUpgrades.candyCost, 10, "") + "\n" +
        indent + "//! The stardust cost to upgrade from the one level to the next one.\n" +
        indent + "std::array<uint16_t, " + std::to_string(numLevels) + "> stardustCost" + vectorToString(pokemonUpgrades.stardustCost, 10, "") + "\n" +
        "} PokemonUpgrades;\n";

    output <<
        "\n"
        "const struct WeatherBonusSettings\n" +
        "{\n"s +
        indent + "//! Maximum level increase for a weather boosted Pokémon.\n" +
        indent + "uint8_t cpBaseLevelBonus{" + std::to_string(weatherBonus.cpBaseLevelBonus) + "};\n" +
        indent + "//! The guaranteed minimum IV for a weather-boosted wild Pokémon.\n" +
        indent + "uint8_t guaranteedIndividualValues{" + std::to_string(weatherBonus.guaranteedIndividualValues) + "};\n" +
        indent + "//! Stardust bonus for a weather-boosted Pokémon.\n" +
        indent + "float stardustBonusMultiplier{" + FloatLiteralToString(weatherBonus.stardustBonusMultiplier) + "};\n" +
        indent + "//! Bonus multiplier applied to attacks with weather affinity.\n" +
        indent + "float attackBonusMultiplier{" + FloatLiteralToString(weatherBonus.attackBonusMultiplier) + "};\n" +
        indent + "//! Level increase for a weather-boosted raid encounter.\n" +
        indent + "uint8_t raidEncounterCpBaseLevelBonus{" + std::to_string(weatherBonus.raidEncounterCpBaseLevelBonus) + "};\n" +
        indent + "//! The guaranteed minimum IV for a weather-boosted raid encounter.\n" +
        indent + "uint8_t raidEncounterGuaranteedIndividualValues{" + std::to_string(weatherBonus.raidEncounterGuaranteedIndividualValues) + "};\n" +
        "} WeatherBonus;\n";

    const std::string typeNone{ "NONE" };

    //output << "enum class PokemonType : uint8_t\n{\n";
    //output << indent << typeNone << ",\n";
    //for (auto type : pokemonTypes)
    //{
    //    output << indent << type << ",\n";
    //}
    //output << "};\n\n";

    // It's very unlikely that new types are added anytime soon.
    // Fairy type was added in gen6 but it was incorporated in PoGo since day 1.
    assert(pokemonTypes.size() == numTypes);

    // Indices taken from https://github.com/BrunnerLivio/pokemongo-json-pokedex/blob/master/src/app.settings.ts
    output <<
        R"(
//! The numerical value of the enum is used to access type-effectiveness table.
enum class PokemonType : int8_t
{
    NONE = -1,
    NORMAL,
    FIGHTING,
    FLYING,
    POISON,
    GROUND,
    ROCK,
    BUG,
    GHOST,
    STEEL,
    FIRE,
    WATER,
    GRASS,
    ELECTRIC,
    PSYCHIC,
    ICE,
    DRAGON,
    DARK,
    FAIRY,
    NUM_TYPES
};

//! 0 is an invalid number.
using PokedexNumber = std::uint16_t;

struct PokemonSpecie
{
    //! Pokédex number.
    PokedexNumber number;
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
    //! How much tracked buddy walking is required for a candy, in kilometers.
    uint8_t buddyDistance;
    //! If both malePercent and femalePercent are 0, it means the Pokémon is genderless.
    float malePercent;
    float femalePercent;
};

)";

    output << "//! Type-effectiveness scalar table, use PokemonType enum to access.\n";
    output << "//! @sa AttackScalars\n";
    output << "static const std::array<std::array<float, 18>, 18> TypeEffectiveness{{\n";
    //output << "static const float TypeEffectiveness[18][18]{\n";
    for (size_t i = 0; i < numTypes; ++i)
    {
        output << indent << "{ ";
        for (size_t j = 0; j < numTypes; ++j)
        {
            auto scalar = typeEffectiveness[i][j];
            assert(!IsZero(scalar));
            output << typeEffectiveness[i][j];
            if (!Equals(scalar, 1)) output << "f";
            if (j < numTypes - 1) output << ",";
            output << " ";
        }
        output << "}";
        if (i < numTypes - 1) output << ",";
        output << "\n";
    }
    output << "}};\n\n";
    //output << "};\n\n";


    auto writePokemon = [&](const PokemonSpecieTemp& pkm)
    {
        output << "{ " << pkm.number << ", " << pkm.baseAtk << ", " << pkm.baseDef << ", " << pkm.baseSta
            << ", " << std::quoted(!pkm.formId.empty() ? pkm.formId : pkm.id) << ", PokemonType::"
            << PokemonTypeToString(pkm.type) << ", PokemonType::" << PokemonTypeToString(pkm.type2)
            << ", PokemonRarity::" << PokemonRarityToString(pkm.rarity) << ", " << (int)pkm.buddyDistance
            << ", " << pkm.malePercent << ", " << pkm.femalePercent << " }";
    };

    auto writePokemonMapEntry = [&](const PokemonTable::key_type& key, const PokemonSpecieTemp& pkm)
    {
        output << indent << "{ " << key << ", ";
        writePokemon(pkm);
        output << " },\n";
    };

    auto writeForms = [&](const auto& formRange)
    {
        //output << "{\n";
        for(auto formIt = formRange.first; formIt != formRange.second; ++formIt)
        {
            //output << indent << indent;
            writePokemonMapEntry(formIt->first, formIt->second);
            //writePokemon(it->second);
            //output << ",\n";
            //output << indent << indent;
        }
        //output << indent << "},\n";
    };

    //output << "static const std::map<uint16_t, PokemonSpecie> PokemonByNumber {\n";
    output << "//! A Pokémon can have multiple forms.\n";
    //output << "//! Use PokemonIndex() for the index in PoGoCmp::PokemonByNumber.\n";
    //output << "static const std::array<std::vector<PokemonSpecie>, " << pokemonTable.size() << "> PokemonByNumber{\n";
    output << "static const std::multimap<uint16_t, PokemonSpecie> PokemonByNumber{\n";
    for(auto it = pokemonTable.begin(), end = pokemonTable.end();
        it != end;
        it = pokemonTable.upper_bound(it->first)) // iterate only unique keys
    {
        //output << indent;
        writeForms(pokemonTable.equal_range(it->first));
    }
    output << "};\n\n";

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

struct StringLessThanI
{
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return CompareI(lhs.c_str(), rhs.c_str()) < 0;
    }
};

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
using PokedexRange = std::pair<PokedexNumber, PokedexNumber>;
const PokedexRange Gen1Range{ PokedexNumber{1}, PokedexNumber{151} };
const PokedexRange Gen2Range{ PokedexNumber{152}, PokedexNumber{251} };
const PokedexRange Gen3Range{ PokedexNumber{252}, PokedexNumber{386} };
const PokedexRange Gen4Range{ PokedexNumber{387}, PokedexNumber{493} };
const PokedexRange LatestGenRange{ Gen4Range };
//! Meltan and Melmetal
const PokedexRange UnknownRange{ PokedexNumber{808}, PokedexNumber{809} };
//! Range for known generations, unknown Pokémon appended to the end
const PokedexRange MaxRange{ PokedexNumber{1}, (PokedexNumber)PoGoCmp::PokemonByNumber.size() };
const std::array<PokedexRange, 2> ValidRanges{{
    { Gen1Range.first, LatestGenRange.second },
    { UnknownRange }
}};

//! Properties of individual Pokémon.
struct Pokemon
{
    //! Level [1,maxLevel], 0.5 steps.
    float level;
    //! Indivial value (IVs), each IV has value of [0,15].
    uint8_t atk;
    uint8_t def;
    //! @note For raid bosses this is used to store the arbitrary HP value instead of IV value.
    uint16_t sta;
};

const std::array<Pokemon, 5> RaidLevels{{
    { 21, 15, 15, 600 },
    { 25, 15, 15, 1800 },
    { 30, 15, 15, 3000 },
    { 40, 15, 15, 7500 },
    { 40, 15, 15, 12500 }
    //! @todo "level 6" (level 5 but with more health (Mewtwo in regular raids)
}};

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

//! Maps Pokémon's base name to names of its forms, if Pokémon has multiple forms.
//! Case-insensitive.
//! @sa PokemonNameToId, PokemonIdToName
static const std::multimap<std::string, std::string, StringLessThanI> FormNames{
)";
    for (const auto& it : pokemonTable)
    {
        const auto& pkm = it.second;
        if (!pkm.formId.empty())
            output << indent << "{ " << std::quoted(pkm.id) << ", " << std::quoted(pkm.formId) << " },\n" ;
    }
    output << "};";
    output <<
R"(

static inline PokemonSpecie PokemonByIdName(const std::string& id)
{
    if (auto it = std::find_if(
            PokemonByNumber.begin(), PokemonByNumber.end(),
            [&id](const auto& kvp) { return CompareI(kvp.second.id.c_str(), id.c_str()) == 0; }
        );
        it != PokemonByNumber.end())
    {
        return it->second;
    }

    if (auto it = std::find_if(
            FormNames.begin(), FormNames.end(),
            [&id](const auto& kvp) { return CompareI(kvp.first.c_str(), id.c_str()) == 0; }
        );
        it != FormNames.end())
    {
        return PokemonByIdName(it->second);
    }

    return {};
}

static inline bool IsValidIdName(const std::string& id)
{
    return PokemonByIdName(id).number != 0;
}

// Pokémon whose names don't directly match the ID name.
static const std::string MrMimeName{ "Mr. Mime" };
static const std::string FarfetchdName{ "Farfetch'd" };
static const std::string HoOhName{ "Ho-Oh" };
// Cannot use the actual ♀ & ♂ symbols in the literals as those would get corrupted
// on MSVC unless the file would be saved as UTF-16 LE BOM which I don't want to do.
static const Utf8::String NidoranFemaleName{ u8"Nidoran\u2640" };
static const Utf8::String NidoranMaleName{ u8"Nidoran\u2642" };
// N.B. "Porygon2" and "Porygon Z" inconsitency
static const Utf8::String PorygonZName{ "Porygon Z" };
// - Mime Jr. -> Unknown at the moment, probably MIME_JR
// - Flabébé -> Unknown at the moment, probably FLABEBE
static const std::string EmptyString;

//! Returns Pokémon's proper name (base, e.g. Rattata, or form, e.g. Rattata Normal) in ID name format (UPPERCASE_WITH_UNDERSOCES)
//! @note "Nidoran Female", "Nidoran Male", "Mr Mime", "Ho Oh" and "PorygonZ" accepted also.
static inline std::string PokemonNameToId(Utf8::String name)
{
    // PokemonByNumber[29]    // "NIDORAN_FEMALE
    // PokemonByNumber[32]    // "NIDORAN_MALE"
    // PokemonByNumber[83]    // "FARFETCHD"
    // PokemonByNumber[122]   // "MR_MIME"
    // PokemonByNumber[250]   // "HO_OH"
    // PokemonByNumber[474]   // "PORYGON_Z"

    if (Utf8::CompareI(name.c_str(), NidoranFemaleName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Nidoran Female") == 0)
    {
        return PokemonByNumber.find(29)->second.id;
    }
    else if (Utf8::CompareI(name.c_str(), NidoranMaleName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Nidoran Male") == 0)
    {
        return PokemonByNumber.find(32)->second.id;
    }
    else if (Utf8::CompareI(name.c_str(), FarfetchdName.c_str()) == 0)
    {
        return PokemonByNumber.find(83)->second.id;
    }
    else if (Utf8::CompareI(name.c_str(), MrMimeName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Mr Mime") == 0)
    {
        return PokemonByNumber.find(122)->second.id;
    }
    else if (Utf8::CompareI(name.c_str(), HoOhName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Ho Oh") == 0)
    {
        return PokemonByNumber.find(250)->second.id;
    }
    else if (Utf8::CompareI(name.c_str(), PorygonZName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "PorygonZ") == 0)
    {
        return PokemonByNumber.find(474)->second.id;
    }
    else
    {
        const auto& cLocale = std::locale::classic();
        std::transform(
            name.begin(), name.end(), name.begin(),
            [&cLocale](auto c)
            {
                if (c == ' ' || c == '-') return '_';
                else return std::toupper(c, cLocale);
            }
        );
        return name;
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
//! - Porygon Z -> PORYGON_Z
static inline const Utf8::String& PokemonIdToName(const std::string& name)
{
    if (CompareI(name.c_str(), "NIDORAN_FEMALE") == 0) return NidoranFemaleName;
    else if (CompareI(name.c_str(), "NIDORAN_MALE") == 0) return NidoranMaleName;
    else if (CompareI(name.c_str(), "FARFETCHD") == 0) return FarfetchdName;
    else if (CompareI(name.c_str(), "MR_MIME") == 0) return MrMimeName;
    else if (CompareI(name.c_str(), "HO_OH") == 0) return HoOhName;
    else if (CompareI(name.c_str(), "PORYGON_Z") == 0) return PorygonZName;
    else return name;
}

)";
    output << "}\n"; // ~namespace PoGoCmp
}
