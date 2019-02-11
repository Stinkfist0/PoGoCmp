/**
    @file PoGoCmP.cpp
    @brief */
#include "PoGoCmp.h"
#include "PoGoDb.h"

#include <cmath>

namespace PoGoCmp
{

const char* VersionString() { return "0.0.1"; }

float GetCpm(float level)
{
    const auto numLevels = (float)PoGoCmp::PlayerLevel.cpMultiplier.size();
    if (level  < 1 || level > numLevels) return NAN;
    float levelIdx;
    auto levelFact = std::modf(level, &levelIdx);
    if (levelFact != 0.f && levelFact != 0.5f) return NAN;
    levelIdx -= 1;
    auto nextLevelIdx = std::min(levelIdx + 1, numLevels - 1);
    auto cpmBase = PoGoCmp::PlayerLevel.cpMultiplier[(size_t)levelIdx];
    auto cpmNext = PoGoCmp::PlayerLevel.cpMultiplier[(size_t)nextLevelIdx];
    auto cpmStep = (std::pow(cpmNext, 2) - std::pow(cpmBase, 2)) / 2.f;
    auto cpm = levelFact != 0.f ? std::sqrt(std::pow(cpmBase, 2) + cpmStep) : cpmBase;
    return cpm;
}

int ComputeCp(float level, float atk, float def, float sta)
{
    auto cpm = GetCpm(level);
    if (std::isnan(cpm)) return -1;
    return static_cast<int>(std::floor(atk * std::pow(def, 0.5f) * std::pow(sta, 0.5f) * std::pow(cpm, 2) / 10.f));
}

int ComputeCp(const PoGoCmp::PokemonSpecie& base, float level, float atk, float def, float sta)
{
    if (atk < 0 || atk > 15) return -1;
    if (def < 0 || def > 15) return -1;
    if (sta < 0 || sta > 15) return -1;
    atk = base.baseAtk + atk;
    def = base.baseDef + def;
    sta = base.baseSta + sta;
    return ComputeCp(level, atk, def, sta);
}

int ComputeCp(const PoGoCmp::PokemonSpecie& base, const PoGoCmp::Pokemon& pkm)
{
    return ComputeCp(base, pkm.level, pkm.atk, pkm.def, pkm.sta);
}

int ComputeStat(int base, int iv, float level)
{
    return int(float(base + iv) * GetCpm(level));
}

int ComputeRaidBossCp(const PoGoCmp::PokemonSpecie& base, const PoGoCmp::Pokemon& pkm)
{
    auto atk = base.baseAtk + pkm.atk;
    auto def = base.baseDef + pkm.def;
    auto sta = pkm.sta; // raid bosses have a fixed stamina and base stamina is ignored
    return (int)std::floor(atk * std::sqrt(def) * std::sqrt(sta) / 10.f);
}

int MinCp(const PoGoCmp::PokemonSpecie& base)
{
    return ComputeCp(base, 1, 0, 0, 0);
}

int MaxCp(const PoGoCmp::PokemonSpecie& base)
{
    return ComputeCp(base, (float)PoGoCmp::PlayerLevel.cpMultiplier.size(), 15, 15, 15);
}

}
