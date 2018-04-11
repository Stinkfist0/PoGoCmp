/** @file PoGoDb.h
    @brief C++ API for PoGoCmp. For C one can use PoGoCmp.h.

    Input file's timestamp 2018-02-10 02:24:00+0200 */

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
    //! How many power-ups a a level consists of.
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

enum class PokemonType : uint8_t
{
    NONE,
    BUG,
    DARK,
    DRAGON,
    ELECTRIC,
    FAIRY,
    FIGHTING,
    FIRE,
    FLYING,
    GHOST,
    GRASS,
    GROUND,
    ICE,
    NORMAL,
    POISON,
    PSYCHIC,
    ROCK,
    STEEL,
    WATER,
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

//! Pokedex number - 1 can be used as the index to the array.
static const std::array<PokemonSpecie, 386> PokemonByNumber{{
    { 1, 118, 118, 90, "BULBASAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 2, 151, 151, 120, "IVYSAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 3, 198, 198, 160, "VENUSAUR", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 4, 116, 96, 78, "CHARMANDER", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 5, 158, 129, 116, "CHARMELEON", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 6, 223, 176, 156, "CHARIZARD", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 7, 94, 122, 88, "SQUIRTLE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 8, 126, 155, 118, "WARTORTLE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 9, 171, 210, 158, "BLASTOISE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 10, 55, 62, 90, "CATERPIE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 11, 45, 94, 100, "METAPOD", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 12, 167, 151, 120, "BUTTERFREE", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 13, 63, 55, 80, "WEEDLE", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 14, 46, 86, 90, "KAKUNA", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 15, 169, 150, 130, "BEEDRILL", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 16, 85, 76, 80, "PIDGEY", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 17, 117, 108, 126, "PIDGEOTTO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 18, 166, 157, 166, "PIDGEOT", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 19, 103, 70, 60, "RATTATA", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 20, 161, 144, 110, "RATICATE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 21, 112, 61, 80, "SPEAROW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 22, 182, 135, 130, "FEAROW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 23, 110, 102, 70, "EKANS", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 24, 167, 158, 120, "ARBOK", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 25, 112, 101, 70, "PIKACHU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 26, 193, 165, 120, "RAICHU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 27, 126, 145, 100, "SANDSHREW", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 28, 182, 202, 150, "SANDSLASH", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 29, 86, 94, 110, "NIDORAN_FEMALE", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 },
    { 30, 117, 126, 140, "NIDORINA", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 },
    { 31, 180, 174, 180, "NIDOQUEEN", PokemonType::POISON, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 100, 0 },
    { 32, 105, 76, 92, "NIDORAN_MALE", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 },
    { 33, 137, 112, 122, "NIDORINO", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 },
    { 34, 204, 157, 162, "NIDOKING", PokemonType::POISON, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 0, 100 },
    { 35, 107, 116, 140, "CLEFAIRY", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 75, 25 },
    { 36, 178, 171, 190, "CLEFABLE", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 75, 25 },
    { 37, 96, 122, 76, "VULPIX", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 38, 169, 204, 146, "NINETALES", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 39, 80, 44, 230, "JIGGLYPUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 75, 25 },
    { 40, 156, 93, 280, "WIGGLYTUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 75, 25 },
    { 41, 83, 76, 80, "ZUBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 42, 161, 153, 150, "GOLBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 43, 131, 116, 90, "ODDISH", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 44, 153, 139, 120, "GLOOM", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 45, 202, 170, 150, "VILEPLUME", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 46, 121, 99, 70, "PARAS", PokemonType::BUG, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 47, 165, 146, 120, "PARASECT", PokemonType::BUG, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 48, 100, 102, 120, "VENONAT", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 49, 179, 150, 140, "VENOMOTH", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 50, 109, 88, 20, "DIGLETT", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 51, 167, 147, 70, "DUGTRIO", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 52, 92, 81, 80, "MEOWTH", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 53, 150, 139, 130, "PERSIAN", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 54, 122, 96, 100, "PSYDUCK", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 55, 191, 163, 160, "GOLDUCK", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 56, 148, 87, 80, "MANKEY", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 57, 207, 144, 130, "PRIMEAPE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 58, 136, 96, 110, "GROWLITHE", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 59, 227, 166, 180, "ARCANINE", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 60, 101, 82, 80, "POLIWAG", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 61, 130, 130, 130, "POLIWHIRL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 62, 182, 187, 180, "POLIWRATH", PokemonType::WATER, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 63, 195, 103, 50, "ABRA", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 64, 232, 138, 80, "KADABRA", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 65, 271, 194, 110, "ALAKAZAM", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 66, 137, 88, 140, "MACHOP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 67, 177, 130, 160, "MACHOKE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 68, 234, 162, 180, "MACHAMP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 69, 139, 64, 100, "BELLSPROUT", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 70, 172, 95, 130, "WEEPINBELL", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 71, 207, 138, 160, "VICTREEBEL", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 72, 97, 182, 80, "TENTACOOL", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 73, 166, 237, 160, "TENTACRUEL", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 74, 132, 163, 80, "GEODUDE", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 75, 164, 196, 110, "GRAVELER", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 76, 211, 229, 160, "GOLEM", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 77, 170, 132, 100, "PONYTA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 78, 207, 167, 130, "RAPIDASH", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 79, 109, 109, 180, "SLOWPOKE", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 80, 177, 194, 190, "SLOWBRO", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 81, 165, 128, 50, "MAGNEMITE", PokemonType::ELECTRIC, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 0, 0 },
    { 82, 223, 182, 100, "MAGNETON", PokemonType::ELECTRIC, PokemonType::STEEL, PokemonRarity::NORMAL, 3, 0, 0 },
    { 83, 124, 118, 104, "FARFETCHD", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 84, 158, 88, 70, "DODUO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 85, 218, 145, 120, "DODRIO", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 86, 85, 128, 130, "SEEL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 87, 139, 184, 180, "DEWGONG", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 88, 135, 90, 160, "GRIMER", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 89, 190, 184, 210, "MUK", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 90, 116, 168, 60, "SHELLDER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 91, 186, 323, 100, "CLOYSTER", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 92, 186, 70, 60, "GASTLY", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 93, 223, 112, 90, "HAUNTER", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 94, 261, 156, 120, "GENGAR", PokemonType::GHOST, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 95, 85, 288, 70, "ONIX", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 96, 89, 158, 120, "DROWZEE", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 97, 144, 215, 170, "HYPNO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 98, 181, 156, 60, "KRABBY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 99, 240, 214, 110, "KINGLER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 100, 109, 114, 80, "VOLTORB", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 101, 173, 179, 120, "ELECTRODE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 102, 107, 140, 120, "EXEGGCUTE", PokemonType::GRASS, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 103, 233, 158, 190, "EXEGGUTOR", PokemonType::GRASS, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 104, 90, 165, 100, "CUBONE", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 105, 144, 200, 120, "MAROWAK", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 106, 224, 211, 100, "HITMONLEE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 },
    { 107, 193, 212, 100, "HITMONCHAN", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 },
    { 108, 108, 137, 180, "LICKITUNG", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 109, 119, 164, 80, "KOFFING", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 110, 174, 221, 130, "WEEZING", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 111, 140, 157, 160, "RHYHORN", PokemonType::GROUND, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 112, 222, 206, 210, "RHYDON", PokemonType::GROUND, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 113, 60, 176, 500, "CHANSEY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 },
    { 114, 183, 205, 130, "TANGELA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 115, 181, 165, 210, "KANGASKHAN", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 },
    { 116, 129, 125, 60, "HORSEA", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 117, 187, 182, 110, "SEADRA", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 118, 123, 115, 90, "GOLDEEN", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 119, 175, 154, 160, "SEAKING", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 120, 137, 112, 60, "STARYU", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 121, 210, 184, 120, "STARMIE", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 122, 192, 233, 80, "MR_MIME", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 123, 218, 170, 140, "SCYTHER", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 124, 223, 182, 130, "JYNX", PokemonType::ICE, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 100, 0 },
    { 125, 198, 173, 130, "ELECTABUZZ", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 25, 75 },
    { 126, 206, 169, 130, "MAGMAR", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 25, 75 },
    { 127, 238, 197, 130, "PINSIR", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 128, 198, 197, 150, "TAUROS", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 },
    { 129, 29, 102, 40, "MAGIKARP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 130, 237, 197, 190, "GYARADOS", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 131, 165, 180, 260, "LAPRAS", PokemonType::WATER, PokemonType::ICE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 132, 91, 91, 96, "DITTO", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 133, 104, 121, 110, "EEVEE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 134, 205, 177, 260, "VAPOREON", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 135, 232, 201, 130, "JOLTEON", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 136, 246, 204, 130, "FLAREON", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 137, 153, 139, 130, "PORYGON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 138, 155, 174, 70, "OMANYTE", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 139, 207, 227, 140, "OMASTAR", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 140, 148, 162, 60, "KABUTO", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 141, 220, 203, 120, "KABUTOPS", PokemonType::ROCK, PokemonType::WATER, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 142, 221, 164, 160, "AERODACTYL", PokemonType::ROCK, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 143, 190, 190, 320, "SNORLAX", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 144, 192, 249, 180, "ARTICUNO", PokemonType::ICE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 145, 253, 188, 180, "ZAPDOS", PokemonType::ELECTRIC, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 146, 251, 184, 180, "MOLTRES", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 147, 119, 94, 82, "DRATINI", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 148, 163, 138, 122, "DRAGONAIR", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 149, 263, 201, 182, "DRAGONITE", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 150, 300, 182, 193, "MEWTWO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 151, 210, 210, 200, "MEW", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 152, 92, 122, 90, "CHIKORITA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 153, 122, 155, 120, "BAYLEEF", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 154, 168, 202, 160, "MEGANIUM", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 155, 116, 96, 78, "CYNDAQUIL", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 156, 158, 129, 116, "QUILAVA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 157, 223, 176, 156, "TYPHLOSION", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 158, 117, 116, 100, "TOTODILE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 159, 150, 151, 130, "CROCONAW", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 160, 205, 197, 170, "FERALIGATR", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 161, 79, 77, 70, "SENTRET", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 162, 148, 130, 170, "FURRET", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 163, 67, 101, 120, "HOOTHOOT", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 164, 145, 179, 200, "NOCTOWL", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 165, 72, 142, 80, "LEDYBA", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 166, 107, 209, 110, "LEDIAN", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 167, 105, 73, 80, "SPINARAK", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 168, 161, 128, 140, "ARIADOS", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 169, 194, 178, 170, "CROBAT", PokemonType::POISON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 170, 106, 106, 150, "CHINCHOU", PokemonType::WATER, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 171, 146, 146, 250, "LANTURN", PokemonType::WATER, PokemonType::ELECTRIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 172, 77, 63, 40, "PICHU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 173, 75, 91, 100, "CLEFFA", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 1, 75, 25 },
    { 174, 69, 34, 180, "IGGLYBUFF", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 1, 75, 25 },
    { 175, 67, 116, 70, "TOGEPI", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 176, 139, 191, 110, "TOGETIC", PokemonType::FAIRY, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 177, 134, 89, 80, "NATU", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 178, 192, 146, 130, "XATU", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 179, 114, 82, 110, "MAREEP", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 180, 145, 112, 140, "FLAAFFY", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 181, 211, 172, 180, "AMPHAROS", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 182, 169, 189, 150, "BELLOSSOM", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 183, 37, 93, 140, "MARILL", PokemonType::WATER, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 50, 50 },
    { 184, 112, 152, 200, "AZUMARILL", PokemonType::WATER, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 50, 50 },
    { 185, 167, 198, 140, "SUDOWOODO", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 186, 174, 192, 180, "POLITOED", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 187, 67, 101, 70, "HOPPIP", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 188, 91, 127, 110, "SKIPLOOM", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 189, 118, 197, 150, "JUMPLUFF", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 190, 136, 112, 110, "AIPOM", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 191, 55, 55, 60, "SUNKERN", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 192, 185, 148, 150, "SUNFLORA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 193, 154, 94, 130, "YANMA", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 194, 75, 75, 110, "WOOPER", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 195, 152, 152, 190, "QUAGSIRE", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 196, 261, 194, 130, "ESPEON", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 197, 126, 250, 190, "UMBREON", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 198, 175, 87, 120, "MURKROW", PokemonType::DARK, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 199, 177, 194, 190, "SLOWKING", PokemonType::WATER, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 200, 167, 167, 120, "MISDREAVUS", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 201, 136, 91, 96, "UNOWN", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 0 },
    { 202, 60, 106, 380, "WOBBUFFET", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 203, 182, 133, 140, "GIRAFARIG", PokemonType::NORMAL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 204, 108, 146, 100, "PINECO", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 205, 161, 242, 150, "FORRETRESS", PokemonType::BUG, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 50, 50 },
    { 206, 131, 131, 200, "DUNSPARCE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 207, 143, 204, 130, "GLIGAR", PokemonType::GROUND, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 208, 148, 333, 150, "STEELIX", PokemonType::STEEL, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 209, 137, 89, 120, "SNUBBULL", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 210, 212, 137, 180, "GRANBULL", PokemonType::FAIRY, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 211, 184, 148, 130, "QWILFISH", PokemonType::WATER, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 212, 236, 191, 140, "SCIZOR", PokemonType::BUG, PokemonType::STEEL, PokemonRarity::NORMAL, 5, 50, 50 },
    { 213, 17, 396, 40, "SHUCKLE", PokemonType::BUG, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 214, 234, 189, 160, "HERACROSS", PokemonType::BUG, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 215, 189, 157, 110, "SNEASEL", PokemonType::DARK, PokemonType::ICE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 216, 142, 93, 120, "TEDDIURSA", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 217, 236, 144, 180, "URSARING", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 218, 118, 71, 80, "SLUGMA", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 219, 139, 209, 100, "MAGCARGO", PokemonType::FIRE, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 },
    { 220, 90, 74, 100, "SWINUB", PokemonType::ICE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 221, 181, 147, 200, "PILOSWINE", PokemonType::ICE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 222, 118, 156, 110, "CORSOLA", PokemonType::WATER, PokemonType::ROCK, PokemonRarity::NORMAL, 3, 75, 25 },
    { 223, 127, 69, 70, "REMORAID", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 224, 197, 141, 150, "OCTILLERY", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 225, 128, 90, 90, "DELIBIRD", PokemonType::ICE, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 226, 148, 260, 130, "MANTINE", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 227, 148, 260, 130, "SKARMORY", PokemonType::STEEL, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 228, 152, 93, 90, "HOUNDOUR", PokemonType::DARK, PokemonType::FIRE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 229, 224, 159, 150, "HOUNDOOM", PokemonType::DARK, PokemonType::FIRE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 230, 194, 194, 150, "KINGDRA", PokemonType::WATER, PokemonType::DRAGON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 231, 107, 107, 180, "PHANPY", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 232, 214, 214, 180, "DONPHAN", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 233, 198, 183, 170, "PORYGON2", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 0 },
    { 234, 192, 132, 146, "STANTLER", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 235, 40, 88, 110, "SMEARGLE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 236, 64, 64, 70, "TYROGUE", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 },
    { 237, 173, 214, 100, "HITMONTOP", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 5, 0, 100 },
    { 238, 153, 116, 90, "SMOOCHUM", PokemonType::ICE, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 100, 0 },
    { 239, 135, 110, 90, "ELEKID", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 25, 75 },
    { 240, 151, 108, 90, "MAGBY", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 5, 25, 75 },
    { 241, 157, 211, 190, "MILTANK", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 },
    { 242, 129, 229, 510, "BLISSEY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 100, 0 },
    { 243, 241, 210, 180, "RAIKOU", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 244, 235, 176, 230, "ENTEI", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 245, 180, 235, 200, "SUICUNE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 246, 115, 93, 100, "LARVITAR", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 247, 155, 133, 140, "PUPITAR", PokemonType::ROCK, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 248, 251, 212, 200, "TYRANITAR", PokemonType::ROCK, PokemonType::DARK, PokemonRarity::NORMAL, 5, 50, 50 },
    { 249, 193, 323, 212, "LUGIA", PokemonType::PSYCHIC, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 250, 239, 274, 193, "HO_OH", PokemonType::FIRE, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 251, 210, 210, 200, "CELEBI", PokemonType::PSYCHIC, PokemonType::GRASS, PokemonRarity::MYTHIC, 5, 0, 0 },
    { 252, 124, 104, 80, "TREECKO", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 253, 172, 130, 100, "GROVYLE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 254, 223, 180, 140, "SCEPTILE", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 255, 130, 92, 90, "TORCHIC", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 256, 163, 115, 120, "COMBUSKEN", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 257, 240, 141, 160, "BLAZIKEN", PokemonType::FIRE, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 258, 126, 93, 100, "MUDKIP", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 259, 156, 133, 140, "MARSHTOMP", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 260, 208, 175, 200, "SWAMPERT", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 261, 96, 63, 70, "POOCHYENA", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 262, 171, 137, 140, "MIGHTYENA", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 263, 58, 80, 76, "ZIGZAGOON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 264, 142, 128, 156, "LINOONE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 265, 75, 61, 90, "WURMPLE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 266, 60, 91, 100, "SILCOON", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 267, 189, 98, 120, "BEAUTIFLY", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 268, 60, 91, 100, "CASCOON", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 269, 98, 172, 120, "DUSTOX", PokemonType::BUG, PokemonType::POISON, PokemonRarity::NORMAL, 1, 50, 50 },
    { 270, 71, 86, 80, "LOTAD", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 271, 112, 128, 120, "LOMBRE", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 272, 173, 191, 160, "LUDICOLO", PokemonType::WATER, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 50, 50 },
    { 273, 71, 86, 80, "SEEDOT", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 274, 134, 78, 140, "NUZLEAF", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 275, 200, 121, 180, "SHIFTRY", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 276, 106, 61, 80, "TAILLOW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 277, 185, 130, 120, "SWELLOW", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 278, 106, 61, 80, "WINGULL", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 279, 175, 189, 120, "PELIPPER", PokemonType::WATER, PokemonType::FLYING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 280, 79, 63, 56, "RALTS", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 281, 117, 100, 76, "KIRLIA", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 282, 237, 220, 136, "GARDEVOIR", PokemonType::PSYCHIC, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 283, 93, 97, 80, "SURSKIT", PokemonType::BUG, PokemonType::WATER, PokemonRarity::NORMAL, 1, 50, 50 },
    { 284, 192, 161, 140, "MASQUERAIN", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 285, 74, 110, 120, "SHROOMISH", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 286, 241, 153, 120, "BRELOOM", PokemonType::GRASS, PokemonType::FIGHTING, PokemonRarity::NORMAL, 3, 50, 50 },
    { 287, 104, 104, 120, "SLAKOTH", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 288, 159, 159, 160, "VIGOROTH", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 289, 290, 183, 273, "SLAKING", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 290, 80, 153, 62, "NINCADA", PokemonType::BUG, PokemonType::GROUND, PokemonRarity::NORMAL, 5, 50, 50 },
    { 291, 196, 114, 122, "NINJASK", PokemonType::BUG, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 292, 153, 80, 2, "SHEDINJA", PokemonType::BUG, PokemonType::GHOST, PokemonRarity::NORMAL, 5, 0, 0 },
    { 293, 92, 42, 128, "WHISMUR", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 294, 134, 81, 168, "LOUDRED", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 295, 179, 142, 208, "EXPLOUD", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 296, 99, 54, 144, "MAKUHITA", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 297, 209, 114, 288, "HARIYAMA", PokemonType::FIGHTING, PokemonType::NONE, PokemonRarity::NORMAL, 3, 25, 75 },
    { 298, 36, 71, 100, "AZURILL", PokemonType::NORMAL, PokemonType::FAIRY, PokemonRarity::NORMAL, 3, 75, 25 },
    { 299, 82, 236, 60, "NOSEPASS", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 300, 84, 84, 100, "SKITTY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 301, 132, 132, 140, "DELCATTY", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 302, 141, 141, 100, "SABLEYE", PokemonType::DARK, PokemonType::GHOST, PokemonRarity::NORMAL, 5, 50, 50 },
    { 303, 155, 155, 100, "MAWILE", PokemonType::STEEL, PokemonType::FAIRY, PokemonRarity::NORMAL, 5, 50, 50 },
    { 304, 121, 168, 100, "ARON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 },
    { 305, 158, 240, 120, "LAIRON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 },
    { 306, 198, 314, 140, "AGGRON", PokemonType::STEEL, PokemonType::ROCK, PokemonRarity::NORMAL, 1, 50, 50 },
    { 307, 78, 107, 60, "MEDITITE", PokemonType::FIGHTING, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 308, 121, 152, 120, "MEDICHAM", PokemonType::FIGHTING, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 50, 50 },
    { 309, 123, 78, 80, "ELECTRIKE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 310, 215, 127, 140, "MANECTRIC", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 311, 167, 147, 120, "PLUSLE", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 312, 147, 167, 120, "MINUN", PokemonType::ELECTRIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 313, 143, 171, 130, "VOLBEAT", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 3, 0, 100 },
    { 314, 143, 171, 130, "ILLUMISE", PokemonType::BUG, PokemonType::NONE, PokemonRarity::NORMAL, 3, 100, 0 },
    { 315, 186, 148, 100, "ROSELIA", PokemonType::GRASS, PokemonType::POISON, PokemonRarity::NORMAL, 3, 50, 50 },
    { 316, 80, 99, 140, "GULPIN", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 317, 140, 159, 200, "SWALOT", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 318, 171, 39, 90, "CARVANHA", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 319, 243, 83, 140, "SHARPEDO", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 320, 136, 68, 260, "WAILMER", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 321, 175, 87, 340, "WAILORD", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 322, 119, 82, 120, "NUMEL", PokemonType::FIRE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 323, 194, 139, 140, "CAMERUPT", PokemonType::FIRE, PokemonType::GROUND, PokemonRarity::NORMAL, 3, 50, 50 },
    { 324, 151, 234, 140, "TORKOAL", PokemonType::FIRE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 325, 125, 145, 120, "SPOINK", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 326, 171, 211, 160, "GRUMPIG", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 1, 50, 50 },
    { 327, 116, 116, 120, "SPINDA", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 328, 162, 78, 90, "TRAPINCH", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 329, 134, 99, 100, "VIBRAVA", PokemonType::GROUND, PokemonType::DRAGON, PokemonRarity::NORMAL, 5, 50, 50 },
    { 330, 205, 168, 160, "FLYGON", PokemonType::GROUND, PokemonType::DRAGON, PokemonRarity::NORMAL, 5, 50, 50 },
    { 331, 156, 74, 100, "CACNEA", PokemonType::GRASS, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 332, 221, 115, 140, "CACTURNE", PokemonType::GRASS, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 333, 76, 139, 90, "SWABLU", PokemonType::NORMAL, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 334, 141, 208, 150, "ALTARIA", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 1, 50, 50 },
    { 335, 222, 124, 146, "ZANGOOSE", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 336, 196, 118, 146, "SEVIPER", PokemonType::POISON, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 337, 178, 163, 180, "LUNATONE", PokemonType::ROCK, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 338, 178, 163, 180, "SOLROCK", PokemonType::ROCK, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 339, 93, 83, 100, "BARBOACH", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 340, 151, 142, 220, "WHISCASH", PokemonType::WATER, PokemonType::GROUND, PokemonRarity::NORMAL, 1, 50, 50 },
    { 341, 141, 113, 86, "CORPHISH", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 342, 224, 156, 126, "CRAWDAUNT", PokemonType::WATER, PokemonType::DARK, PokemonRarity::NORMAL, 3, 50, 50 },
    { 343, 77, 131, 80, "BALTOY", PokemonType::GROUND, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 344, 140, 236, 120, "CLAYDOL", PokemonType::GROUND, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 3, 0, 0 },
    { 345, 105, 154, 132, "LILEEP", PokemonType::ROCK, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 346, 152, 198, 172, "CRADILY", PokemonType::ROCK, PokemonType::GRASS, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 347, 176, 100, 90, "ANORITH", PokemonType::ROCK, PokemonType::BUG, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 348, 222, 183, 150, "ARMALDO", PokemonType::ROCK, PokemonType::BUG, PokemonRarity::NORMAL, 3, 12.5, 87.5 },
    { 349, 29, 102, 40, "FEEBAS", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 350, 192, 242, 190, "MILOTIC", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 351, 139, 139, 140, "CASTFORM", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 352, 161, 212, 120, "KECLEON", PokemonType::NORMAL, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 353, 138, 66, 88, "SHUPPET", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 354, 218, 127, 128, "BANETTE", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 355, 70, 162, 40, "DUSKULL", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 356, 124, 234, 80, "DUSCLOPS", PokemonType::GHOST, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 357, 136, 165, 198, "TROPIUS", PokemonType::GRASS, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 358, 175, 174, 150, "CHIMECHO", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 359, 246, 120, 130, "ABSOL", PokemonType::DARK, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 360, 41, 86, 190, "WYNAUT", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 361, 95, 95, 100, "SNORUNT", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 362, 162, 162, 160, "GLALIE", PokemonType::ICE, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 363, 95, 90, 140, "SPHEAL", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 },
    { 364, 137, 132, 180, "SEALEO", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 },
    { 365, 182, 176, 220, "WALREIN", PokemonType::ICE, PokemonType::WATER, PokemonRarity::NORMAL, 3, 50, 50 },
    { 366, 133, 149, 70, "CLAMPERL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 367, 197, 194, 110, "HUNTAIL", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 368, 211, 194, 110, "GOREBYSS", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 50, 50 },
    { 369, 162, 234, 200, "RELICANTH", PokemonType::WATER, PokemonType::ROCK, PokemonRarity::NORMAL, 5, 12.5, 87.5 },
    { 370, 81, 134, 86, "LUVDISC", PokemonType::WATER, PokemonType::NONE, PokemonRarity::NORMAL, 3, 75, 25 },
    { 371, 134, 107, 90, "BAGON", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 372, 172, 179, 130, "SHELGON", PokemonType::DRAGON, PokemonType::NONE, PokemonRarity::NORMAL, 5, 50, 50 },
    { 373, 277, 168, 190, "SALAMENCE", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::NORMAL, 5, 50, 50 },
    { 374, 96, 141, 80, "BELDUM", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 },
    { 375, 138, 185, 120, "METANG", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 },
    { 376, 257, 247, 160, "METAGROSS", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::NORMAL, 5, 0, 0 },
    { 377, 179, 356, 160, "REGIROCK", PokemonType::ROCK, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 378, 179, 356, 160, "REGICE", PokemonType::ICE, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 379, 143, 285, 160, "REGISTEEL", PokemonType::STEEL, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 380, 228, 268, 160, "LATIAS", PokemonType::DRAGON, PokemonType::PSYCHIC, PokemonRarity::LEGENDARY, 20, 100, 0 },
    { 381, 268, 228, 160, "LATIOS", PokemonType::DRAGON, PokemonType::PSYCHIC, PokemonRarity::LEGENDARY, 20, 0, 100 },
    { 382, 270, 251, 182, "KYOGRE", PokemonType::WATER, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 383, 270, 251, 182, "GROUDON", PokemonType::GROUND, PokemonType::NONE, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 384, 284, 170, 191, "RAYQUAZA", PokemonType::DRAGON, PokemonType::FLYING, PokemonRarity::LEGENDARY, 20, 0, 0 },
    { 385, 210, 210, 200, "JIRACHI", PokemonType::STEEL, PokemonType::PSYCHIC, PokemonRarity::MYTHIC, 20, 0, 0 },
    { 386, 1, 1, 1, "DEOXYS", PokemonType::PSYCHIC, PokemonType::NONE, PokemonRarity::MYTHIC, 20, 0, 0 },
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

//! Pokedex number range, number - 1 for the index in PoGoCmp::PokemonByNumber.
using PokedexRange = std::pair<size_t, size_t>;
const PokedexRange Gen1Range{ 1, 151 };
const PokedexRange Gen2Range{ 152, 251 };
const PokedexRange Gen3Range{ 252, 386 };
const PokedexRange MaxRange{ 1, PoGoCmp::PokemonByNumber.size() };

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
