/** @file PoGoDb.h
    @brief C++ API for PoGoCmp. For C one can use PoGoCmp.h.

    Input file's timestamp 2018-12-26 12:17:51+0200 */

#pragma once

#include "Utf8.h"
#include "StringUtils.h"

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

enum class PokemonRarity : uint8_t
{
    //! Unspecified/invalid.
    NONE,
    //! Can be obtained normally in the wild or from eggs, some are raid-exlusive though.
    NORMAL,
    //! Obtainable only from raids, cannot be placed in gyms.
    LEGENDARY,
    //! Obtainable e.g. from raids and special researches. Cannot typically be placed in gyms.
    MYTHIC
};

const struct PlayerLevelSettings
{
    //! The required amount of XP to level up.
    std::array<uint32_t, 40> requiredExperience{{
        0, 1000, 3000, 6000, 10000, 15000, 21000, 28000, 36000, 45000,
        55000, 65000, 75000, 85000, 100000, 120000, 140000, 160000, 185000, 210000,
        260000, 335000, 435000, 560000, 710000, 900000, 1100000, 1350000, 1650000, 2000000,
        2500000, 3000000, 3750000, 4750000, 6000000, 7500000, 9500000, 12000000, 15000000, 20000000
    }};
    //! Level cap for Pokémon from eggs.
    uint8_t  maxEggPlayerLevel{20};
    //! Level cap for Pokémon in the wild.
    //! Additional WeatherBonus.cpBaseLevelBonus can be added to this for wild encounters.
    uint8_t  maxEncounterPlayerLevel{30};
    //! Combat point (CP) multipliers for different Pokémon levels.
    std::array<float, 40> cpMultiplier{{
        0.094f, 0.166398f, 0.215732f, 0.25572f, 0.29025f, 0.321088f, 0.349213f, 0.375236f, 0.399567f, 0.4225f,
        0.443108f, 0.462798f, 0.481685f, 0.499858f, 0.517394f, 0.534354f, 0.550793f, 0.566755f, 0.582279f, 0.5974f,
        0.612157f, 0.626567f, 0.640653f, 0.654436f, 0.667934f, 0.681165f, 0.694144f, 0.706884f, 0.719399f, 0.7317f,
        0.737769f, 0.743789f, 0.749761f, 0.755686f, 0.761564f, 0.767397f, 0.773187f, 0.778933f, 0.784637f, 0.7903f
    }};
} PlayerLevel;

const struct PokemonUpgradeSettings
{
    //! How many power-ups a level consists of.
    uint8_t upgradesPerLevel{2};
    //! Trainer level + allowedLevelsAbovePlayer is the maximum level for the Pokémon.
    uint8_t allowedLevelsAbovePlayer{2};
    //! The candy cost to upgrade from one level to the next one.
    std::array<uint8_t, 40> candyCost{{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 4, 4, 4, 4, 4,
        6, 6, 8, 8, 10, 10, 12, 12, 15, 15
    }};
    //! The stardust cost to upgrade from the one level to the next one.
    std::array<uint16_t, 40> stardustCost{{
        200, 200, 400, 400, 600, 600, 800, 800, 1000, 1000,
        1300, 1300, 1600, 1600, 1900, 1900, 2200, 2200, 2500, 2500,
        3000, 3000, 3500, 3500, 4000, 4000, 4500, 4500, 5000, 5000,
        6000, 6000, 7000, 7000, 8000, 8000, 9000, 9000, 10000, 10000
    }};
} PokemonUpgrades;

const struct WeatherBonusSettings
{
    //! Maximum level increase for a weather boosted Pokémon.
    uint8_t cpBaseLevelBonus{5};
    //! The guaranteed minimum IV for a weather-boosted wild Pokémon.
    uint8_t guaranteedIndividualValues{4};
    //! Stardust bonus for a weather-boosted Pokémon.
    float stardustBonusMultiplier{1.25f};
    //! Bonus multiplier applied to attacks with weather affinity.
    float attackBonusMultiplier{1.2f};
    //! Level increase for a weather-boosted raid encounter.
    uint8_t raidEncounterCpBaseLevelBonus{5};
    //! The guaranteed minimum IV for a weather-boosted raid encounter.
    uint8_t raidEncounterGuaranteedIndividualValues{10};
} WeatherBonus;

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

//! Type-effectiveness scalar table, use PokemonType enum to access.
//! @sa AttackScalars
static const std::array<std::array<float, 18>, 18> TypeEffectiveness{{
    { 1, 1, 1, 1, 1, 0.625f, 1, 0.390625f, 0.625f, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1.6f, 1, 0.625f, 0.625f, 1, 1.6f, 0.625f, 0.390625f, 1.6f, 1, 1, 1, 1, 0.625f, 1.6f, 1, 1.6f, 0.625f },
    { 1, 1.6f, 1, 1, 1, 0.625f, 1.6f, 1, 0.625f, 1, 1, 1.6f, 0.625f, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 0.625f, 0.625f, 0.625f, 1, 0.625f, 0.390625f, 1, 1, 1.6f, 1, 1, 1, 1, 1, 1.6f },
    { 1, 1, 0.390625f, 1.6f, 1, 1.6f, 0.625f, 1, 1.6f, 1.6f, 1, 0.625f, 1.6f, 1, 1, 1, 1, 1 },
    { 1, 0.625f, 1.6f, 1, 0.625f, 1, 1.6f, 1, 0.625f, 1.6f, 1, 1, 1, 1, 1.6f, 1, 1, 1 },
    { 1, 0.625f, 0.625f, 0.625f, 1, 1, 1, 0.625f, 0.625f, 0.625f, 1, 1.6f, 1, 1.6f, 1, 1, 1.6f, 0.625f },
    { 0.390625f, 1, 1, 1, 1, 1, 1, 1.6f, 1, 1, 1, 1, 1, 1.6f, 1, 1, 0.625f, 1 },
    { 1, 1, 1, 1, 1, 1.6f, 1, 1, 0.625f, 0.625f, 0.625f, 1, 0.625f, 1, 1.6f, 1, 1, 1.6f },
    { 1, 1, 1, 1, 1, 0.625f, 1.6f, 1, 1.6f, 0.625f, 0.625f, 1.6f, 1, 1, 1.6f, 0.625f, 1, 1 },
    { 1, 1, 1, 1, 1.6f, 1.6f, 1, 1, 1, 1.6f, 0.625f, 0.625f, 1, 1, 1, 0.625f, 1, 1 },
    { 1, 1, 0.625f, 0.625f, 1.6f, 1.6f, 0.625f, 1, 0.625f, 0.625f, 1.6f, 0.625f, 1, 1, 1, 0.625f, 1, 1 },
    { 1, 1, 1.6f, 1, 0.390625f, 1, 1, 1, 1, 1, 1.6f, 0.625f, 0.625f, 1, 1, 0.625f, 1, 1 },
    { 1, 1.6f, 1, 1.6f, 1, 1, 1, 1, 0.625f, 1, 1, 1, 1, 0.625f, 1, 1, 0.390625f, 1 },
    { 1, 1, 1.6f, 1, 1.6f, 1, 1, 1, 0.625f, 0.625f, 0.625f, 1.6f, 1, 1, 0.625f, 1.6f, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 0.625f, 1, 1, 1, 1, 1, 1, 1.6f, 1, 0.390625f },
    { 1, 0.625f, 1, 1, 1, 1, 1, 1.6f, 1, 1, 1, 1, 1, 1.6f, 1, 1, 0.625f, 0.625f },
    { 1, 1.6f, 1, 0.625f, 1, 1, 1, 1, 0.625f, 0.625f, 1, 1, 1, 1, 1, 1.6f, 1.6f, 1 }
}};

