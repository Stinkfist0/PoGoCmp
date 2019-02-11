/**
    @file PoGoCmP.h
    @brief */
#pragma once

#include "PoGoCmpApi.h"

namespace PoGoCmp
{

struct PokemonSpecie;
struct Pokemon;

const char* VersionString();

float GetCpm(float level);

//! @param level [1,maxLevel], 0.5 steps, maxLevel 40 for now.
//! @param atk baseAtk + atkIv, integer.
//! @param def baseDef + defIv, integer.
//! @param sta baseSta + staIv, integer.
//! @note https://pokemongo.gamepress.gg/pokemon-stats-advanced
int ComputeCp(float level, float atk, float def, float sta);
//! @param base Pokémon's base stats.
//! @param level [1,maxLevel], 0.5 steps, maxLevel 40 for now.
//! @param atk [0, 15], integer.
//! @param def [0, 15], integer.
//! @param sta [0, 15], integer.
//! @note The game doesn't show CP under 10 but this function returns the actual CP even for values below 10.
//! @return < 0 on invalid input, > 0 otherwise
int ComputeCp(const PokemonSpecie& base, float level, float atk, float def, float sta);
int ComputeCp(const PokemonSpecie& base, const PoGoCmp::Pokemon& pkm);

int ComputeStat(int base, int iv, float level);

//! https://www.reddit.com/r/TheSilphRoad/comments/6wrw6a/raid_boss_cp_explained_if_it_hasnt_been_already/
//! @note For some reasons raid bosses have have different arbitrary formula without CPM.
int ComputeRaidBossCp(const PokemonSpecie& base, const PoGoCmp::Pokemon& pkm);

int MinCp(const PokemonSpecie& base);
int MaxCp(const PokemonSpecie& base);

} // ~namespace PoGoCmp
