/**
    @file PoGoCmP.h
    @brief C API for PoGoCmp. For C++ one can use the PoGoCmpDb.h directly. */
#pragma once

#include "PoGoCmpApi.h"

#include <stdint.h>
#include <stddef.h>

typedef enum PokemonType
{
    POKEMON_TYPE_NONE,
    POKEMON_TYPE_BUG,
    POKEMON_TYPE_DARK,
    POKEMON_TYPE_DRAGON,
    POKEMON_TYPE_ELECTRIC,
    POKEMON_TYPE_FAIRY,
    POKEMON_TYPE_FIGHTING,
    POKEMON_TYPE_FIRE,
    POKEMON_TYPE_FLYING,
    POKEMON_TYPE_GHOST,
    POKEMON_TYPE_GRASS,
    POKEMON_TYPE_GROUND,
    POKEMON_TYPE_ICE,
    POKEMON_TYPE_NORMAL,
    POKEMON_TYPE_POISON,
    POKEMON_TYPE_PSYCHIC,
    POKEMON_TYPE_ROCK,
    POKEMON_TYPE_STEEL,
    POKEMON_TYPE_WATER
} PokemonType;

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
    //! Pokémon's specie name, uppercase with underscores
    //! Punctuation and other "special" characters are replaced with underscores.
    //! There are only a handful of Pokémon with special character's in their names:
    //! - Farfetch'd -> FARFETCHD
    //! - Ho-Oh -> HO_OH
    //! - Flabébé -> Unknown at the moment, probably FLABEBE
    //! - Nidoran♂  & Nidoran♀ -> NIDORAN_MALE & NIDORAN_FEMALE
    //! The longest name (Crabominable) currently (in a distant PoGO future) has 12 characters,
    //! but as Nidoran♀ is translated into NIDORAN_FEMALE the longest name has 14 characters.
    //! https://bulbapedia.bulbagarden.net/wiki/List_of_Pokémon_by_name
    char name[15];
    //! Primary type.
    PokemonType type;
    //! Secondary type, if applicable.
    PokemonType type2;
};

#ifdef __cplusplus
extern "C" {
#endif

POGOCMP_API const char* PoGoCmpVersionString();

POGOCMP_API size_t NumPokemonInDb();

#ifdef __cplusplus
}
#endif
