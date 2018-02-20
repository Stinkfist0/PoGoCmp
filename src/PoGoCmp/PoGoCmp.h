/**
    @file PoGoCmP.h
    @brief C API for PoGoCmp. For C++ one can use the PoGoCmpDb.h directly. */
#pragma once

#ifdef POGOCMP_SHARED_BUILD
#ifdef _WIN32
#ifdef POGOCMP_EXPORTS
#define POGOCMP_API __declspec(dllexport)
#else
#define POGOCMP_API __declspec(dllimport)
#endif
#else // simply assume *nix + GCC-like compiler
#define POGOCMP_API __attribute__((visibility("default")))
#endif
#else
#define POGOCMP_API
#endif

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
    /// Pokedex number.
    uint16_t number;
    /// Base attack.
    uint16_t baseAtk;
    /// Base defence.
    uint16_t baseDef;
    /// Base stamina (a.k.a. HP).
    uint16_t baseSta;
    /// Pokemon's specie name, uppercase.
    /// Punctuation and other "special" characters are replaced with underscores.
    char name[10];
    /// Primary type.
    PokemonType type;
    /// Secondary type, if applicable.
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