//! A Pokémon can have multiple forms.
static const std::multimap<uint16_t, PokemonSpecie> PokemonByNumber{
    { 1, { 1, 118, 111, 128, "BULBASAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 2, { 2, 151, 143, 155, "IVYSAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 3, { 3, 198, 189, 190, "VENUSAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 4, { 4, 116, 93, 118, "CHARMANDER", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 5, { 5, 158, 126, 151, "CHARMELEON", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 6, { 6, 223, 173, 186, "CHARIZARD", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 7, { 7, 94, 121, 127, "SQUIRTLE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 8, { 8, 126, 155, 153, "WARTORTLE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 9, { 9, 171, 207, 188, "BLASTOISE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 10, { 10, 55, 55, 128, "CATERPIE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 11, { 11, 45, 80, 137, "METAPOD", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 12, { 12, 167, 137, 155, "BUTTERFREE", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 13, { 13, 63, 50, 120, "WEEDLE", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 14, { 14, 46, 75, 128, "KAKUNA", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 15, { 15, 169, 130, 163, "BEEDRILL", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 16, { 16, 85, 73, 120, "PIDGEY", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 17, { 17, 117, 105, 160, "PIDGEOTTO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 18, { 18, 166, 154, 195, "PIDGEOT", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 19, { 19, 103, 70, 102, "RATTATA_NORMAL", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 19, { 19, 103, 70, 102, "RATTATA_ALOLA", PokemonType::DARK, PokemonType::NORMAL, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 20, { 20, 161, 139, 146, "RATICATE_NORMAL", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 20, { 20, 135, 154, 181, "RATICATE_ALOLA", PokemonType::DARK, PokemonType::NORMAL, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 21, { 21, 112, 60, 120, "SPEAROW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 22, { 22, 182, 133, 163, "FEAROW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 23, { 23, 110, 97, 111, "EKANS", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 24, { 24, 167, 153, 155, "ARBOK", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 25, { 25, 112, 96, 111, "PIKACHU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 26, { 26, 193, 151, 155, "RAICHU_NORMAL", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 26, { 26, 201, 154, 155, "RAICHU_ALOLA", PokemonType::ELECTRIC, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 27, { 27, 126, 120, 137, "SANDSHREW_NORMAL", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 27, { 27, 125, 129, 137, "SANDSHREW_ALOLA", PokemonType::ICE, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 28, { 28, 182, 175, 181, "SANDSLASH_NORMAL", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 28, { 28, 177, 195, 181, "SANDSLASH_ALOLA", PokemonType::ICE, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 29, { 29, 86, 89, 146, "NIDORAN_FEMALE", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 5000, 100 } },
    { 30, { 30, 117, 120, 172, "NIDORINA", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 } },
    { 31, { 31, 180, 173, 207, "NIDOQUEEN", PokemonType::POISON, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 0, 100 } },
    { 32, { 32, 105, 76, 130, "NIDORAN_MALE", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 } },
    { 33, { 33, 137, 111, 156, "NIDORINO", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 } },
    { 34, { 34, 204, 156, 191, "NIDOKING", PokemonType::POISON, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 100, 0 } },
    { 35, { 35, 107, 108, 172, "CLEFAIRY", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 25, 75 } },
    { 36, { 36, 178, 162, 216, "CLEFABLE", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 25, 75 } },
    { 37, { 37, 96, 109, 116, "VULPIX_NORMAL", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 37, { 37, 96, 109, 116, "VULPIX_ALOLA", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 38, { 38, 169, 190, 177, "NINETALES_NORMAL", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 38, { 38, 170, 193, 177, "NINETALES_ALOLA", PokemonType::ICE, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 39, { 39, 80, 41, 251, "JIGGLYPUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 25, 75 } },
    { 40, { 40, 156, 90, 295, "WIGGLYTUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 25, 75 } },
    { 41, { 41, 83, 73, 120, "ZUBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 42, { 42, 161, 150, 181, "GOLBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 43, { 43, 131, 112, 128, "ODDISH", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 44, { 44, 153, 136, 155, "GLOOM", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 45, { 45, 202, 167, 181, "VILEPLUME", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 46, { 46, 121, 99, 111, "PARAS", PokemonType::BUG, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 47, { 47, 165, 146, 155, "PARASECT", PokemonType::BUG, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 48, { 48, 100, 100, 155, "VENONAT", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 49, { 49, 179, 143, 172, "VENOMOTH", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 50, { 50, 109, 78, 67, "DIGLETT_NORMAL", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 50, { 50, 108, 81, 67, "DIGLETT_ALOLA", PokemonType::GROUND, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 51, { 51, 167, 136, 111, "DUGTRIO_NORMAL", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 51, { 51, 201, 142, 111, "DUGTRIO_ALOLA", PokemonType::GROUND, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 52, { 52, 92, 78, 120, "MEOWTH_NORMAL", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 52, { 52, 99, 78, 120, "MEOWTH_ALOLA", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 53, { 53, 150, 136, 163, "PERSIAN_NORMAL", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 53, { 53, 158, 136, 163, "PERSIAN_ALOLA", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 54, { 54, 122, 95, 137, "PSYDUCK", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 55, { 55, 191, 162, 190, "GOLDUCK", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 56, { 56, 148, 82, 120, "MANKEY", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 57, { 57, 207, 138, 163, "PRIMEAPE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 58, { 58, 136, 93, 146, "GROWLITHE", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 59, { 59, 227, 166, 207, "ARCANINE", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 60, { 60, 101, 82, 120, "POLIWAG", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 61, { 61, 130, 123, 163, "POLIWHIRL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 62, { 62, 182, 184, 207, "POLIWRATH", PokemonType::WATER, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 63, { 63, 195, 82, 93, "ABRA", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 64, { 64, 232, 117, 120, "KADABRA", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 65, { 65, 271, 167, 146, "ALAKAZAM", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 66, { 66, 137, 82, 172, "MACHOP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 67, { 67, 177, 125, 190, "MACHOKE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 68, { 68, 234, 159, 207, "MACHAMP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 69, { 69, 139, 61, 137, "BELLSPROUT", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 70, { 70, 172, 92, 163, "WEEPINBELL", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 71, { 71, 207, 135, 190, "VICTREEBEL", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 72, { 72, 97, 149, 120, "TENTACOOL", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 73, { 73, 166, 209, 190, "TENTACRUEL", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 74, { 74, 132, 132, 120, "GEODUDE_NORMAL", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 74, { 74, 132, 132, 120, "GEODUDE_ALOLA", PokemonType::ROCK, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 75, { 75, 164, 164, 146, "GRAVELER_NORMAL", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 75, { 75, 164, 164, 146, "GRAVELER_ALOLA", PokemonType::ROCK, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 76, { 76, 211, 198, 190, "GOLEM_NORMAL", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 76, { 76, 211, 198, 190, "GOLEM_ALOLA", PokemonType::ROCK, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 77, { 77, 170, 127, 137, "PONYTA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 78, { 78, 207, 162, 163, "RAPIDASH", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 79, { 79, 109, 98, 207, "SLOWPOKE", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 80, { 80, 177, 180, 216, "SLOWBRO", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 81, { 81, 165, 121, 93, "MAGNEMITE", PokemonType::ELECTRIC, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 82, { 82, 223, 169, 137, "MAGNETON", PokemonType::ELECTRIC, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 83, { 83, 124, 115, 141, "FARFETCHD", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 84, { 84, 158, 83, 111, "DODUO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 85, { 85, 218, 140, 155, "DODRIO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 86, { 86, 85, 121, 163, "SEEL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 87, { 87, 139, 177, 207, "DEWGONG", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 88, { 88, 135, 90, 190, "GRIMER_NORMAL", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 88, { 88, 135, 90, 190, "GRIMER_ALOLA", PokemonType::POISON, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 89, { 89, 190, 172, 233, "MUK_NORMAL", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 89, { 89, 190, 172, 233, "MUK_ALOLA", PokemonType::POISON, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 90, { 90, 116, 134, 102, "SHELLDER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 91, { 91, 186, 256, 137, "CLOYSTER", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 92, { 92, 186, 67, 102, "GASTLY", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 93, { 93, 223, 107, 128, "HAUNTER", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 94, { 94, 261, 149, 155, "GENGAR", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 95, { 95, 85, 232, 111, "ONIX", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 96, { 96, 89, 136, 155, "DROWZEE", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 97, { 97, 144, 193, 198, "HYPNO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 98, { 98, 181, 124, 102, "KRABBY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 99, { 99, 240, 181, 146, "KINGLER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 100, { 100, 109, 111, 120, "VOLTORB", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 101, { 101, 173, 173, 155, "ELECTRODE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 102, { 102, 107, 125, 155, "EXEGGCUTE", PokemonType::GRASS, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 103, { 103, 233, 149, 216, "EXEGGUTOR_NORMAL", PokemonType::GRASS, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 103, { 103, 230, 153, 216, "EXEGGUTOR_ALOLA", PokemonType::GRASS, PokemonType::DRAGON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 104, { 104, 90, 144, 137, "CUBONE", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 105, { 105, 144, 186, 155, "MAROWAK_NORMAL", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 105, { 105, 144, 186, 155, "MAROWAK_ALOLA", PokemonType::FIRE, PokemonType::GHOST, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 106, { 106, 224, 181, 137, "HITMONLEE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 5000 } },
    { 107, { 107, 193, 197, 137, "HITMONCHAN", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 } },
    { 108, { 108, 108, 137, 207, "LICKITUNG", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 109, { 109, 119, 141, 120, "KOFFING", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 110, { 110, 174, 197, 163, "WEEZING", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 111, { 111, 140, 127, 190, "RHYHORN", PokemonType::GROUND, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 112, { 112, 222, 171, 233, "RHYDON", PokemonType::GROUND, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 113, { 113, 60, 128, 487, "CHANSEY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 } },
    { 114, { 114, 183, 169, 163, "TANGELA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 115, { 115, 181, 165, 233, "KANGASKHAN", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 } },
    { 116, { 116, 129, 103, 102, "HORSEA", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 117, { 117, 187, 156, 146, "SEADRA", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 118, { 118, 123, 110, 128, "GOLDEEN", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 119, { 119, 175, 147, 190, "SEAKING", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 120, { 120, 137, 112, 102, "STARYU", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 121, { 121, 210, 184, 155, "STARMIE", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 122, { 122, 192, 205, 120, "MR_MIME", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 123, { 123, 218, 170, 172, "SCYTHER", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 124, { 124, 223, 151, 163, "JYNX", PokemonType::ICE, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 100 } },
    { 125, { 125, 198, 158, 163, "ELECTABUZZ", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 75, 25 } },
    { 126, { 126, 206, 154, 163, "MAGMAR", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 75, 25 } },
    { 127, { 127, 238, 182, 163, "PINSIR", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 128, { 128, 198, 183, 181, "TAUROS", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 } },
    { 129, { 129, 29, 85, 85, "MAGIKARP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 130, { 130, 237, 186, 216, "GYARADOS", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 131, { 131, 165, 174, 277, "LAPRAS", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 132, { 132, 91, 91, 134, "DITTO", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 133, { 133, 104, 114, 146, "EEVEE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 134, { 134, 205, 161, 277, "VAPOREON", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 135, { 135, 232, 182, 163, "JOLTEON", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 136, { 136, 246, 179, 163, "FLAREON", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 137, { 137, 153, 136, 163, "PORYGON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 138, { 138, 155, 153, 111, "OMANYTE", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 139, { 139, 207, 201, 172, "OMASTAR", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 140, { 140, 148, 140, 102, "KABUTO", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 141, { 141, 220, 186, 155, "KABUTOPS", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 142, { 142, 221, 159, 190, "AERODACTYL", PokemonType::ROCK, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 143, { 143, 190, 169, 330, "SNORLAX", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 144, { 144, 192, 236, 207, "ARTICUNO", PokemonType::ICE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 145, { 145, 253, 185, 207, "ZAPDOS", PokemonType::ELECTRIC, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 146, { 146, 251, 181, 207, "MOLTRES", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 147, { 147, 119, 91, 121, "DRATINI", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 148, { 148, 163, 135, 156, "DRAGONAIR", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 149, { 149, 263, 198, 209, "DRAGONITE", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 150, { 150, 300, 182, 214, "MEWTWO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 151, { 151, 210, 210, 225, "MEW", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 152, { 152, 92, 122, 128, "CHIKORITA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 153, { 153, 122, 155, 155, "BAYLEEF", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 154, { 154, 168, 202, 190, "MEGANIUM", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 155, { 155, 116, 93, 118, "CYNDAQUIL", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 156, { 156, 158, 126, 151, "QUILAVA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 157, { 157, 223, 173, 186, "TYPHLOSION", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 158, { 158, 117, 109, 137, "TOTODILE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 159, { 159, 150, 142, 163, "CROCONAW", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 160, { 160, 205, 188, 198, "FERALIGATR", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 161, { 161, 79, 73, 111, "SENTRET", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 162, { 162, 148, 125, 198, "FURRET", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 163, { 163, 67, 88, 155, "HOOTHOOT", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 164, { 164, 145, 156, 225, "NOCTOWL", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 165, { 165, 72, 118, 120, "LEDYBA", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 166, { 166, 107, 179, 146, "LEDIAN", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 167, { 167, 105, 73, 120, "SPINARAK", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 168, { 168, 161, 124, 172, "ARIADOS", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 169, { 169, 194, 178, 198, "CROBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 170, { 170, 106, 97, 181, "CHINCHOU", PokemonType::WATER, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 171, { 171, 146, 137, 268, "LANTURN", PokemonType::WATER, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 172, { 172, 77, 53, 85, "PICHU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 173, { 173, 75, 79, 137, "CLEFFA", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 25, 75 } },
    { 174, { 174, 69, 32, 207, "IGGLYBUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 25, 75 } },
    { 175, { 175, 67, 116, 111, "TOGEPI", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 176, { 176, 139, 181, 146, "TOGETIC", PokemonType::FAIRY, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 177, { 177, 134, 89, 120, "NATU", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 178, { 178, 192, 146, 163, "XATU", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 179, { 179, 114, 79, 146, "MAREEP", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 180, { 180, 145, 109, 172, "FLAAFFY", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 181, { 181, 211, 169, 207, "AMPHAROS", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 182, { 182, 169, 186, 181, "BELLOSSOM", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 183, { 183, 37, 93, 172, "MARILL", PokemonType::WATER, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 184, { 184, 112, 152, 225, "AZUMARILL", PokemonType::WATER, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 185, { 185, 167, 176, 172, "SUDOWOODO", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 186, { 186, 174, 179, 207, "POLITOED", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 187, { 187, 67, 94, 111, "HOPPIP", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 188, { 188, 91, 120, 146, "SKIPLOOM", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 189, { 189, 118, 183, 181, "JUMPLUFF", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 190, { 190, 136, 112, 146, "AIPOM", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 191, { 191, 55, 55, 102, "SUNKERN", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 192, { 192, 185, 135, 181, "SUNFLORA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 193, { 193, 154, 94, 163, "YANMA", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 194, { 194, 75, 66, 146, "WOOPER", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 195, { 195, 152, 143, 216, "QUAGSIRE", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 196, { 196, 261, 175, 163, "ESPEON", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 197, { 197, 126, 240, 216, "UMBREON", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 198, { 198, 175, 87, 155, "MURKROW", PokemonType::DARK, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 199, { 199, 177, 180, 216, "SLOWKING", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 200, { 200, 167, 154, 155, "MISDREAVUS", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 201, { 201, 136, 91, 134, "UNOWN_F", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_A", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_B", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_C", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_D", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_E", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_G", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_H", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_I", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_J", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_K", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_L", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_M", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_N", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_O", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_P", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_Q", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_R", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_S", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_T", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_U", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_V", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_W", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_X", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_Y", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_Z", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_EXCLAMATION_POINT", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 201, { 201, 136, 91, 134, "UNOWN_QUESTION_MARK", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 202, { 202, 60, 106, 382, "WOBBUFFET", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 203, { 203, 182, 133, 172, "GIRAFARIG", PokemonType::NORMAL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 204, { 204, 108, 122, 137, "PINECO", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 205, { 205, 161, 205, 181, "FORRETRESS", PokemonType::BUG, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 206, { 206, 131, 128, 225, "DUNSPARCE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 207, { 207, 143, 184, 163, "GLIGAR", PokemonType::GROUND, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 208, { 208, 148, 272, 181, "STEELIX", PokemonType::STEEL, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 209, { 209, 137, 85, 155, "SNUBBULL", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 210, { 210, 212, 131, 207, "GRANBULL", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 211, { 211, 184, 138, 163, "QWILFISH", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 212, { 212, 236, 181, 172, "SCIZOR", PokemonType::BUG, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 213, { 213, 17, 396, 85, "SHUCKLE", PokemonType::BUG, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 214, { 214, 234, 179, 190, "HERACROSS", PokemonType::BUG, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 215, { 215, 189, 146, 146, "SNEASEL", PokemonType::DARK, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 216, { 216, 142, 93, 155, "TEDDIURSA", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 217, { 217, 236, 144, 207, "URSARING", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 218, { 218, 118, 71, 120, "SLUGMA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 219, { 219, 139, 191, 137, "MAGCARGO", PokemonType::FIRE, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 220, { 220, 90, 69, 137, "SWINUB", PokemonType::ICE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 221, { 221, 181, 138, 225, "PILOSWINE", PokemonType::ICE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 222, { 222, 118, 156, 146, "CORSOLA", PokemonType::WATER, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 223, { 223, 127, 69, 111, "REMORAID", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 224, { 224, 197, 141, 181, "OCTILLERY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 225, { 225, 128, 90, 128, "DELIBIRD", PokemonType::ICE, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 226, { 226, 148, 226, 163, "MANTINE", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 227, { 227, 148, 226, 163, "SKARMORY", PokemonType::STEEL, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 228, { 228, 152, 83, 128, "HOUNDOUR", PokemonType::DARK, PokemonType::FIRE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 229, { 229, 224, 144, 181, "HOUNDOOM", PokemonType::DARK, PokemonType::FIRE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 230, { 230, 194, 194, 181, "KINGDRA", PokemonType::WATER, PokemonType::DRAGON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 231, { 231, 107, 98, 207, "PHANPY", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 232, { 232, 214, 185, 207, "DONPHAN", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 233, { 233, 198, 180, 198, "PORYGON2", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 234, { 234, 192, 131, 177, "STANTLER", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 235, { 235, 40, 83, 146, "SMEARGLE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 236, { 236, 64, 64, 111, "TYROGUE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 } },
    { 237, { 237, 173, 207, 137, "HITMONTOP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 } },
    { 238, { 238, 153, 91, 128, "SMOOCHUM", PokemonType::ICE, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 100 } },
    { 239, { 239, 135, 101, 128, "ELEKID", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 75, 25 } },
    { 240, { 240, 151, 99, 128, "MAGBY", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 75, 25 } },
    { 241, { 241, 157, 193, 216, "MILTANK", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 } },
    { 242, { 242, 129, 169, 496, "BLISSEY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 } },
    { 243, { 243, 241, 195, 207, "RAIKOU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 244, { 244, 235, 171, 251, "ENTEI", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 245, { 245, 180, 235, 225, "SUICUNE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 246, { 246, 115, 93, 137, "LARVITAR", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 247, { 247, 155, 133, 172, "PUPITAR", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 248, { 248, 251, 207, 225, "TYRANITAR", PokemonType::ROCK, PokemonType::DARK, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 249, { 249, 193, 310, 235, "LUGIA", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 250, { 250, 239, 244, 214, "HO_OH", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 251, { 251, 210, 210, 225, "CELEBI", PokemonType::PSYCHIC, PokemonType::GRASS, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 252, { 252, 124, 94, 120, "TREECKO", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 253, { 253, 172, 120, 137, "GROVYLE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 254, { 254, 223, 169, 172, "SCEPTILE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 255, { 255, 130, 87, 128, "TORCHIC", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 256, { 256, 163, 115, 155, "COMBUSKEN", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 257, { 257, 240, 141, 190, "BLAZIKEN", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 258, { 258, 126, 93, 137, "MUDKIP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 259, { 259, 156, 133, 172, "MARSHTOMP", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 260, { 260, 208, 175, 225, "SWAMPERT", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 261, { 261, 96, 61, 111, "POOCHYENA", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 262, { 262, 171, 132, 172, "MIGHTYENA", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 263, { 263, 58, 80, 116, "ZIGZAGOON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 264, { 264, 142, 128, 186, "LINOONE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 265, { 265, 75, 59, 128, "WURMPLE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 266, { 266, 60, 77, 137, "SILCOON", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 267, { 267, 189, 98, 155, "BEAUTIFLY", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 268, { 268, 60, 77, 137, "CASCOON", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 269, { 269, 98, 162, 155, "DUSTOX", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 270, { 270, 71, 77, 120, "LOTAD", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 271, { 271, 112, 119, 155, "LOMBRE", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 272, { 272, 173, 176, 190, "LUDICOLO", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 273, { 273, 71, 77, 120, "SEEDOT", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 274, { 274, 134, 78, 172, "NUZLEAF", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 275, { 275, 200, 121, 207, "SHIFTRY", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 276, { 276, 106, 61, 120, "TAILLOW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 277, { 277, 185, 124, 155, "SWELLOW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 278, { 278, 106, 61, 120, "WINGULL", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 279, { 279, 175, 174, 155, "PELIPPER", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 280, { 280, 79, 59, 99, "RALTS", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 281, { 281, 117, 90, 116, "KIRLIA", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 282, { 282, 237, 195, 169, "GARDEVOIR", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 283, { 283, 93, 87, 120, "SURSKIT", PokemonType::BUG, PokemonType::WATER, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 284, { 284, 192, 150, 172, "MASQUERAIN", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 285, { 285, 74, 110, 155, "SHROOMISH", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 286, { 286, 241, 144, 155, "BRELOOM", PokemonType::GRASS, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 287, { 287, 104, 92, 155, "SLAKOTH", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 288, { 288, 159, 145, 190, "VIGOROTH", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 289, { 289, 290, 166, 284, "SLAKING", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 290, { 290, 80, 126, 104, "NINCADA", PokemonType::BUG, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 291, { 291, 199, 112, 156, "NINJASK", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 292, { 292, 153, 73, 1, "SHEDINJA", PokemonType::BUG, PokemonType::GHOST, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 293, { 293, 92, 42, 162, "WHISMUR", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 294, { 294, 134, 81, 197, "LOUDRED", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 295, { 295, 179, 137, 232, "EXPLOUD", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 296, { 296, 99, 54, 176, "MAKUHITA", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 297, { 297, 209, 114, 302, "HARIYAMA", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 298, { 298, 36, 71, 137, "AZURILL", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 299, { 299, 82, 215, 102, "NOSEPASS", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 300, { 300, 84, 79, 137, "SKITTY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 301, { 301, 132, 127, 172, "DELCATTY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 302, { 302, 141, 136, 137, "SABLEYE", PokemonType::DARK, PokemonType::GHOST, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 303, { 303, 155, 141, 137, "MAWILE", PokemonType::STEEL, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 304, { 304, 121, 141, 137, "ARON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 305, { 305, 158, 198, 155, "LAIRON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 306, { 306, 198, 257, 172, "AGGRON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 307, { 307, 78, 107, 102, "MEDITITE", PokemonType::FIGHTING, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 308, { 308, 121, 152, 155, "MEDICHAM", PokemonType::FIGHTING, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 309, { 309, 123, 78, 120, "ELECTRIKE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 310, { 310, 215, 127, 172, "MANECTRIC", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 311, { 311, 167, 129, 155, "PLUSLE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 312, { 312, 147, 150, 155, "MINUN", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 313, { 313, 143, 166, 163, "VOLBEAT", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 } },
    { 314, { 314, 143, 166, 163, "ILLUMISE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 } },
    { 315, { 315, 186, 131, 137, "ROSELIA", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 316, { 316, 80, 99, 172, "GULPIN", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 317, { 317, 140, 159, 225, "SWALOT", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 318, { 318, 171, 39, 128, "CARVANHA", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 319, { 319, 243, 83, 172, "SHARPEDO", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 320, { 320, 136, 68, 277, "WAILMER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 321, { 321, 175, 87, 347, "WAILORD", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 322, { 322, 119, 79, 155, "NUMEL", PokemonType::FIRE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 323, { 323, 194, 136, 172, "CAMERUPT", PokemonType::FIRE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 324, { 324, 151, 203, 172, "TORKOAL", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 325, { 325, 125, 122, 155, "SPOINK", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 326, { 326, 171, 188, 190, "GRUMPIG", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 327, { 327, 116, 116, 155, "SPINDA_00", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 327, { 327, 116, 116, 155, "SPINDA_01", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 327, { 327, 116, 116, 155, "SPINDA_02", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 327, { 327, 116, 116, 155, "SPINDA_03", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 327, { 327, 116, 116, 155, "SPINDA_04", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 327, { 327, 116, 116, 155, "SPINDA_05", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 327, { 327, 116, 116, 155, "SPINDA_06", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 327, { 327, 116, 116, 155, "SPINDA_07", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 328, { 328, 162, 78, 128, "TRAPINCH", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 329, { 329, 134, 99, 137, "VIBRAVA", PokemonType::GROUND, PokemonType::DRAGON, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 330, { 330, 205, 168, 190, "FLYGON", PokemonType::GROUND, PokemonType::DRAGON, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 331, { 331, 156, 74, 137, "CACNEA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 332, { 332, 221, 115, 172, "CACTURNE", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 333, { 333, 76, 132, 128, "SWABLU", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 334, { 334, 141, 201, 181, "ALTARIA", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 335, { 335, 222, 124, 177, "ZANGOOSE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 336, { 336, 196, 118, 177, "SEVIPER", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 337, { 337, 178, 153, 207, "LUNATONE", PokemonType::ROCK, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 338, { 338, 178, 153, 207, "SOLROCK", PokemonType::ROCK, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 339, { 339, 93, 82, 137, "BARBOACH", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 340, { 340, 151, 141, 242, "WHISCASH", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 341, { 341, 141, 99, 125, "CORPHISH", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 342, { 342, 224, 142, 160, "CRAWDAUNT", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 343, { 343, 77, 124, 120, "BALTOY", PokemonType::GROUND, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 344, { 344, 140, 229, 155, "CLAYDOL", PokemonType::GROUND, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 345, { 345, 105, 150, 165, "LILEEP", PokemonType::ROCK, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 346, { 346, 152, 194, 200, "CRADILY", PokemonType::ROCK, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 347, { 347, 176, 100, 128, "ANORITH", PokemonType::ROCK, PokemonType::BUG, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 348, { 348, 222, 174, 181, "ARMALDO", PokemonType::ROCK, PokemonType::BUG, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 349, { 349, 29, 85, 85, "FEEBAS", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 350, { 350, 192, 219, 216, "MILOTIC", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 351, { 351, 139, 139, 172, "CASTFORM_NORMAL", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 351, { 351, 139, 139, 172, "CASTFORM_SUNNY", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 351, { 351, 139, 139, 172, "CASTFORM_RAINY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 351, { 351, 139, 139, 172, "CASTFORM_SNOWY", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 352, { 352, 161, 189, 155, "KECLEON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 353, { 353, 138, 65, 127, "SHUPPET", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 354, { 354, 218, 126, 162, "BANETTE", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 355, { 355, 70, 162, 85, "DUSKULL", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 356, { 356, 124, 234, 120, "DUSCLOPS", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 357, { 357, 136, 163, 223, "TROPIUS", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 358, { 358, 175, 170, 181, "CHIMECHO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 359, { 359, 246, 120, 163, "ABSOL", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 360, { 360, 41, 86, 216, "WYNAUT", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 361, { 361, 95, 95, 137, "SNORUNT", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 362, { 362, 162, 162, 190, "GLALIE", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 363, { 363, 95, 90, 172, "SPHEAL", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 364, { 364, 137, 132, 207, "SEALEO", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 365, { 365, 182, 176, 242, "WALREIN", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 366, { 366, 133, 135, 111, "CLAMPERL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 367, { 367, 197, 179, 146, "HUNTAIL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 368, { 368, 211, 179, 146, "GOREBYSS", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 369, { 369, 162, 203, 225, "RELICANTH", PokemonType::WATER, PokemonType::ROCK, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 370, { 370, 81, 128, 125, "LUVDISC", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 371, { 371, 134, 93, 128, "BAGON", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 372, { 372, 172, 155, 163, "SHELGON", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 373, { 373, 277, 168, 216, "SALAMENCE", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 374, { 374, 96, 132, 120, "BELDUM", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 375, { 375, 138, 176, 155, "METANG", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 376, { 376, 257, 228, 190, "METAGROSS", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 377, { 377, 179, 309, 190, "REGIROCK", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 378, { 378, 179, 309, 190, "REGICE", PokemonType::ICE, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 379, { 379, 143, 285, 190, "REGISTEEL", PokemonType::STEEL, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 380, { 380, 228, 246, 190, "LATIAS", PokemonType::DRAGON, PokemonType::PSYCHIC, PokemonRarity::LEGENDARY, 20, 0, 100 } },
    { 381, { 381, 268, 212, 190, "LATIOS", PokemonType::DRAGON, PokemonType::PSYCHIC, PokemonRarity::LEGENDARY, 20, 100, 0 } },
    { 382, { 382, 270, 228, 205, "KYOGRE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 383, { 383, 270, 228, 205, "GROUDON", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 384, { 384, 284, 170, 213, "RAYQUAZA", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 385, { 385, 210, 210, 225, "JIRACHI", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 386, { 386, 345, 115, 137, "DEOXYS_NORMAL", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 386, { 386, 414, 46, 137, "DEOXYS_ATTACK", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 386, { 386, 144, 330, 137, "DEOXYS_DEFENSE", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 386, { 386, 230, 218, 137, "DEOXYS_SPEED", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 387, { 387, 119, 110, 146, "TURTWIG", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 388, { 388, 157, 143, 181, "GROTLE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 389, { 389, 202, 188, 216, "TORTERRA", PokemonType::GRASS, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 390, { 390, 113, 86, 127, "CHIMCHAR", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 391, { 391, 158, 105, 162, "MONFERNO", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 392, { 392, 222, 151, 183, "INFERNAPE", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 393, { 393, 112, 102, 142, "PIPLUP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 394, { 394, 150, 139, 162, "PRINPLUP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 395, { 395, 210, 186, 197, "EMPOLEON", PokemonType::WATER, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 396, { 396, 101, 58, 120, "STARLY", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 397, { 397, 142, 94, 146, "STARAVIA", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 398, { 398, 234, 140, 198, "STARAPTOR", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 399, { 399, 80, 73, 153, "BIDOOF", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 400, { 400, 162, 119, 188, "BIBAREL", PokemonType::NORMAL, PokemonType::WATER, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 401, { 401, 45, 74, 114, "KRICKETOT", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 402, { 402, 160, 100, 184, "KRICKETUNE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 403, { 403, 117, 64, 128, "SHINX", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 404, { 404, 159, 95, 155, "LUXIO", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 405, { 405, 232, 156, 190, "LUXRAY", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 406, { 406, 91, 109, 120, "BUDEW", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 407, { 407, 243, 185, 155, "ROSERADE", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 408, { 408, 218, 71, 167, "CRANIDOS", PokemonType::ROCK, PokemonType::ROCK, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 409, { 409, 295, 109, 219, "RAMPARDOS", PokemonType::ROCK, PokemonType::ROCK, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 410, { 410, 76, 195, 102, "SHIELDON", PokemonType::ROCK, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 411, { 411, 94, 286, 155, "BASTIODON", PokemonType::ROCK, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 412, { 412, 53, 83, 120, "BURMY_PLANT", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 412, { 412, 53, 83, 120, "BURMY_SANDY", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 412, { 412, 53, 83, 120, "BURMY_TRASH", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 413, { 413, 141, 180, 155, "WORMADAM_PLANT", PokemonType::BUG, PokemonType::GRASS, PokemonRarity::NORMAL, 1, 5000, 100 } },
    { 413, { 413, 141, 180, 155, "WORMADAM_SANDY", PokemonType::BUG, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 0, 100 } },
    { 413, { 413, 127, 175, 155, "WORMADAM_TRASH", PokemonType::BUG, PokemonType::STEEL, PokemonRarity::NORMAL, 1, 0, 100 } },
    { 414, { 414, 185, 98, 172, "MOTHIM", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 100, 10000 } },
    { 415, { 415, 59, 83, 102, "COMBEE", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 416, { 416, 149, 190, 172, "VESPIQUEN", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 0, 100 } },
    { 417, { 417, 94, 172, 155, "PACHIRISU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 418, { 418, 132, 67, 146, "BUIZEL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 419, { 419, 221, 114, 198, "FLOATZEL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 420, { 420, 108, 92, 128, "CHERUBI", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 421, { 421, 170, 153, 172, "CHERRIM_OVERCAST", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 421, { 421, 170, 153, 172, "CHERRIM_SUNNY", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 422, { 422, 103, 105, 183, "SHELLOS_WEST_SEA", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 422, { 422, 103, 105, 183, "SHELLOS_EAST_SEA", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 423, { 423, 169, 143, 244, "GASTRODON_WEST_SEA", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 423, { 423, 169, 143, 244, "GASTRODON_EAST_SEA", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 424, { 424, 205, 143, 181, "AMBIPOM", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 425, { 425, 117, 80, 207, "DRIFLOON", PokemonType::GHOST, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 426, { 426, 180, 102, 312, "DRIFBLIM", PokemonType::GHOST, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 427, { 427, 130, 105, 146, "BUNEARY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 428, { 428, 156, 194, 163, "LOPUNNY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 429, { 429, 211, 187, 155, "MISMAGIUS", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 430, { 430, 243, 103, 225, "HONCHKROW", PokemonType::DARK, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 431, { 431, 109, 82, 135, "GLAMEOW", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 432, { 432, 172, 133, 174, "PURUGLY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 } },
    { 433, { 433, 114, 94, 128, "CHINGLING", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 434, { 434, 121, 90, 160, "STUNKY", PokemonType::POISON, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 435, { 435, 184, 132, 230, "SKUNTANK", PokemonType::POISON, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 436, { 436, 43, 154, 149, "BRONZOR", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 437, { 437, 161, 213, 167, "BRONZONG", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 438, { 438, 124, 133, 137, "BONSLY", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 439, { 439, 125, 142, 85, "MIME_JR", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 440, { 440, 25, 77, 225, "HAPPINY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 } },
    { 441, { 441, 183, 91, 183, "CHATOT", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 442, { 442, 169, 199, 137, "SPIRITOMB", PokemonType::GHOST, PokemonType::DARK, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 443, { 443, 124, 84, 151, "GIBLE", PokemonType::DRAGON, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 444, { 444, 172, 125, 169, "GABITE", PokemonType::DRAGON, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 445, { 445, 261, 193, 239, "GARCHOMP", PokemonType::DRAGON, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 446, { 446, 137, 117, 286, "MUNCHLAX", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 447, { 447, 127, 78, 120, "RIOLU", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 448, { 448, 236, 144, 172, "LUCARIO", PokemonType::FIGHTING, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 87.5, 12.5 } },
    { 449, { 449, 124, 118, 169, "HIPPOPOTAS", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 450, { 450, 201, 191, 239, "HIPPOWDON", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 } },
    { 451, { 451, 93, 151, 120, "SKORUPI", PokemonType::POISON, PokemonType::BUG, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 452, { 452, 180, 202, 172, "DRAPION", PokemonType::POISON, PokemonType::DARK, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 453, { 453, 116, 76, 134, "CROAGUNK", PokemonType::POISON, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 454, { 454, 211, 133, 195, "TOXICROAK", PokemonType::POISON, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 455, { 455, 187, 136, 179, "CARNIVINE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 456, { 456, 96, 116, 135, "FINNEON", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 457, { 457, 142, 170, 170, "LUMINEON", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 458, { 458, 105, 179, 128, "MANTYKE", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 459, { 459, 115, 105, 155, "SNOVER", PokemonType::GRASS, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 460, { 460, 178, 158, 207, "ABOMASNOW", PokemonType::GRASS, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 461, { 461, 243, 171, 172, "WEAVILE", PokemonType::DARK, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 462, { 462, 238, 205, 172, "MAGNEZONE", PokemonType::ELECTRIC, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 463, { 463, 161, 181, 242, "LICKILICKY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 464, { 464, 241, 190, 251, "RHYPERIOR", PokemonType::GROUND, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 465, { 465, 207, 184, 225, "TANGROWTH", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 466, { 466, 249, 163, 181, "ELECTIVIRE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 467, { 467, 247, 172, 181, "MAGMORTAR", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 } },
    { 468, { 468, 225, 217, 198, "TOGEKISS", PokemonType::FAIRY, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 469, { 469, 231, 156, 200, "YANMEGA", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 470, { 470, 216, 219, 163, "LEAFEON", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 471, { 471, 238, 205, 163, "GLACEON", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 } },
    { 472, { 472, 185, 222, 181, "GLISCOR", PokemonType::GROUND, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 } },
    { 473, { 473, 247, 146, 242, "MAMOSWINE", PokemonType::ICE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 474, { 474, 264, 150, 198, "PORYGON_Z", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 } },
    { 475, { 475, 237, 195, 169, "GALLADE", PokemonType::PSYCHIC, PokemonType::FIGHTING, PokemonRarity::NORMAL, 5, 100, 0 } },
    { 476, { 476, 135, 275, 155, "PROBOPASS", PokemonType::ROCK, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 477, { 477, 180, 254, 128, "DUSKNOIR", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 } },
    { 478, { 478, 171, 150, 172, "FROSLASS", PokemonType::ICE, PokemonType::GHOST, PokemonRarity::NORMAL, 3, 0, 100 } },
    { 479, { 479, 185, 159, 137, "ROTOM_NORMAL", PokemonType::ELECTRIC, PokemonType::GHOST, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 479, { 479, 204, 219, 137, "ROTOM_FROST", PokemonType::ELECTRIC, PokemonType::ICE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 479, { 479, 204, 219, 137, "ROTOM_FAN", PokemonType::ELECTRIC, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 479, { 479, 204, 219, 137, "ROTOM_MOW", PokemonType::ELECTRIC, PokemonType::GRASS, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 479, { 479, 204, 219, 137, "ROTOM_WASH", PokemonType::ELECTRIC, PokemonType::WATER, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 479, { 479, 204, 219, 137, "ROTOM_HEAT", PokemonType::ELECTRIC, PokemonType::FIRE, PokemonRarity::NORMAL, 5, 0, 0 } },
    { 480, { 480, 156, 270, 181, "UXIE", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 481, { 481, 212, 212, 190, "MESPRIT", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 482, { 482, 270, 151, 181, "AZELF", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 483, { 483, 275, 211, 205, "DIALGA", PokemonType::STEEL, PokemonType::DRAGON, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 484, { 484, 280, 215, 189, "PALKIA", PokemonType::WATER, PokemonType::DRAGON, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 485, { 485, 251, 213, 209, "HEATRAN", PokemonType::FIRE, PokemonType::STEEL, PokemonRarity::LEGENDARY, 20, 50, 50 } },
    { 486, { 486, 287, 210, 221, "REGIGIGAS", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 487, { 487, 187, 225, 284, "GIRATINA_ALTERED", PokemonType::GHOST, PokemonType::DRAGON, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 487, { 487, 225, 187, 284, "GIRATINA_ORIGIN", PokemonType::GHOST, PokemonType::DRAGON, PokemonRarity::LEGENDARY, 20, 0, 0 } },
    { 488, { 488, 152, 258, 260, "CRESSELIA", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 100 } },
    { 489, { 489, 162, 162, 190, "PHIONE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 490, { 490, 210, 210, 225, "MANAPHY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 491, { 491, 285, 198, 172, "DARKRAI", PokemonType::DARK, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 492, { 492, 210, 210, 225, "SHAYMIN_LAND", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 492, { 492, 261, 166, 225, "SHAYMIN_SKY", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_NORMAL", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_FIGHTING", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_FLYING", PokemonType::FLYING, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_POISON", PokemonType::POISON, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_GROUND", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_ROCK", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_BUG", PokemonType::BUG, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_GHOST", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_STEEL", PokemonType::STEEL, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_FIRE", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_WATER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_GRASS", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_ELECTRIC", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_PSYCHIC", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_ICE", PokemonType::ICE, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_DRAGON", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_DARK", PokemonType::DARK, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 493, { 493, 238, 238, 237, "ARCEUS_FAIRY", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 808, { 808, 118, 99, 130, "MELTAN", PokemonType::STEEL, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
    { 809, { 809, 226, 190, 264, "MELMETAL", PokemonType::STEEL, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 } },
};

//! Case-insensitive string comparison.
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

//! case-insensitive
static inline PokemonType StringToPokemonType(const char* str)
{
    if (CompareI(str, "BUG") == 0) return PokemonType::BUG;
    if (CompareI(str, "DARK") == 0) return PokemonType::DARK;
    if (CompareI(str, "DRAGON") == 0) return PokemonType::DRAGON;
    if (CompareI(str, "ELECTRIC") == 0) return PokemonType::ELECTRIC;
    if (CompareI(str, "FAIRY") == 0) return PokemonType::FAIRY;
    if (CompareI(str, "FIGHTING") == 0) return PokemonType::FIGHTING;
    if (CompareI(str, "FIRE") == 0) return PokemonType::FIRE;
    if (CompareI(str, "FLYING") == 0) return PokemonType::FLYING;
    if (CompareI(str, "GHOST") == 0) return PokemonType::GHOST;
    if (CompareI(str, "GRASS") == 0) return PokemonType::GRASS;
    if (CompareI(str, "GROUND") == 0) return PokemonType::GROUND;
    if (CompareI(str, "ICE") == 0) return PokemonType::ICE;
    if (CompareI(str, "NORMAL") == 0) return PokemonType::NORMAL;
    if (CompareI(str, "POISON") == 0) return PokemonType::POISON;
    if (CompareI(str, "PSYCHIC") == 0) return PokemonType::PSYCHIC;
    if (CompareI(str, "ROCK") == 0) return PokemonType::ROCK;
    if (CompareI(str, "STEEL") == 0) return PokemonType::STEEL;
    if (CompareI(str, "WATER") == 0) return PokemonType::WATER;
    return PokemonType::NONE;
}

//! Returns all-uppercase name
static inline const char* PokemonTypeToString(PokemonType type)
{
    if (type == PokemonType::BUG) return "BUG";
    if (type == PokemonType::DARK) return "DARK";
    if (type == PokemonType::DRAGON) return "DRAGON";
    if (type == PokemonType::ELECTRIC) return "ELECTRIC";
    if (type == PokemonType::FAIRY) return "FAIRY";
    if (type == PokemonType::FIGHTING) return "FIGHTING";
    if (type == PokemonType::FIRE) return "FIRE";
    if (type == PokemonType::FLYING) return "FLYING";
    if (type == PokemonType::GHOST) return "GHOST";
    if (type == PokemonType::GRASS) return "GRASS";
    if (type == PokemonType::GROUND) return "GROUND";
    if (type == PokemonType::ICE) return "ICE";
    if (type == PokemonType::NORMAL) return "NORMAL";
    if (type == PokemonType::POISON) return "POISON";
    if (type == PokemonType::PSYCHIC) return "PSYCHIC";
    if (type == PokemonType::ROCK) return "ROCK";
    if (type == PokemonType::STEEL) return "STEEL";
    if (type == PokemonType::WATER) return "WATER";
    return "NONE";
}

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

const std::array<Pokemon, 6> RaidLevels{{
    { 21, 15, 15, 600 },
    { 25, 15, 15, 1800 },
    { 30, 15, 15, 3000 },
    { 40, 15, 15, 7500 },
    { 40, 15, 15, 12500 },
    { 40, 15, 15, 18750 } // level 5 but with more health (e.g. Mewtwo in regular raids)
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
    { "RATTATA", "RATTATA_NORMAL" },
    { "RATTATA", "RATTATA_ALOLA" },
    { "RATICATE", "RATICATE_NORMAL" },
    { "RATICATE", "RATICATE_ALOLA" },
    { "RAICHU", "RAICHU_NORMAL" },
    { "RAICHU", "RAICHU_ALOLA" },
    { "SANDSHREW", "SANDSHREW_NORMAL" },
    { "SANDSHREW", "SANDSHREW_ALOLA" },
    { "SANDSLASH", "SANDSLASH_NORMAL" },
    { "SANDSLASH", "SANDSLASH_ALOLA" },
    { "VULPIX", "VULPIX_NORMAL" },
    { "VULPIX", "VULPIX_ALOLA" },
    { "NINETALES", "NINETALES_NORMAL" },
    { "NINETALES", "NINETALES_ALOLA" },
    { "DIGLETT", "DIGLETT_NORMAL" },
    { "DIGLETT", "DIGLETT_ALOLA" },
    { "DUGTRIO", "DUGTRIO_NORMAL" },
    { "DUGTRIO", "DUGTRIO_ALOLA" },
    { "MEOWTH", "MEOWTH_NORMAL" },
    { "MEOWTH", "MEOWTH_ALOLA" },
    { "PERSIAN", "PERSIAN_NORMAL" },
    { "PERSIAN", "PERSIAN_ALOLA" },
    { "GEODUDE", "GEODUDE_NORMAL" },
    { "GEODUDE", "GEODUDE_ALOLA" },
    { "GRAVELER", "GRAVELER_NORMAL" },
    { "GRAVELER", "GRAVELER_ALOLA" },
    { "GOLEM", "GOLEM_NORMAL" },
    { "GOLEM", "GOLEM_ALOLA" },
    { "GRIMER", "GRIMER_NORMAL" },
    { "GRIMER", "GRIMER_ALOLA" },
    { "MUK", "MUK_NORMAL" },
    { "MUK", "MUK_ALOLA" },
    { "EXEGGUTOR", "EXEGGUTOR_NORMAL" },
    { "EXEGGUTOR", "EXEGGUTOR_ALOLA" },
    { "MAROWAK", "MAROWAK_NORMAL" },
    { "MAROWAK", "MAROWAK_ALOLA" },
    { "UNOWN", "UNOWN_F" },
    { "UNOWN", "UNOWN_A" },
    { "UNOWN", "UNOWN_B" },
    { "UNOWN", "UNOWN_C" },
    { "UNOWN", "UNOWN_D" },
    { "UNOWN", "UNOWN_E" },
    { "UNOWN", "UNOWN_G" },
    { "UNOWN", "UNOWN_H" },
    { "UNOWN", "UNOWN_I" },
    { "UNOWN", "UNOWN_J" },
    { "UNOWN", "UNOWN_K" },
    { "UNOWN", "UNOWN_L" },
    { "UNOWN", "UNOWN_M" },
    { "UNOWN", "UNOWN_N" },
    { "UNOWN", "UNOWN_O" },
    { "UNOWN", "UNOWN_P" },
    { "UNOWN", "UNOWN_Q" },
    { "UNOWN", "UNOWN_R" },
    { "UNOWN", "UNOWN_S" },
    { "UNOWN", "UNOWN_T" },
    { "UNOWN", "UNOWN_U" },
    { "UNOWN", "UNOWN_V" },
    { "UNOWN", "UNOWN_W" },
    { "UNOWN", "UNOWN_X" },
    { "UNOWN", "UNOWN_Y" },
    { "UNOWN", "UNOWN_Z" },
    { "UNOWN", "UNOWN_EXCLAMATION_POINT" },
    { "UNOWN", "UNOWN_QUESTION_MARK" },
    { "SPINDA", "SPINDA_00" },
    { "SPINDA", "SPINDA_01" },
    { "SPINDA", "SPINDA_02" },
    { "SPINDA", "SPINDA_03" },
    { "SPINDA", "SPINDA_04" },
    { "SPINDA", "SPINDA_05" },
    { "SPINDA", "SPINDA_06" },
    { "SPINDA", "SPINDA_07" },
    { "CASTFORM", "CASTFORM_NORMAL" },
    { "CASTFORM", "CASTFORM_SUNNY" },
    { "CASTFORM", "CASTFORM_RAINY" },
    { "CASTFORM", "CASTFORM_SNOWY" },
    { "DEOXYS", "DEOXYS_NORMAL" },
    { "DEOXYS", "DEOXYS_ATTACK" },
    { "DEOXYS", "DEOXYS_DEFENSE" },
    { "DEOXYS", "DEOXYS_SPEED" },
    { "BURMY", "BURMY_PLANT" },
    { "BURMY", "BURMY_SANDY" },
    { "BURMY", "BURMY_TRASH" },
    { "WORMADAM", "WORMADAM_PLANT" },
    { "WORMADAM", "WORMADAM_SANDY" },
    { "WORMADAM", "WORMADAM_TRASH" },
    { "CHERRIM", "CHERRIM_OVERCAST" },
    { "CHERRIM", "CHERRIM_SUNNY" },
    { "SHELLOS", "SHELLOS_WEST_SEA" },
    { "SHELLOS", "SHELLOS_EAST_SEA" },
    { "GASTRODON", "GASTRODON_WEST_SEA" },
    { "GASTRODON", "GASTRODON_EAST_SEA" },
    { "ROTOM", "ROTOM_NORMAL" },
    { "ROTOM", "ROTOM_FROST" },
    { "ROTOM", "ROTOM_FAN" },
    { "ROTOM", "ROTOM_MOW" },
    { "ROTOM", "ROTOM_WASH" },
    { "ROTOM", "ROTOM_HEAT" },
    { "GIRATINA", "GIRATINA_ALTERED" },
    { "GIRATINA", "GIRATINA_ORIGIN" },
    { "SHAYMIN", "SHAYMIN_LAND" },
    { "SHAYMIN", "SHAYMIN_SKY" },
    { "ARCEUS", "ARCEUS_NORMAL" },
    { "ARCEUS", "ARCEUS_FIGHTING" },
    { "ARCEUS", "ARCEUS_FLYING" },
    { "ARCEUS", "ARCEUS_POISON" },
    { "ARCEUS", "ARCEUS_GROUND" },
    { "ARCEUS", "ARCEUS_ROCK" },
    { "ARCEUS", "ARCEUS_BUG" },
    { "ARCEUS", "ARCEUS_GHOST" },
    { "ARCEUS", "ARCEUS_STEEL" },
    { "ARCEUS", "ARCEUS_FIRE" },
    { "ARCEUS", "ARCEUS_WATER" },
    { "ARCEUS", "ARCEUS_GRASS" },
    { "ARCEUS", "ARCEUS_ELECTRIC" },
    { "ARCEUS", "ARCEUS_PSYCHIC" },
    { "ARCEUS", "ARCEUS_ICE" },
    { "ARCEUS", "ARCEUS_DRAGON" },
    { "ARCEUS", "ARCEUS_DARK" },
    { "ARCEUS", "ARCEUS_FAIRY" },
};

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
static const std::string UnownExclamationName{ "Unown !" };
static const std::string UnownQuestionName{ "Unown ?" };
// N.B. "Porygon2" and "Porygon Z" inconsitency
static const std::string PorygonZName{ "Porygon Z" };
static const std::string MimeJrName{ "Mime Jr." };
// - Flabébé -> Unknown at the moment, probably FLABEBE

//! Returns Pokémon's proper name (base, e.g. Rattata, or form, e.g. Rattata Normal) in ID name format (SCREAMING_SNAKE_CASE)
//! @param name Handled case-insensitively.
//! @note "Nidoran Female", "Nidoran Male", "Mr Mime", "Ho Oh", "Mime Jr" and "PorygonZ" accepted also.
static inline std::string PokemonNameToId(Utf8::String name)
{
    if (Utf8::CompareI(name.c_str(), NidoranFemaleName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Nidoran Female") == 0)
    {
        return "NIDORAN_FEMALE";
    }
    else if (Utf8::CompareI(name.c_str(), NidoranMaleName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Nidoran Male") == 0)
    {
        return "NIDORAN_MALE";
    }
    else if (Utf8::CompareI(name.c_str(), FarfetchdName.c_str()) == 0)
    {
        return "FARFETCHD";
    }
    else if (Utf8::CompareI(name.c_str(), MrMimeName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Mr Mime") == 0)
    {
        return "MR_MIME";
    }
    else if (Utf8::CompareI(name.c_str(), UnownExclamationName.c_str()) == 0)
    {
        return "UNOWN_EXCLAMATION_POINT";
    }
    else if (Utf8::CompareI(name.c_str(), UnownQuestionName.c_str()) == 0)
    {
        return "UNOWN_QUESTION_MARK";
    }
    else if (Utf8::CompareI(name.c_str(), HoOhName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Ho Oh") == 0)
    {
        return "HO_OH";
    }
    //! @todo more generic solution for 00 -> 1 conversion?
    else if (Utf8::CompareI(name.c_str(), "Spinda 1") == 0) { return "SPINDA_00"; }
    else if (Utf8::CompareI(name.c_str(), "Spinda 2") == 0) { return "SPINDA_01"; }
    else if (Utf8::CompareI(name.c_str(), "Spinda 3") == 0) { return "SPINDA_02"; }
    else if (Utf8::CompareI(name.c_str(), "Spinda 4") == 0) { return "SPINDA_03"; }
    else if (Utf8::CompareI(name.c_str(), "Spinda 5") == 0) { return "SPINDA_04"; }
    else if (Utf8::CompareI(name.c_str(), "Spinda 6") == 0) { return "SPINDA_05"; }
    else if (Utf8::CompareI(name.c_str(), "Spinda 7") == 0) { return "SPINDA_06"; }
    else if (Utf8::CompareI(name.c_str(), "Spinda 8") == 0) { return "SPINDA_07"; }
    else if (Utf8::CompareI(name.c_str(), PorygonZName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "PorygonZ") == 0)
    {
        return "PORYGON_Z";
    }
    else if (Utf8::CompareI(name.c_str(), MimeJrName.c_str()) == 0 || Utf8::CompareI(name.c_str(), "Mime Jr") == 0)
    {
        return "MIME_JR";
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
//! - Mime Jr. -> MIME_JR
//! - Nidoran♂  & Nidoran♀ -> NIDORAN_MALE & NIDORAN_FEMALE
//! - Porygon Z -> PORYGON_Z
//! - Unown ? and Unown !
//! - Spinda 1 (SPINDA_00), et al.
//! - Flabébé -> Unknown at the moment, probably FLABEBE
//! @param id Handled case-insensitively.
static inline Utf8::String PokemonIdToName(const std::string& id)
{
    if (CompareI(id.c_str(), "NIDORAN_FEMALE") == 0) return NidoranFemaleName;
    else if (CompareI(id.c_str(), "NIDORAN_MALE") == 0) return NidoranMaleName;
    else if (CompareI(id.c_str(), "FARFETCHD") == 0) return FarfetchdName;
    else if (CompareI(id.c_str(), "MR_MIME") == 0) return MrMimeName;
    else if (CompareI(id.c_str(), "UNOWN_EXCLAMATION_POINT") == 0) return UnownExclamationName;
    else if (CompareI(id.c_str(), "UNOWN_QUESTION_MARK") == 0) return UnownQuestionName;
    else if (CompareI(id.c_str(), "HO_OH") == 0) return HoOhName;
    else if (CompareI(id.c_str(), "SPINDA_00") == 0) return "Spinda 1";
    else if (CompareI(id.c_str(), "SPINDA_01") == 0) return "Spinda 2";
    else if (CompareI(id.c_str(), "SPINDA_02") == 0) return "Spinda 3";
    else if (CompareI(id.c_str(), "SPINDA_03") == 0) return "Spinda 4";
    else if (CompareI(id.c_str(), "SPINDA_04") == 0) return "Spinda 5";
    else if (CompareI(id.c_str(), "SPINDA_05") == 0) return "Spinda 6";
    else if (CompareI(id.c_str(), "SPINDA_06") == 0) return "Spinda 7";
    else if (CompareI(id.c_str(), "SPINDA_07") == 0) return "Spinda 8";
    else if (CompareI(id.c_str(), "PORYGON_Z") == 0) return PorygonZName;
    else if (CompareI(id.c_str(), "MIME_JR") == 0) return MimeJrName;
    else return StringUtils::SnakeCaseToTitleCase(id);
}

///! If the ID is an ID of a form, the base ID/name of the form is returned.
static inline std::string FormIdToBaseId(const std::string& id)
{
    auto it = std::find_if(
        FormNames.begin(), FormNames.end(),
        [&id](const auto& kvp) { return CompareI(kvp.second.c_str(), id.c_str()) == 0; }
    );
    return it != FormNames.end() ? it->first : id;
}

}
