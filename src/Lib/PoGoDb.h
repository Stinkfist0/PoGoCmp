/** @file PoGoDb.h
    @brief C++ API for PoGoCmp. For C one can use PoGoCmp.h.

    Input file's timestamp 2018-12-26 12:17:51+0200 */

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

enum class PokemonRarity : uint8_t
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

struct PokemonSpecie
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
    //! How much tracked buddy walking is required for a candy, in kilometers.
    uint8_t buddyDistance;
    //! If both malePercent and femalePercent are 0, it means the Pokémon is genderless.
    float malePercent;
    float femalePercent;
};

//! Type-effectiveness scalar table, use PokemonType enum to access.
//! @sa AttackScalars
static const std::array<std::array<float, 18>, 18> TypeEffectiveness{{
    { 1, 1, 1, 1, 1, 0.625f, 1, 0.390625f, 0.625f, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    { 1.6f, 1, 0.625f, 0.625f, 1, 1.6f, 0.625f, 0.390625f, 1.6f, 1, 1, 1, 1, 0.625f, 1.6f, 1, 1.6f, 0.625f},
    { 1, 1.6f, 1, 1, 1, 0.625f, 1.6f, 1, 0.625f, 1, 1, 1.6f, 0.625f, 1, 1, 1, 1, 1},
    { 1, 1, 1, 0.625f, 0.625f, 0.625f, 1, 0.625f, 0.390625f, 1, 1, 1.6f, 1, 1, 1, 1, 1, 1.6f},
    { 1, 1, 0.390625f, 1.6f, 1, 1.6f, 0.625f, 1, 1.6f, 1.6f, 1, 0.625f, 1.6f, 1, 1, 1, 1, 1},
    { 1, 0.625f, 1.6f, 1, 0.625f, 1, 1.6f, 1, 0.625f, 1.6f, 1, 1, 1, 1, 1.6f, 1, 1, 1},
    { 1, 0.625f, 0.625f, 0.625f, 1, 1, 1, 0.625f, 0.625f, 0.625f, 1, 1.6f, 1, 1.6f, 1, 1, 1.6f, 0.625f},
    { 0.390625f, 1, 1, 1, 1, 1, 1, 1.6f, 1, 1, 1, 1, 1, 1.6f, 1, 1, 0.625f, 1},
    { 1, 1, 1, 1, 1, 1.6f, 1, 1, 0.625f, 0.625f, 0.625f, 1, 0.625f, 1, 1.6f, 1, 1, 1.6f},
    { 1, 1, 1, 1, 1, 0.625f, 1.6f, 1, 1.6f, 0.625f, 0.625f, 1.6f, 1, 1, 1.6f, 0.625f, 1, 1},
    { 1, 1, 1, 1, 1.6f, 1.6f, 1, 1, 1, 1.6f, 0.625f, 0.625f, 1, 1, 1, 0.625f, 1, 1},
    { 1, 1, 0.625f, 0.625f, 1.6f, 1.6f, 0.625f, 1, 0.625f, 0.625f, 1.6f, 0.625f, 1, 1, 1, 0.625f, 1, 1},
    { 1, 1, 1.6f, 1, 0.390625f, 1, 1, 1, 1, 1, 1.6f, 0.625f, 0.625f, 1, 1, 0.625f, 1, 1},
    { 1, 1.6f, 1, 1.6f, 1, 1, 1, 1, 0.625f, 1, 1, 1, 1, 0.625f, 1, 1, 0.390625f, 1},
    { 1, 1, 1.6f, 1, 1.6f, 1, 1, 1, 0.625f, 0.625f, 0.625f, 1.6f, 1, 1, 0.625f, 1.6f, 1, 1},
    { 1, 1, 1, 1, 1, 1, 1, 1, 0.625f, 1, 1, 1, 1, 1, 1, 1.6f, 1, 0.390625f},
    { 1, 0.625f, 1, 1, 1, 1, 1, 1.6f, 1, 1, 1, 1, 1, 1.6f, 1, 1, 0.625f, 0.625f},
    { 1, 1.6f, 1, 0.625f, 1, 1, 1, 1, 0.625f, 0.625f, 1, 1, 1, 1, 1, 1.6f, 1.6f, 1}
}};

//! Use PokemonIndex() for the index in PoGoCmp::PokemonByNumber.
static const std::array<PokemonSpecie, 495> PokemonByNumber{{
    { 1, 118, 111, 128, "BULBASAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 2, 151, 143, 155, "IVYSAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 3, 198, 189, 190, "VENUSAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 4, 116, 93, 118, "CHARMANDER", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 5, 158, 126, 151, "CHARMELEON", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 6, 223, 173, 186, "CHARIZARD", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 7, 94, 121, 127, "SQUIRTLE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 8, 126, 155, 153, "WARTORTLE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 9, 171, 207, 188, "BLASTOISE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 10, 55, 55, 128, "CATERPIE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 11, 45, 80, 137, "METAPOD", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 12, 167, 137, 155, "BUTTERFREE", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 13, 63, 50, 120, "WEEDLE", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 14, 46, 75, 128, "KAKUNA", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 15, 169, 130, 163, "BEEDRILL", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 16, 85, 73, 120, "PIDGEY", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 17, 117, 105, 160, "PIDGEOTTO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 18, 166, 154, 195, "PIDGEOT", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 19, 103, 70, 102, "RATTATA", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 20, 161, 139, 146, "RATICATE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 21, 112, 60, 120, "SPEAROW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 22, 182, 133, 163, "FEAROW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 23, 110, 97, 111, "EKANS", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 24, 167, 153, 155, "ARBOK", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 25, 112, 96, 111, "PIKACHU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 26, 193, 151, 155, "RAICHU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 27, 126, 120, 137, "SANDSHREW", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 28, 182, 175, 181, "SANDSLASH", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 29, 86, 89, 146, "NIDORAN_FEMALE", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 },
    { 30, 117, 120, 172, "NIDORINA", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 },
    { 31, 180, 173, 207, "NIDOQUEEN", PokemonType::POISON, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 0, 100 },
    { 32, 105, 76, 130, "NIDORAN_MALE", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 },
    { 33, 137, 111, 156, "NIDORINO", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 },
    { 34, 204, 156, 191, "NIDOKING", PokemonType::POISON, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 100, 0 },
    { 35, 107, 108, 172, "CLEFAIRY", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 25, 75 },
    { 36, 178, 162, 216, "CLEFABLE", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 25, 75 },
    { 37, 96, 109, 116, "VULPIX", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 38, 169, 190, 177, "NINETALES", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 39, 80, 41, 251, "JIGGLYPUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 25, 75 },
    { 40, 156, 90, 295, "WIGGLYTUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 25, 75 },
    { 41, 83, 73, 120, "ZUBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 42, 161, 150, 181, "GOLBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 43, 131, 112, 128, "ODDISH", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 44, 153, 136, 155, "GLOOM", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 45, 202, 167, 181, "VILEPLUME", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 46, 121, 99, 111, "PARAS", PokemonType::BUG, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 47, 165, 146, 155, "PARASECT", PokemonType::BUG, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 48, 100, 100, 155, "VENONAT", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 49, 179, 143, 172, "VENOMOTH", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 50, 109, 78, 67, "DIGLETT", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 51, 167, 134, 111, "DUGTRIO", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 52, 92, 78, 120, "MEOWTH", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 53, 150, 136, 163, "PERSIAN", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 54, 122, 95, 137, "PSYDUCK", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 55, 191, 162, 190, "GOLDUCK", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 56, 148, 82, 120, "MANKEY", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 57, 207, 138, 163, "PRIMEAPE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 58, 136, 93, 146, "GROWLITHE", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 59, 227, 166, 207, "ARCANINE", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 60, 101, 82, 120, "POLIWAG", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 61, 130, 123, 163, "POLIWHIRL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 62, 182, 184, 207, "POLIWRATH", PokemonType::WATER, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 63, 195, 82, 93, "ABRA", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 64, 232, 117, 120, "KADABRA", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 65, 271, 167, 146, "ALAKAZAM", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 66, 137, 82, 172, "MACHOP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 67, 177, 125, 190, "MACHOKE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 68, 234, 159, 207, "MACHAMP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 69, 139, 61, 137, "BELLSPROUT", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 70, 172, 92, 163, "WEEPINBELL", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 71, 207, 135, 190, "VICTREEBEL", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 72, 97, 149, 120, "TENTACOOL", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 73, 166, 209, 190, "TENTACRUEL", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 74, 132, 132, 120, "GEODUDE", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 75, 164, 164, 146, "GRAVELER", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 76, 211, 198, 190, "GOLEM", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 77, 170, 127, 137, "PONYTA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 78, 207, 162, 163, "RAPIDASH", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 79, 109, 98, 207, "SLOWPOKE", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 80, 177, 180, 216, "SLOWBRO", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 81, 165, 121, 93, "MAGNEMITE", PokemonType::ELECTRIC, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 0, 0 },
    { 82, 223, 169, 137, "MAGNETON", PokemonType::ELECTRIC, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 0, 0 },
    { 83, 124, 115, 141, "FARFETCHD", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 84, 158, 83, 111, "DODUO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 85, 218, 140, 155, "DODRIO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 86, 85, 121, 163, "SEEL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 87, 139, 177, 207, "DEWGONG", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 88, 135, 90, 190, "GRIMER", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 89, 190, 172, 233, "MUK", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 90, 116, 134, 102, "SHELLDER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 91, 186, 256, 137, "CLOYSTER", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 92, 186, 67, 102, "GASTLY", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 93, 223, 107, 128, "HAUNTER", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 94, 261, 149, 155, "GENGAR", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 95, 85, 232, 111, "ONIX", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 96, 89, 136, 155, "DROWZEE", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 97, 144, 193, 198, "HYPNO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 98, 181, 124, 102, "KRABBY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 99, 240, 181, 146, "KINGLER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 100, 109, 111, 120, "VOLTORB", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 101, 173, 173, 155, "ELECTRODE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 102, 107, 125, 155, "EXEGGCUTE", PokemonType::GRASS, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 103, 233, 149, 216, "EXEGGUTOR", PokemonType::GRASS, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 104, 90, 144, 137, "CUBONE", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 105, 144, 186, 155, "MAROWAK", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 106, 224, 181, 137, "HITMONLEE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 },
    { 107, 193, 197, 137, "HITMONCHAN", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 },
    { 108, 108, 137, 207, "LICKITUNG", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 109, 119, 141, 120, "KOFFING", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 110, 174, 197, 163, "WEEZING", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 111, 140, 127, 190, "RHYHORN", PokemonType::GROUND, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 112, 222, 171, 233, "RHYDON", PokemonType::GROUND, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 113, 60, 128, 487, "CHANSEY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 },
    { 114, 183, 169, 163, "TANGELA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 115, 181, 165, 233, "KANGASKHAN", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 },
    { 116, 129, 103, 102, "HORSEA", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 117, 187, 156, 146, "SEADRA", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 118, 123, 110, 128, "GOLDEEN", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 119, 175, 147, 190, "SEAKING", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 120, 137, 112, 102, "STARYU", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 121, 210, 184, 155, "STARMIE", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 122, 192, 205, 120, "MR_MIME", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 123, 218, 170, 172, "SCYTHER", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 124, 223, 151, 163, "JYNX", PokemonType::ICE, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 100 },
    { 125, 198, 158, 163, "ELECTABUZZ", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 75, 25 },
    { 126, 206, 154, 163, "MAGMAR", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 75, 25 },
    { 127, 238, 182, 163, "PINSIR", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 128, 198, 183, 181, "TAUROS", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 },
    { 129, 29, 85, 85, "MAGIKARP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 130, 237, 186, 216, "GYARADOS", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 131, 165, 174, 277, "LAPRAS", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 132, 91, 91, 134, "DITTO", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 133, 104, 114, 146, "EEVEE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 134, 205, 161, 277, "VAPOREON", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 135, 232, 182, 163, "JOLTEON", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 136, 246, 179, 163, "FLAREON", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 137, 153, 136, 163, "PORYGON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 138, 155, 153, 111, "OMANYTE", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 139, 207, 201, 172, "OMASTAR", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 140, 148, 140, 102, "KABUTO", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 141, 220, 186, 155, "KABUTOPS", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 142, 221, 159, 190, "AERODACTYL", PokemonType::ROCK, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 143, 190, 169, 330, "SNORLAX", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 144, 192, 236, 207, "ARTICUNO", PokemonType::ICE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 145, 253, 185, 207, "ZAPDOS", PokemonType::ELECTRIC, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 146, 251, 181, 207, "MOLTRES", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 147, 119, 91, 121, "DRATINI", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 148, 163, 135, 156, "DRAGONAIR", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 149, 263, 198, 209, "DRAGONITE", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 150, 300, 182, 214, "MEWTWO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 151, 210, 210, 225, "MEW", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 152, 92, 122, 128, "CHIKORITA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 153, 122, 155, 155, "BAYLEEF", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 154, 168, 202, 190, "MEGANIUM", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 155, 116, 93, 118, "CYNDAQUIL", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 156, 158, 126, 151, "QUILAVA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 157, 223, 173, 186, "TYPHLOSION", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 158, 117, 109, 137, "TOTODILE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 159, 150, 142, 163, "CROCONAW", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 160, 205, 188, 198, "FERALIGATR", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 161, 79, 73, 111, "SENTRET", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 162, 148, 125, 198, "FURRET", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 163, 67, 88, 155, "HOOTHOOT", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 164, 145, 156, 225, "NOCTOWL", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 165, 72, 118, 120, "LEDYBA", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 166, 107, 179, 146, "LEDIAN", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 167, 105, 73, 120, "SPINARAK", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 168, 161, 124, 172, "ARIADOS", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 169, 194, 178, 198, "CROBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 170, 106, 97, 181, "CHINCHOU", PokemonType::WATER, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 171, 146, 137, 268, "LANTURN", PokemonType::WATER, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 172, 77, 53, 85, "PICHU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 173, 75, 79, 137, "CLEFFA", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 25, 75 },
    { 174, 69, 32, 207, "IGGLYBUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 25, 75 },
    { 175, 67, 116, 111, "TOGEPI", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 176, 139, 181, 146, "TOGETIC", PokemonType::FAIRY, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 177, 134, 89, 120, "NATU", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 178, 192, 146, 163, "XATU", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 179, 114, 79, 146, "MAREEP", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 180, 145, 109, 172, "FLAAFFY", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 181, 211, 169, 207, "AMPHAROS", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 182, 169, 186, 181, "BELLOSSOM", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 183, 37, 93, 172, "MARILL", PokemonType::WATER, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 50, 50 },
    { 184, 112, 152, 225, "AZUMARILL", PokemonType::WATER, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 50, 50 },
    { 185, 167, 176, 172, "SUDOWOODO", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 186, 174, 179, 207, "POLITOED", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 187, 67, 94, 111, "HOPPIP", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 188, 91, 120, 146, "SKIPLOOM", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 189, 118, 183, 181, "JUMPLUFF", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 190, 136, 112, 146, "AIPOM", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 191, 55, 55, 102, "SUNKERN", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 192, 185, 135, 181, "SUNFLORA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 193, 154, 94, 163, "YANMA", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 194, 75, 66, 146, "WOOPER", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 195, 152, 143, 216, "QUAGSIRE", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 196, 261, 175, 163, "ESPEON", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 197, 126, 240, 216, "UMBREON", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 198, 175, 87, 155, "MURKROW", PokemonType::DARK, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 199, 177, 180, 216, "SLOWKING", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 200, 167, 154, 155, "MISDREAVUS", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 201, 136, 91, 134, "UNOWN", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 },
    { 202, 60, 106, 382, "WOBBUFFET", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 203, 182, 133, 172, "GIRAFARIG", PokemonType::NORMAL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 204, 108, 122, 137, "PINECO", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 205, 161, 205, 181, "FORRETRESS", PokemonType::BUG, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 50, 50 },
    { 206, 131, 128, 225, "DUNSPARCE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 207, 143, 184, 163, "GLIGAR", PokemonType::GROUND, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 208, 148, 272, 181, "STEELIX", PokemonType::STEEL, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 209, 137, 85, 155, "SNUBBULL", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 210, 212, 131, 207, "GRANBULL", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 211, 184, 138, 163, "QWILFISH", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 212, 236, 181, 172, "SCIZOR", PokemonType::BUG, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 50, 50 },
    { 213, 17, 396, 85, "SHUCKLE", PokemonType::BUG, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 214, 234, 179, 190, "HERACROSS", PokemonType::BUG, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 215, 189, 146, 146, "SNEASEL", PokemonType::DARK, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 216, 142, 93, 155, "TEDDIURSA", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 217, 236, 144, 207, "URSARING", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 218, 118, 71, 120, "SLUGMA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 219, 139, 191, 137, "MAGCARGO", PokemonType::FIRE, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 },
    { 220, 90, 69, 137, "SWINUB", PokemonType::ICE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 221, 181, 138, 225, "PILOSWINE", PokemonType::ICE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 222, 118, 156, 146, "CORSOLA", PokemonType::WATER, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 25, 75 },
    { 223, 127, 69, 111, "REMORAID", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 224, 197, 141, 181, "OCTILLERY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 225, 128, 90, 128, "DELIBIRD", PokemonType::ICE, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 226, 148, 226, 163, "MANTINE", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 227, 148, 226, 163, "SKARMORY", PokemonType::STEEL, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 228, 152, 83, 128, "HOUNDOUR", PokemonType::DARK, PokemonType::FIRE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 229, 224, 144, 181, "HOUNDOOM", PokemonType::DARK, PokemonType::FIRE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 230, 194, 194, 181, "KINGDRA", PokemonType::WATER, PokemonType::DRAGON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 231, 107, 98, 207, "PHANPY", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 232, 214, 185, 207, "DONPHAN", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 233, 198, 180, 198, "PORYGON2", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 234, 192, 131, 177, "STANTLER", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 235, 40, 83, 146, "SMEARGLE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 236, 64, 64, 111, "TYROGUE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 },
    { 237, 173, 207, 137, "HITMONTOP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 },
    { 238, 153, 91, 128, "SMOOCHUM", PokemonType::ICE, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 100 },
    { 239, 135, 101, 128, "ELEKID", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 75, 25 },
    { 240, 151, 99, 128, "MAGBY", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 75, 25 },
    { 241, 157, 193, 216, "MILTANK", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 },
    { 242, 129, 169, 496, "BLISSEY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 },
    { 243, 241, 195, 207, "RAIKOU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 244, 235, 171, 251, "ENTEI", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 245, 180, 235, 225, "SUICUNE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 246, 115, 93, 137, "LARVITAR", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 247, 155, 133, 172, "PUPITAR", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 248, 251, 207, 225, "TYRANITAR", PokemonType::ROCK, PokemonType::DARK, PokemonRarity::NORMAL, 5, 50, 50 },
    { 249, 193, 310, 235, "LUGIA", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 250, 239, 244, 214, "HO_OH", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 251, 210, 210, 225, "CELEBI", PokemonType::PSYCHIC, PokemonType::GRASS, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 252, 124, 94, 120, "TREECKO", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 253, 172, 120, 137, "GROVYLE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 254, 223, 169, 172, "SCEPTILE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 255, 130, 87, 128, "TORCHIC", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 256, 163, 115, 155, "COMBUSKEN", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 257, 240, 141, 190, "BLAZIKEN", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 258, 126, 93, 137, "MUDKIP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 259, 156, 133, 172, "MARSHTOMP", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 260, 208, 175, 225, "SWAMPERT", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 261, 96, 61, 111, "POOCHYENA", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 262, 171, 132, 172, "MIGHTYENA", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 263, 58, 80, 116, "ZIGZAGOON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 264, 142, 128, 186, "LINOONE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 265, 75, 59, 128, "WURMPLE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 266, 60, 77, 137, "SILCOON", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 267, 189, 98, 155, "BEAUTIFLY", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 268, 60, 77, 137, "CASCOON", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 269, 98, 162, 155, "DUSTOX", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 270, 71, 77, 120, "LOTAD", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 271, 112, 119, 155, "LOMBRE", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 272, 173, 176, 190, "LUDICOLO", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 273, 71, 77, 120, "SEEDOT", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 274, 134, 78, 172, "NUZLEAF", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 275, 200, 121, 207, "SHIFTRY", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 276, 106, 61, 120, "TAILLOW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 277, 185, 124, 155, "SWELLOW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 278, 106, 61, 120, "WINGULL", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 279, 175, 174, 155, "PELIPPER", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 280, 79, 59, 99, "RALTS", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 281, 117, 90, 116, "KIRLIA", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 282, 237, 195, 169, "GARDEVOIR", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 283, 93, 87, 120, "SURSKIT", PokemonType::BUG, PokemonType::WATER, PokemonRarity::NORMAL, 1, 50, 50 },
    { 284, 192, 150, 172, "MASQUERAIN", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 285, 74, 110, 155, "SHROOMISH", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 286, 241, 144, 155, "BRELOOM", PokemonType::GRASS, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 287, 104, 92, 155, "SLAKOTH", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 288, 159, 145, 190, "VIGOROTH", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 289, 290, 166, 284, "SLAKING", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 290, 80, 126, 104, "NINCADA", PokemonType::BUG, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 291, 199, 112, 156, "NINJASK", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 292, 153, 73, 1, "SHEDINJA", PokemonType::BUG, PokemonType::GHOST, PokemonRarity::NORMAL, 5, 0, 0 },
    { 293, 92, 42, 162, "WHISMUR", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 294, 134, 81, 197, "LOUDRED", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 295, 179, 137, 232, "EXPLOUD", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 296, 99, 54, 176, "MAKUHITA", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 297, 209, 114, 302, "HARIYAMA", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 298, 36, 71, 137, "AZURILL", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 25, 75 },
    { 299, 82, 215, 102, "NOSEPASS", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 300, 84, 79, 137, "SKITTY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 301, 132, 127, 172, "DELCATTY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 302, 141, 136, 137, "SABLEYE", PokemonType::DARK, PokemonType::GHOST, PokemonRarity::NORMAL, 5, 50, 50 },
    { 303, 155, 141, 137, "MAWILE", PokemonType::STEEL, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 304, 121, 141, 137, "ARON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 },
    { 305, 158, 198, 155, "LAIRON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 },
    { 306, 198, 257, 172, "AGGRON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 },
    { 307, 78, 107, 102, "MEDITITE", PokemonType::FIGHTING, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 308, 121, 152, 155, "MEDICHAM", PokemonType::FIGHTING, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 309, 123, 78, 120, "ELECTRIKE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 310, 215, 127, 172, "MANECTRIC", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 311, 167, 129, 155, "PLUSLE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 312, 147, 150, 155, "MINUN", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 313, 143, 166, 163, "VOLBEAT", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 },
    { 314, 143, 166, 163, "ILLUMISE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 },
    { 315, 186, 131, 137, "ROSELIA", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 316, 80, 99, 172, "GULPIN", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 317, 140, 159, 225, "SWALOT", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 318, 171, 39, 128, "CARVANHA", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 319, 243, 83, 172, "SHARPEDO", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 320, 136, 68, 277, "WAILMER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 321, 175, 87, 347, "WAILORD", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 322, 119, 79, 155, "NUMEL", PokemonType::FIRE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 323, 194, 136, 172, "CAMERUPT", PokemonType::FIRE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 324, 151, 203, 172, "TORKOAL", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 325, 125, 122, 155, "SPOINK", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 326, 171, 188, 190, "GRUMPIG", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 327, 116, 116, 155, "SPINDA", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 328, 162, 78, 128, "TRAPINCH", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 329, 134, 99, 137, "VIBRAVA", PokemonType::GROUND, PokemonType::DRAGON, PokemonRarity::NORMAL, 5, 50, 50 },
    { 330, 205, 168, 190, "FLYGON", PokemonType::GROUND, PokemonType::DRAGON, PokemonRarity::NORMAL, 5, 50, 50 },
    { 331, 156, 74, 137, "CACNEA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 332, 221, 115, 172, "CACTURNE", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 333, 76, 132, 128, "SWABLU", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 334, 141, 201, 181, "ALTARIA", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 335, 222, 124, 177, "ZANGOOSE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 336, 196, 118, 177, "SEVIPER", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 337, 178, 153, 207, "LUNATONE", PokemonType::ROCK, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 338, 178, 153, 207, "SOLROCK", PokemonType::ROCK, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 339, 93, 82, 137, "BARBOACH", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 340, 151, 141, 242, "WHISCASH", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 341, 141, 99, 125, "CORPHISH", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 342, 224, 142, 160, "CRAWDAUNT", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 343, 77, 124, 120, "BALTOY", PokemonType::GROUND, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 344, 140, 229, 155, "CLAYDOL", PokemonType::GROUND, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 345, 105, 150, 165, "LILEEP", PokemonType::ROCK, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 346, 152, 194, 200, "CRADILY", PokemonType::ROCK, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 347, 176, 100, 128, "ANORITH", PokemonType::ROCK, PokemonType::BUG, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 348, 222, 174, 181, "ARMALDO", PokemonType::ROCK, PokemonType::BUG, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 349, 29, 85, 85, "FEEBAS", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 350, 192, 219, 216, "MILOTIC", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 351, 139, 139, 172, "CASTFORM", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 352, 161, 189, 155, "KECLEON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 353, 138, 65, 127, "SHUPPET", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 354, 218, 126, 162, "BANETTE", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 355, 70, 162, 85, "DUSKULL", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 356, 124, 234, 120, "DUSCLOPS", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 357, 136, 163, 223, "TROPIUS", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 358, 175, 170, 181, "CHIMECHO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 359, 246, 120, 163, "ABSOL", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 360, 41, 86, 216, "WYNAUT", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 361, 95, 95, 137, "SNORUNT", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 362, 162, 162, 190, "GLALIE", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 363, 95, 90, 172, "SPHEAL", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 },
    { 364, 137, 132, 207, "SEALEO", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 },
    { 365, 182, 176, 242, "WALREIN", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 },
    { 366, 133, 135, 111, "CLAMPERL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 367, 197, 179, 146, "HUNTAIL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 368, 211, 179, 146, "GOREBYSS", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 369, 162, 203, 225, "RELICANTH", PokemonType::WATER, PokemonType::ROCK, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 370, 81, 128, 125, "LUVDISC", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 371, 134, 93, 128, "BAGON", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 372, 172, 155, 163, "SHELGON", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 373, 277, 168, 216, "SALAMENCE", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 374, 96, 132, 120, "BELDUM", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 },
    { 375, 138, 176, 155, "METANG", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 },
    { 376, 257, 228, 190, "METAGROSS", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 },
    { 377, 179, 309, 190, "REGIROCK", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 378, 179, 309, 190, "REGICE", PokemonType::ICE, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 379, 143, 285, 190, "REGISTEEL", PokemonType::STEEL, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 380, 228, 246, 190, "LATIAS", PokemonType::DRAGON, PokemonType::PSYCHIC, PokemonRarity::LEGENDARY, 20, 0, 100 },
    { 381, 268, 212, 190, "LATIOS", PokemonType::DRAGON, PokemonType::PSYCHIC, PokemonRarity::LEGENDARY, 20, 100, 0 },
    { 382, 270, 228, 205, "KYOGRE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 383, 270, 228, 205, "GROUDON", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 384, 284, 170, 213, "RAYQUAZA", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 385, 210, 210, 225, "JIRACHI", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 386, 345, 115, 137, "DEOXYS", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 387, 119, 110, 146, "TURTWIG", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 388, 157, 143, 181, "GROTLE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 389, 202, 188, 216, "TORTERRA", PokemonType::GRASS, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 390, 113, 86, 127, "CHIMCHAR", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 391, 158, 105, 162, "MONFERNO", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 392, 222, 151, 183, "INFERNAPE", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 393, 112, 102, 142, "PIPLUP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 394, 150, 139, 162, "PRINPLUP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 395, 210, 186, 197, "EMPOLEON", PokemonType::WATER, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 396, 101, 58, 120, "STARLY", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 397, 142, 94, 146, "STARAVIA", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 398, 234, 140, 198, "STARAPTOR", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 399, 80, 73, 153, "BIDOOF", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 400, 162, 119, 188, "BIBAREL", PokemonType::NORMAL, PokemonType::WATER, PokemonRarity::NORMAL, 1, 50, 50 },
    { 401, 45, 74, 114, "KRICKETOT", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 402, 160, 100, 184, "KRICKETUNE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 403, 117, 64, 128, "SHINX", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 404, 159, 95, 155, "LUXIO", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 405, 232, 156, 190, "LUXRAY", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 406, 91, 109, 120, "BUDEW", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 407, 243, 185, 155, "ROSERADE", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 408, 218, 71, 167, "CRANIDOS", PokemonType::ROCK, PokemonType::ROCK, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 409, 295, 109, 219, "RAMPARDOS", PokemonType::ROCK, PokemonType::ROCK, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 410, 76, 195, 102, "SHIELDON", PokemonType::ROCK, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 411, 94, 286, 155, "BASTIODON", PokemonType::ROCK, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 412, 53, 83, 120, "BURMY", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 413, 141, 180, 155, "WORMADAM", PokemonType::BUG, PokemonType::GRASS, PokemonRarity::NORMAL, 1, 0, 100 },
    { 414, 185, 98, 172, "MOTHIM", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 100, 0 },
    { 415, 59, 83, 102, "COMBEE", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 416, 149, 190, 172, "VESPIQUEN", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 0, 100 },
    { 417, 94, 172, 155, "PACHIRISU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 418, 132, 67, 146, "BUIZEL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 419, 221, 114, 198, "FLOATZEL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 420, 108, 92, 128, "CHERUBI", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 421, 170, 153, 172, "CHERRIM", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 422, 103, 105, 183, "SHELLOS", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 423, 169, 143, 244, "GASTRODON", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 424, 205, 143, 181, "AMBIPOM", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 425, 117, 80, 207, "DRIFLOON", PokemonType::GHOST, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 426, 180, 102, 312, "DRIFBLIM", PokemonType::GHOST, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 427, 130, 105, 146, "BUNEARY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 428, 156, 194, 163, "LOPUNNY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 429, 211, 187, 155, "MISMAGIUS", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 430, 243, 103, 225, "HONCHKROW", PokemonType::DARK, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 431, 109, 82, 135, "GLAMEOW", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 432, 172, 133, 174, "PURUGLY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 433, 114, 94, 128, "CHINGLING", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 434, 121, 90, 160, "STUNKY", PokemonType::POISON, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 435, 184, 132, 230, "SKUNTANK", PokemonType::POISON, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 436, 43, 154, 149, "BRONZOR", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 437, 161, 213, 167, "BRONZONG", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 438, 124, 133, 137, "BONSLY", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 439, 125, 142, 85, "MIME_JR", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 440, 25, 77, 225, "HAPPINY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 },
    { 441, 183, 91, 183, "CHATOT", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 442, 169, 199, 137, "SPIRITOMB", PokemonType::GHOST, PokemonType::DARK, PokemonRarity::NORMAL, 5, 50, 50 },
    { 443, 124, 84, 151, "GIBLE", PokemonType::DRAGON, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 444, 172, 125, 169, "GABITE", PokemonType::DRAGON, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 445, 261, 193, 239, "GARCHOMP", PokemonType::DRAGON, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 446, 137, 117, 286, "MUNCHLAX", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 447, 127, 78, 120, "RIOLU", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 448, 236, 144, 172, "LUCARIO", PokemonType::FIGHTING, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 87.5, 12.5 },
    { 449, 124, 118, 169, "HIPPOPOTAS", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 450, 201, 191, 239, "HIPPOWDON", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 451, 93, 151, 120, "SKORUPI", PokemonType::POISON, PokemonType::BUG, PokemonRarity::NORMAL, 5, 50, 50 },
    { 452, 180, 202, 172, "DRAPION", PokemonType::POISON, PokemonType::DARK, PokemonRarity::NORMAL, 5, 50, 50 },
    { 453, 116, 76, 134, "CROAGUNK", PokemonType::POISON, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 454, 211, 133, 195, "TOXICROAK", PokemonType::POISON, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 455, 187, 136, 179, "CARNIVINE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 456, 96, 116, 135, "FINNEON", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 457, 142, 170, 170, "LUMINEON", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 458, 105, 179, 128, "MANTYKE", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 459, 115, 105, 155, "SNOVER", PokemonType::GRASS, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 460, 178, 158, 207, "ABOMASNOW", PokemonType::GRASS, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 461, 243, 171, 172, "WEAVILE", PokemonType::DARK, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 462, 238, 205, 172, "MAGNEZONE", PokemonType::ELECTRIC, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 0, 0 },
    { 463, 161, 181, 242, "LICKILICKY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 464, 241, 190, 251, "RHYPERIOR", PokemonType::GROUND, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 465, 207, 184, 225, "TANGROWTH", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 466, 249, 163, 181, "ELECTIVIRE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 467, 247, 172, 181, "MAGMORTAR", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 468, 225, 217, 198, "TOGEKISS", PokemonType::FAIRY, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 469, 231, 156, 200, "YANMEGA", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 470, 216, 219, 163, "LEAFEON", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 471, 238, 205, 163, "GLACEON", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 87.5, 12.5 },
    { 472, 185, 222, 181, "GLISCOR", PokemonType::GROUND, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 473, 247, 146, 242, "MAMOSWINE", PokemonType::ICE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 474, 264, 150, 198, "PORYGON_Z", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 475, 237, 195, 169, "GALLADE", PokemonType::PSYCHIC, PokemonType::FIGHTING, PokemonRarity::NORMAL, 5, 100, 0 },
    { 476, 135, 275, 155, "PROBOPASS", PokemonType::ROCK, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 50, 50 },
    { 477, 180, 254, 128, "DUSKNOIR", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 478, 171, 150, 172, "FROSLASS", PokemonType::ICE, PokemonType::GHOST, PokemonRarity::NORMAL, 3, 0, 100 },
    { 479, 185, 159, 137, "ROTOM", PokemonType::ELECTRIC, PokemonType::GHOST, PokemonRarity::NORMAL, 5, 0, 0 },
    { 480, 156, 270, 181, "UXIE", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 481, 212, 212, 190, "MESPRIT", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 482, 270, 151, 181, "AZELF", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 483, 275, 211, 205, "DIALGA", PokemonType::STEEL, PokemonType::DRAGON, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 484, 280, 215, 189, "PALKIA", PokemonType::WATER, PokemonType::DRAGON, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 485, 251, 213, 209, "HEATRAN", PokemonType::FIRE, PokemonType::STEEL, PokemonRarity::LEGENDARY, 20, 50, 50 },
    { 486, 287, 210, 221, "REGIGIGAS", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 487, 187, 225, 284, "GIRATINA", PokemonType::GHOST, PokemonType::DRAGON, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 488, 152, 258, 260, "CRESSELIA", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 100 },
    { 489, 162, 162, 190, "PHIONE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 490, 210, 210, 225, "MANAPHY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 491, 285, 198, 172, "DARKRAI", PokemonType::DARK, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 492, 210, 210, 225, "SHAYMIN", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 493, 238, 238, 237, "ARCEUS", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 808, 118, 99, 130, "MELTAN", PokemonType::STEEL, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 809, 226, 190, 264, "MELMETAL", PokemonType::STEEL, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
}};

//! Case-insensitive string comparison.
static inline int CompareI(const char* str1, const char* str2)
{
#ifdef _WIN32
    return _stricmp(str1, str2);
#else
    return strcasecmp(str1, str2);
#endif
}

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

//! Pokedex number range
using PokedexRange = std::pair<size_t, size_t>;
const PokedexRange Gen1Range{ 1, 151 };
const PokedexRange Gen2Range{ 152, 251 };
const PokedexRange Gen3Range{ 252, 386 };
const PokedexRange Gen4Range{ 387, 493 };
const PokedexRange LatestGenRange{ Gen4Range };
//! Meltan and Melmetal
const PokedexRange UnknownRange{ 808, 809};
//! Range for known generations, unknown Pokémon appended to the end
const PokedexRange MaxRange{ 1, PoGoCmp::PokemonByNumber.size() };
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

struct StringLessThanI
{
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return CompareI(lhs.c_str(), rhs.c_str()) < 0;
    }
};

static inline size_t PokemonIndex(size_t number)
{
    if (number == 808) return LatestGenRange.second;
    else if (number == 809) return LatestGenRange.second + 1;
    else return number - 1;
}

//! Case-insensitive.
//! @sa PokemonNameToId, PokemonIdToName
static const std::map<std::string, const PokemonSpecie*, StringLessThanI> PokemonByIdName {
    { "BULBASAUR", &PokemonByNumber[0] },
    { "IVYSAUR", &PokemonByNumber[1] },
    { "VENUSAUR", &PokemonByNumber[2] },
    { "CHARMANDER", &PokemonByNumber[3] },
    { "CHARMELEON", &PokemonByNumber[4] },
    { "CHARIZARD", &PokemonByNumber[5] },
    { "SQUIRTLE", &PokemonByNumber[6] },
    { "WARTORTLE", &PokemonByNumber[7] },
    { "BLASTOISE", &PokemonByNumber[8] },
    { "CATERPIE", &PokemonByNumber[9] },
    { "METAPOD", &PokemonByNumber[10] },
    { "BUTTERFREE", &PokemonByNumber[11] },
    { "WEEDLE", &PokemonByNumber[12] },
    { "KAKUNA", &PokemonByNumber[13] },
    { "BEEDRILL", &PokemonByNumber[14] },
    { "PIDGEY", &PokemonByNumber[15] },
    { "PIDGEOTTO", &PokemonByNumber[16] },
    { "PIDGEOT", &PokemonByNumber[17] },
    { "RATTATA", &PokemonByNumber[18] },
    { "RATICATE", &PokemonByNumber[19] },
    { "SPEAROW", &PokemonByNumber[20] },
    { "FEAROW", &PokemonByNumber[21] },
    { "EKANS", &PokemonByNumber[22] },
    { "ARBOK", &PokemonByNumber[23] },
    { "PIKACHU", &PokemonByNumber[24] },
    { "RAICHU", &PokemonByNumber[25] },
    { "SANDSHREW", &PokemonByNumber[26] },
    { "SANDSLASH", &PokemonByNumber[27] },
    { "NIDORAN_FEMALE", &PokemonByNumber[28] },
    { "NIDORINA", &PokemonByNumber[29] },
    { "NIDOQUEEN", &PokemonByNumber[30] },
    { "NIDORAN_MALE", &PokemonByNumber[31] },
    { "NIDORINO", &PokemonByNumber[32] },
    { "NIDOKING", &PokemonByNumber[33] },
    { "CLEFAIRY", &PokemonByNumber[34] },
    { "CLEFABLE", &PokemonByNumber[35] },
    { "VULPIX", &PokemonByNumber[36] },
    { "NINETALES", &PokemonByNumber[37] },
    { "JIGGLYPUFF", &PokemonByNumber[38] },
    { "WIGGLYTUFF", &PokemonByNumber[39] },
    { "ZUBAT", &PokemonByNumber[40] },
    { "GOLBAT", &PokemonByNumber[41] },
    { "ODDISH", &PokemonByNumber[42] },
    { "GLOOM", &PokemonByNumber[43] },
    { "VILEPLUME", &PokemonByNumber[44] },
    { "PARAS", &PokemonByNumber[45] },
    { "PARASECT", &PokemonByNumber[46] },
    { "VENONAT", &PokemonByNumber[47] },
    { "VENOMOTH", &PokemonByNumber[48] },
    { "DIGLETT", &PokemonByNumber[49] },
    { "DUGTRIO", &PokemonByNumber[50] },
    { "MEOWTH", &PokemonByNumber[51] },
    { "PERSIAN", &PokemonByNumber[52] },
    { "PSYDUCK", &PokemonByNumber[53] },
    { "GOLDUCK", &PokemonByNumber[54] },
    { "MANKEY", &PokemonByNumber[55] },
    { "PRIMEAPE", &PokemonByNumber[56] },
    { "GROWLITHE", &PokemonByNumber[57] },
    { "ARCANINE", &PokemonByNumber[58] },
    { "POLIWAG", &PokemonByNumber[59] },
    { "POLIWHIRL", &PokemonByNumber[60] },
    { "POLIWRATH", &PokemonByNumber[61] },
    { "ABRA", &PokemonByNumber[62] },
    { "KADABRA", &PokemonByNumber[63] },
    { "ALAKAZAM", &PokemonByNumber[64] },
    { "MACHOP", &PokemonByNumber[65] },
    { "MACHOKE", &PokemonByNumber[66] },
    { "MACHAMP", &PokemonByNumber[67] },
    { "BELLSPROUT", &PokemonByNumber[68] },
    { "WEEPINBELL", &PokemonByNumber[69] },
    { "VICTREEBEL", &PokemonByNumber[70] },
    { "TENTACOOL", &PokemonByNumber[71] },
    { "TENTACRUEL", &PokemonByNumber[72] },
    { "GEODUDE", &PokemonByNumber[73] },
    { "GRAVELER", &PokemonByNumber[74] },
    { "GOLEM", &PokemonByNumber[75] },
    { "PONYTA", &PokemonByNumber[76] },
    { "RAPIDASH", &PokemonByNumber[77] },
    { "SLOWPOKE", &PokemonByNumber[78] },
    { "SLOWBRO", &PokemonByNumber[79] },
    { "MAGNEMITE", &PokemonByNumber[80] },
    { "MAGNETON", &PokemonByNumber[81] },
    { "FARFETCHD", &PokemonByNumber[82] },
    { "DODUO", &PokemonByNumber[83] },
    { "DODRIO", &PokemonByNumber[84] },
    { "SEEL", &PokemonByNumber[85] },
    { "DEWGONG", &PokemonByNumber[86] },
    { "GRIMER", &PokemonByNumber[87] },
    { "MUK", &PokemonByNumber[88] },
    { "SHELLDER", &PokemonByNumber[89] },
    { "CLOYSTER", &PokemonByNumber[90] },
    { "GASTLY", &PokemonByNumber[91] },
    { "HAUNTER", &PokemonByNumber[92] },
    { "GENGAR", &PokemonByNumber[93] },
    { "ONIX", &PokemonByNumber[94] },
    { "DROWZEE", &PokemonByNumber[95] },
    { "HYPNO", &PokemonByNumber[96] },
    { "KRABBY", &PokemonByNumber[97] },
    { "KINGLER", &PokemonByNumber[98] },
    { "VOLTORB", &PokemonByNumber[99] },
    { "ELECTRODE", &PokemonByNumber[100] },
    { "EXEGGCUTE", &PokemonByNumber[101] },
    { "EXEGGUTOR", &PokemonByNumber[102] },
    { "CUBONE", &PokemonByNumber[103] },
    { "MAROWAK", &PokemonByNumber[104] },
    { "HITMONLEE", &PokemonByNumber[105] },
    { "HITMONCHAN", &PokemonByNumber[106] },
    { "LICKITUNG", &PokemonByNumber[107] },
    { "KOFFING", &PokemonByNumber[108] },
    { "WEEZING", &PokemonByNumber[109] },
    { "RHYHORN", &PokemonByNumber[110] },
    { "RHYDON", &PokemonByNumber[111] },
    { "CHANSEY", &PokemonByNumber[112] },
    { "TANGELA", &PokemonByNumber[113] },
    { "KANGASKHAN", &PokemonByNumber[114] },
    { "HORSEA", &PokemonByNumber[115] },
    { "SEADRA", &PokemonByNumber[116] },
    { "GOLDEEN", &PokemonByNumber[117] },
    { "SEAKING", &PokemonByNumber[118] },
    { "STARYU", &PokemonByNumber[119] },
    { "STARMIE", &PokemonByNumber[120] },
    { "MR_MIME", &PokemonByNumber[121] },
    { "SCYTHER", &PokemonByNumber[122] },
    { "JYNX", &PokemonByNumber[123] },
    { "ELECTABUZZ", &PokemonByNumber[124] },
    { "MAGMAR", &PokemonByNumber[125] },
    { "PINSIR", &PokemonByNumber[126] },
    { "TAUROS", &PokemonByNumber[127] },
    { "MAGIKARP", &PokemonByNumber[128] },
    { "GYARADOS", &PokemonByNumber[129] },
    { "LAPRAS", &PokemonByNumber[130] },
    { "DITTO", &PokemonByNumber[131] },
    { "EEVEE", &PokemonByNumber[132] },
    { "VAPOREON", &PokemonByNumber[133] },
    { "JOLTEON", &PokemonByNumber[134] },
    { "FLAREON", &PokemonByNumber[135] },
    { "PORYGON", &PokemonByNumber[136] },
    { "OMANYTE", &PokemonByNumber[137] },
    { "OMASTAR", &PokemonByNumber[138] },
    { "KABUTO", &PokemonByNumber[139] },
    { "KABUTOPS", &PokemonByNumber[140] },
    { "AERODACTYL", &PokemonByNumber[141] },
    { "SNORLAX", &PokemonByNumber[142] },
    { "ARTICUNO", &PokemonByNumber[143] },
    { "ZAPDOS", &PokemonByNumber[144] },
    { "MOLTRES", &PokemonByNumber[145] },
    { "DRATINI", &PokemonByNumber[146] },
    { "DRAGONAIR", &PokemonByNumber[147] },
    { "DRAGONITE", &PokemonByNumber[148] },
    { "MEWTWO", &PokemonByNumber[149] },
    { "MEW", &PokemonByNumber[150] },
    { "CHIKORITA", &PokemonByNumber[151] },
    { "BAYLEEF", &PokemonByNumber[152] },
    { "MEGANIUM", &PokemonByNumber[153] },
    { "CYNDAQUIL", &PokemonByNumber[154] },
    { "QUILAVA", &PokemonByNumber[155] },
    { "TYPHLOSION", &PokemonByNumber[156] },
    { "TOTODILE", &PokemonByNumber[157] },
    { "CROCONAW", &PokemonByNumber[158] },
    { "FERALIGATR", &PokemonByNumber[159] },
    { "SENTRET", &PokemonByNumber[160] },
    { "FURRET", &PokemonByNumber[161] },
    { "HOOTHOOT", &PokemonByNumber[162] },
    { "NOCTOWL", &PokemonByNumber[163] },
    { "LEDYBA", &PokemonByNumber[164] },
    { "LEDIAN", &PokemonByNumber[165] },
    { "SPINARAK", &PokemonByNumber[166] },
    { "ARIADOS", &PokemonByNumber[167] },
    { "CROBAT", &PokemonByNumber[168] },
    { "CHINCHOU", &PokemonByNumber[169] },
    { "LANTURN", &PokemonByNumber[170] },
    { "PICHU", &PokemonByNumber[171] },
    { "CLEFFA", &PokemonByNumber[172] },
    { "IGGLYBUFF", &PokemonByNumber[173] },
    { "TOGEPI", &PokemonByNumber[174] },
    { "TOGETIC", &PokemonByNumber[175] },
    { "NATU", &PokemonByNumber[176] },
    { "XATU", &PokemonByNumber[177] },
    { "MAREEP", &PokemonByNumber[178] },
    { "FLAAFFY", &PokemonByNumber[179] },
    { "AMPHAROS", &PokemonByNumber[180] },
    { "BELLOSSOM", &PokemonByNumber[181] },
    { "MARILL", &PokemonByNumber[182] },
    { "AZUMARILL", &PokemonByNumber[183] },
    { "SUDOWOODO", &PokemonByNumber[184] },
    { "POLITOED", &PokemonByNumber[185] },
    { "HOPPIP", &PokemonByNumber[186] },
    { "SKIPLOOM", &PokemonByNumber[187] },
    { "JUMPLUFF", &PokemonByNumber[188] },
    { "AIPOM", &PokemonByNumber[189] },
    { "SUNKERN", &PokemonByNumber[190] },
    { "SUNFLORA", &PokemonByNumber[191] },
    { "YANMA", &PokemonByNumber[192] },
    { "WOOPER", &PokemonByNumber[193] },
    { "QUAGSIRE", &PokemonByNumber[194] },
    { "ESPEON", &PokemonByNumber[195] },
    { "UMBREON", &PokemonByNumber[196] },
    { "MURKROW", &PokemonByNumber[197] },
    { "SLOWKING", &PokemonByNumber[198] },
    { "MISDREAVUS", &PokemonByNumber[199] },
    { "UNOWN", &PokemonByNumber[200] },
    { "WOBBUFFET", &PokemonByNumber[201] },
    { "GIRAFARIG", &PokemonByNumber[202] },
    { "PINECO", &PokemonByNumber[203] },
    { "FORRETRESS", &PokemonByNumber[204] },
    { "DUNSPARCE", &PokemonByNumber[205] },
    { "GLIGAR", &PokemonByNumber[206] },
    { "STEELIX", &PokemonByNumber[207] },
    { "SNUBBULL", &PokemonByNumber[208] },
    { "GRANBULL", &PokemonByNumber[209] },
    { "QWILFISH", &PokemonByNumber[210] },
    { "SCIZOR", &PokemonByNumber[211] },
    { "SHUCKLE", &PokemonByNumber[212] },
    { "HERACROSS", &PokemonByNumber[213] },
    { "SNEASEL", &PokemonByNumber[214] },
    { "TEDDIURSA", &PokemonByNumber[215] },
    { "URSARING", &PokemonByNumber[216] },
    { "SLUGMA", &PokemonByNumber[217] },
    { "MAGCARGO", &PokemonByNumber[218] },
    { "SWINUB", &PokemonByNumber[219] },
    { "PILOSWINE", &PokemonByNumber[220] },
    { "CORSOLA", &PokemonByNumber[221] },
    { "REMORAID", &PokemonByNumber[222] },
    { "OCTILLERY", &PokemonByNumber[223] },
    { "DELIBIRD", &PokemonByNumber[224] },
    { "MANTINE", &PokemonByNumber[225] },
    { "SKARMORY", &PokemonByNumber[226] },
    { "HOUNDOUR", &PokemonByNumber[227] },
    { "HOUNDOOM", &PokemonByNumber[228] },
    { "KINGDRA", &PokemonByNumber[229] },
    { "PHANPY", &PokemonByNumber[230] },
    { "DONPHAN", &PokemonByNumber[231] },
    { "PORYGON2", &PokemonByNumber[232] },
    { "STANTLER", &PokemonByNumber[233] },
    { "SMEARGLE", &PokemonByNumber[234] },
    { "TYROGUE", &PokemonByNumber[235] },
    { "HITMONTOP", &PokemonByNumber[236] },
    { "SMOOCHUM", &PokemonByNumber[237] },
    { "ELEKID", &PokemonByNumber[238] },
    { "MAGBY", &PokemonByNumber[239] },
    { "MILTANK", &PokemonByNumber[240] },
    { "BLISSEY", &PokemonByNumber[241] },
    { "RAIKOU", &PokemonByNumber[242] },
    { "ENTEI", &PokemonByNumber[243] },
    { "SUICUNE", &PokemonByNumber[244] },
    { "LARVITAR", &PokemonByNumber[245] },
    { "PUPITAR", &PokemonByNumber[246] },
    { "TYRANITAR", &PokemonByNumber[247] },
    { "LUGIA", &PokemonByNumber[248] },
    { "HO_OH", &PokemonByNumber[249] },
    { "CELEBI", &PokemonByNumber[250] },
    { "TREECKO", &PokemonByNumber[251] },
    { "GROVYLE", &PokemonByNumber[252] },
    { "SCEPTILE", &PokemonByNumber[253] },
    { "TORCHIC", &PokemonByNumber[254] },
    { "COMBUSKEN", &PokemonByNumber[255] },
    { "BLAZIKEN", &PokemonByNumber[256] },
    { "MUDKIP", &PokemonByNumber[257] },
    { "MARSHTOMP", &PokemonByNumber[258] },
    { "SWAMPERT", &PokemonByNumber[259] },
    { "POOCHYENA", &PokemonByNumber[260] },
    { "MIGHTYENA", &PokemonByNumber[261] },
    { "ZIGZAGOON", &PokemonByNumber[262] },
    { "LINOONE", &PokemonByNumber[263] },
    { "WURMPLE", &PokemonByNumber[264] },
    { "SILCOON", &PokemonByNumber[265] },
    { "BEAUTIFLY", &PokemonByNumber[266] },
    { "CASCOON", &PokemonByNumber[267] },
    { "DUSTOX", &PokemonByNumber[268] },
    { "LOTAD", &PokemonByNumber[269] },
    { "LOMBRE", &PokemonByNumber[270] },
    { "LUDICOLO", &PokemonByNumber[271] },
    { "SEEDOT", &PokemonByNumber[272] },
    { "NUZLEAF", &PokemonByNumber[273] },
    { "SHIFTRY", &PokemonByNumber[274] },
    { "TAILLOW", &PokemonByNumber[275] },
    { "SWELLOW", &PokemonByNumber[276] },
    { "WINGULL", &PokemonByNumber[277] },
    { "PELIPPER", &PokemonByNumber[278] },
    { "RALTS", &PokemonByNumber[279] },
    { "KIRLIA", &PokemonByNumber[280] },
    { "GARDEVOIR", &PokemonByNumber[281] },
    { "SURSKIT", &PokemonByNumber[282] },
    { "MASQUERAIN", &PokemonByNumber[283] },
    { "SHROOMISH", &PokemonByNumber[284] },
    { "BRELOOM", &PokemonByNumber[285] },
    { "SLAKOTH", &PokemonByNumber[286] },
    { "VIGOROTH", &PokemonByNumber[287] },
    { "SLAKING", &PokemonByNumber[288] },
    { "NINCADA", &PokemonByNumber[289] },
    { "NINJASK", &PokemonByNumber[290] },
    { "SHEDINJA", &PokemonByNumber[291] },
    { "WHISMUR", &PokemonByNumber[292] },
    { "LOUDRED", &PokemonByNumber[293] },
    { "EXPLOUD", &PokemonByNumber[294] },
    { "MAKUHITA", &PokemonByNumber[295] },
    { "HARIYAMA", &PokemonByNumber[296] },
    { "AZURILL", &PokemonByNumber[297] },
    { "NOSEPASS", &PokemonByNumber[298] },
    { "SKITTY", &PokemonByNumber[299] },
    { "DELCATTY", &PokemonByNumber[300] },
    { "SABLEYE", &PokemonByNumber[301] },
    { "MAWILE", &PokemonByNumber[302] },
    { "ARON", &PokemonByNumber[303] },
    { "LAIRON", &PokemonByNumber[304] },
    { "AGGRON", &PokemonByNumber[305] },
    { "MEDITITE", &PokemonByNumber[306] },
    { "MEDICHAM", &PokemonByNumber[307] },
    { "ELECTRIKE", &PokemonByNumber[308] },
    { "MANECTRIC", &PokemonByNumber[309] },
    { "PLUSLE", &PokemonByNumber[310] },
    { "MINUN", &PokemonByNumber[311] },
    { "VOLBEAT", &PokemonByNumber[312] },
    { "ILLUMISE", &PokemonByNumber[313] },
    { "ROSELIA", &PokemonByNumber[314] },
    { "GULPIN", &PokemonByNumber[315] },
    { "SWALOT", &PokemonByNumber[316] },
    { "CARVANHA", &PokemonByNumber[317] },
    { "SHARPEDO", &PokemonByNumber[318] },
    { "WAILMER", &PokemonByNumber[319] },
    { "WAILORD", &PokemonByNumber[320] },
    { "NUMEL", &PokemonByNumber[321] },
    { "CAMERUPT", &PokemonByNumber[322] },
    { "TORKOAL", &PokemonByNumber[323] },
    { "SPOINK", &PokemonByNumber[324] },
    { "GRUMPIG", &PokemonByNumber[325] },
    { "SPINDA", &PokemonByNumber[326] },
    { "TRAPINCH", &PokemonByNumber[327] },
    { "VIBRAVA", &PokemonByNumber[328] },
    { "FLYGON", &PokemonByNumber[329] },
    { "CACNEA", &PokemonByNumber[330] },
    { "CACTURNE", &PokemonByNumber[331] },
    { "SWABLU", &PokemonByNumber[332] },
    { "ALTARIA", &PokemonByNumber[333] },
    { "ZANGOOSE", &PokemonByNumber[334] },
    { "SEVIPER", &PokemonByNumber[335] },
    { "LUNATONE", &PokemonByNumber[336] },
    { "SOLROCK", &PokemonByNumber[337] },
    { "BARBOACH", &PokemonByNumber[338] },
    { "WHISCASH", &PokemonByNumber[339] },
    { "CORPHISH", &PokemonByNumber[340] },
    { "CRAWDAUNT", &PokemonByNumber[341] },
    { "BALTOY", &PokemonByNumber[342] },
    { "CLAYDOL", &PokemonByNumber[343] },
    { "LILEEP", &PokemonByNumber[344] },
    { "CRADILY", &PokemonByNumber[345] },
    { "ANORITH", &PokemonByNumber[346] },
    { "ARMALDO", &PokemonByNumber[347] },
    { "FEEBAS", &PokemonByNumber[348] },
    { "MILOTIC", &PokemonByNumber[349] },
    { "CASTFORM", &PokemonByNumber[350] },
    { "KECLEON", &PokemonByNumber[351] },
    { "SHUPPET", &PokemonByNumber[352] },
    { "BANETTE", &PokemonByNumber[353] },
    { "DUSKULL", &PokemonByNumber[354] },
    { "DUSCLOPS", &PokemonByNumber[355] },
    { "TROPIUS", &PokemonByNumber[356] },
    { "CHIMECHO", &PokemonByNumber[357] },
    { "ABSOL", &PokemonByNumber[358] },
    { "WYNAUT", &PokemonByNumber[359] },
    { "SNORUNT", &PokemonByNumber[360] },
    { "GLALIE", &PokemonByNumber[361] },
    { "SPHEAL", &PokemonByNumber[362] },
    { "SEALEO", &PokemonByNumber[363] },
    { "WALREIN", &PokemonByNumber[364] },
    { "CLAMPERL", &PokemonByNumber[365] },
    { "HUNTAIL", &PokemonByNumber[366] },
    { "GOREBYSS", &PokemonByNumber[367] },
    { "RELICANTH", &PokemonByNumber[368] },
    { "LUVDISC", &PokemonByNumber[369] },
    { "BAGON", &PokemonByNumber[370] },
    { "SHELGON", &PokemonByNumber[371] },
    { "SALAMENCE", &PokemonByNumber[372] },
    { "BELDUM", &PokemonByNumber[373] },
    { "METANG", &PokemonByNumber[374] },
    { "METAGROSS", &PokemonByNumber[375] },
    { "REGIROCK", &PokemonByNumber[376] },
    { "REGICE", &PokemonByNumber[377] },
    { "REGISTEEL", &PokemonByNumber[378] },
    { "LATIAS", &PokemonByNumber[379] },
    { "LATIOS", &PokemonByNumber[380] },
    { "KYOGRE", &PokemonByNumber[381] },
    { "GROUDON", &PokemonByNumber[382] },
    { "RAYQUAZA", &PokemonByNumber[383] },
    { "JIRACHI", &PokemonByNumber[384] },
    { "DEOXYS", &PokemonByNumber[385] },
    { "TURTWIG", &PokemonByNumber[386] },
    { "GROTLE", &PokemonByNumber[387] },
    { "TORTERRA", &PokemonByNumber[388] },
    { "CHIMCHAR", &PokemonByNumber[389] },
    { "MONFERNO", &PokemonByNumber[390] },
    { "INFERNAPE", &PokemonByNumber[391] },
    { "PIPLUP", &PokemonByNumber[392] },
    { "PRINPLUP", &PokemonByNumber[393] },
    { "EMPOLEON", &PokemonByNumber[394] },
    { "STARLY", &PokemonByNumber[395] },
    { "STARAVIA", &PokemonByNumber[396] },
    { "STARAPTOR", &PokemonByNumber[397] },
    { "BIDOOF", &PokemonByNumber[398] },
    { "BIBAREL", &PokemonByNumber[399] },
    { "KRICKETOT", &PokemonByNumber[400] },
    { "KRICKETUNE", &PokemonByNumber[401] },
    { "SHINX", &PokemonByNumber[402] },
    { "LUXIO", &PokemonByNumber[403] },
    { "LUXRAY", &PokemonByNumber[404] },
    { "BUDEW", &PokemonByNumber[405] },
    { "ROSERADE", &PokemonByNumber[406] },
    { "CRANIDOS", &PokemonByNumber[407] },
    { "RAMPARDOS", &PokemonByNumber[408] },
    { "SHIELDON", &PokemonByNumber[409] },
    { "BASTIODON", &PokemonByNumber[410] },
    { "BURMY", &PokemonByNumber[411] },
    { "WORMADAM", &PokemonByNumber[412] },
    { "MOTHIM", &PokemonByNumber[413] },
    { "COMBEE", &PokemonByNumber[414] },
    { "VESPIQUEN", &PokemonByNumber[415] },
    { "PACHIRISU", &PokemonByNumber[416] },
    { "BUIZEL", &PokemonByNumber[417] },
    { "FLOATZEL", &PokemonByNumber[418] },
    { "CHERUBI", &PokemonByNumber[419] },
    { "CHERRIM", &PokemonByNumber[420] },
    { "SHELLOS", &PokemonByNumber[421] },
    { "GASTRODON", &PokemonByNumber[422] },
    { "AMBIPOM", &PokemonByNumber[423] },
    { "DRIFLOON", &PokemonByNumber[424] },
    { "DRIFBLIM", &PokemonByNumber[425] },
    { "BUNEARY", &PokemonByNumber[426] },
    { "LOPUNNY", &PokemonByNumber[427] },
    { "MISMAGIUS", &PokemonByNumber[428] },
    { "HONCHKROW", &PokemonByNumber[429] },
    { "GLAMEOW", &PokemonByNumber[430] },
    { "PURUGLY", &PokemonByNumber[431] },
    { "CHINGLING", &PokemonByNumber[432] },
    { "STUNKY", &PokemonByNumber[433] },
    { "SKUNTANK", &PokemonByNumber[434] },
    { "BRONZOR", &PokemonByNumber[435] },
    { "BRONZONG", &PokemonByNumber[436] },
    { "BONSLY", &PokemonByNumber[437] },
    { "MIME_JR", &PokemonByNumber[438] },
    { "HAPPINY", &PokemonByNumber[439] },
    { "CHATOT", &PokemonByNumber[440] },
    { "SPIRITOMB", &PokemonByNumber[441] },
    { "GIBLE", &PokemonByNumber[442] },
    { "GABITE", &PokemonByNumber[443] },
    { "GARCHOMP", &PokemonByNumber[444] },
    { "MUNCHLAX", &PokemonByNumber[445] },
    { "RIOLU", &PokemonByNumber[446] },
    { "LUCARIO", &PokemonByNumber[447] },
    { "HIPPOPOTAS", &PokemonByNumber[448] },
    { "HIPPOWDON", &PokemonByNumber[449] },
    { "SKORUPI", &PokemonByNumber[450] },
    { "DRAPION", &PokemonByNumber[451] },
    { "CROAGUNK", &PokemonByNumber[452] },
    { "TOXICROAK", &PokemonByNumber[453] },
    { "CARNIVINE", &PokemonByNumber[454] },
    { "FINNEON", &PokemonByNumber[455] },
    { "LUMINEON", &PokemonByNumber[456] },
    { "MANTYKE", &PokemonByNumber[457] },
    { "SNOVER", &PokemonByNumber[458] },
    { "ABOMASNOW", &PokemonByNumber[459] },
    { "WEAVILE", &PokemonByNumber[460] },
    { "MAGNEZONE", &PokemonByNumber[461] },
    { "LICKILICKY", &PokemonByNumber[462] },
    { "RHYPERIOR", &PokemonByNumber[463] },
    { "TANGROWTH", &PokemonByNumber[464] },
    { "ELECTIVIRE", &PokemonByNumber[465] },
    { "MAGMORTAR", &PokemonByNumber[466] },
    { "TOGEKISS", &PokemonByNumber[467] },
    { "YANMEGA", &PokemonByNumber[468] },
    { "LEAFEON", &PokemonByNumber[469] },
    { "GLACEON", &PokemonByNumber[470] },
    { "GLISCOR", &PokemonByNumber[471] },
    { "MAMOSWINE", &PokemonByNumber[472] },
    { "PORYGON_Z", &PokemonByNumber[473] },
    { "GALLADE", &PokemonByNumber[474] },
    { "PROBOPASS", &PokemonByNumber[475] },
    { "DUSKNOIR", &PokemonByNumber[476] },
    { "FROSLASS", &PokemonByNumber[477] },
    { "ROTOM", &PokemonByNumber[478] },
    { "UXIE", &PokemonByNumber[479] },
    { "MESPRIT", &PokemonByNumber[480] },
    { "AZELF", &PokemonByNumber[481] },
    { "DIALGA", &PokemonByNumber[482] },
    { "PALKIA", &PokemonByNumber[483] },
    { "HEATRAN", &PokemonByNumber[484] },
    { "REGIGIGAS", &PokemonByNumber[485] },
    { "GIRATINA", &PokemonByNumber[486] },
    { "CRESSELIA", &PokemonByNumber[487] },
    { "PHIONE", &PokemonByNumber[488] },
    { "MANAPHY", &PokemonByNumber[489] },
    { "DARKRAI", &PokemonByNumber[490] },
    { "SHAYMIN", &PokemonByNumber[491] },
    { "ARCEUS", &PokemonByNumber[492] },
    { "MELTAN", &PokemonByNumber[493] },
    { "MELMETAL", &PokemonByNumber[494] },
};

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

}
