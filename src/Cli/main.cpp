#include "ProgramOptions.h"

#include "../Lib/PoGoCmp.h"
#include "../Lib/PoGoDb.h"
#include "../Lib/MathUtils.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <regex>
#include <functional>
#include <cmath>
#include <climits>
#include <cassert>

const Utf8::String defaultFormat{ "%nu %na ATK %ba DEF %bd STA %bs TYPE %Tt CP %cp\\n" };

void LogE(const Utf8::String& msg)
{
    Utf8::PrintLine("Error: " + msg, Utf8::OutputStream::Err);
}

void LogErrorAndExit(const Utf8::String& msg)
{
    LogE(msg);
    std::exit(EXIT_FAILURE);
}

void LogErrorAndExit(const std::wstring& msg)
{
    LogE(Utf8::FromWString(msg));
    std::exit(EXIT_FAILURE);
}

void Log(const Utf8::String& msg)
{
    Utf8::PrintLine(msg, Utf8::OutputStream::Out);
}

void Log(const std::wstring& msg)
{
    Utf8::PrintLine(Utf8::FromWString(msg), Utf8::OutputStream::Out);
}

//! @todo Move CP functions to the PoGoCmp API
float GetCpm(float level)
{
    const auto numLevels = (float)PoGoCmp::PlayerLevel.cpMultiplier.size();
    if (level  < 1 || level > numLevels) return NAN;
    float levelIdx;
    auto levelFact = std::modf(level, &levelIdx);
    if (levelFact != 0.f && levelFact != 0.5f) return NAN;
    levelIdx -= 1;
    auto nextLevelIdx = std::min(levelIdx + 1, numLevels -1);
    auto cpmBase = PoGoCmp::PlayerLevel.cpMultiplier[(size_t)levelIdx];
    auto cpmNext = PoGoCmp::PlayerLevel.cpMultiplier[(size_t)nextLevelIdx];
    auto cpmStep = (std::pow(cpmNext, 2) - std::pow(cpmBase, 2)) / 2.f;
    auto cpm = levelFact != 0.f ? std::sqrt(std::pow(cpmBase, 2) + cpmStep) : cpmBase;
    return cpm;
}

//! @param level [1,maxLevel], 0.5 steps, maxLevel 40 for now.
//! @param atk baseAtk + atkIv, integer.
//! @param def baseDef + defIv, integer.
//! @param sta baseSta + staIv, integer.
int ComputeCp(float level, float atk, float def, float sta)
{
    auto cpm = GetCpm(level);
    if (std::isnan(cpm)) return -1;
    return static_cast<int>(std::floor(atk * std::pow(def, 0.5f) * std::pow(sta, 0.5f) * std::pow(cpm, 2) / 10.f));
}

//! https://pokemongo.gamepress.gg/pokemon-stats-advanced
//! @param base Pokémon's base stats.
//! @param level [1,maxLevel], 0.5 steps, maxLevel 40 for now.
//! @param atk [0, 15], integer.
//! @param def [0, 15], integer.
//! @param sta [0, 15], integer.
//! @note The game doesn't show CP under 10 but this function returns the actual CP even for values below 10.
//! @return < 0 on invalid input, > 0 otherwise
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

//! https://www.reddit.com/r/TheSilphRoad/comments/6wrw6a/raid_boss_cp_explained_if_it_hasnt_been_already/
//! @note For some reasons raid bosses have have different arbitrary formula without CPM.
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

//! returns NAN if unknown criteria given
float PropertyValueByName(const PoGoCmp::PokemonSpecie& pkm, const std::string& prop)
{
    if (prop.empty() || prop == "number") { return pkm.number; }
    else if (prop ==  "atk" || prop == "attack") { return pkm.baseAtk; }
    else if (prop ==  "def" || prop == "defense") { return pkm.baseDef; }
    else if (prop ==  "sta" || prop ==  "hp" || prop == "stamina") { return pkm.baseSta; }
    else if (prop == "bulk") { return float(pkm.baseSta * pkm.baseDef); }
    else if (prop == "total") { return float(pkm.baseAtk + pkm.baseDef + pkm.baseSta); }
    else if (prop == "cp") { return float(MaxCp(pkm)); }
    else if (prop == "gender") { return MathUtils::IsZero(pkm.malePercent) && MathUtils::IsZero(pkm.femalePercent) ? INFINITY : pkm.malePercent; }
    else if (prop == "buddy") { return pkm.buddyDistance; }
    else { return NAN; }
}

// std::to_string(float) uses fixed precision instead of the %g type of specifier so using this to get nicer output
std::string FloatToString(float f) { std::stringstream ss; ss << f; return ss.str(); }

Utf8::String FormatGender(float malePercent, float femalePercent)
{
    std::wstring genderText = L"○";
    if (!MathUtils::IsZero(malePercent))
    {
        genderText = Utf8::ToWString(FloatToString(malePercent)) + L" % ♂";
    }
    if (!MathUtils::IsZero(femalePercent))
    {
        if (MathUtils::IsZero(malePercent))
            genderText = Utf8::ToWString(FloatToString(femalePercent)) + L" % ♀";
        else
            genderText += L" " + Utf8::ToWString(FloatToString(femalePercent)) + L" % ♀";
    }
    return Utf8::FromWString(genderText);
}

//! @param useBaseName For Pokémon with multiple forms with same stats, show only the base name (e.g. "Unown" instead of "Unown A").
Utf8::String PokemonToString(
    const PoGoCmp::PokemonSpecie& base,
    const PoGoCmp::Pokemon& pkm,
    Utf8::String fmt,
    const std::string& sortCriteria,
    bool useBaseName)
{
    using namespace StringUtils;
    const auto type = SnakeCaseToTitleCase(PoGoCmp::PokemonTypeToString(base.type));
    const auto type2 = base.type2 == PoGoCmp::PokemonType::NONE
        ? "" : SnakeCaseToTitleCase(PoGoCmp::PokemonTypeToString(base.type2));
    const auto types = Concat(type, (!type2.empty() ? "/" : ""), type2);
    fmt = std::regex_replace(fmt, std::regex{"%nu"}, std::to_string(base.number));
    auto id = useBaseName ? PoGoCmp::FormIdToBaseId(base.id) : base.id;
    fmt = std::regex_replace(fmt, std::regex{"%na"}, PoGoCmp::PokemonIdToName(id));
    //! @todo Raid bosses might have slightly different stat computation.
    //! When comparing my stats e.g. to Pokebattler's stats, my attack and defence are off by one.
    fmt = std::regex_replace(fmt, std::regex{"%ba"}, std::to_string(base.baseAtk));
    fmt = std::regex_replace(fmt, std::regex{"%a"}, std::to_string(ComputeStat(base.baseAtk, pkm.atk, pkm.level)));
    fmt = std::regex_replace(fmt, std::regex{"%bd"}, std::to_string(base.baseDef));
    fmt = std::regex_replace(fmt, std::regex{"%d"}, std::to_string(ComputeStat(base.baseDef, pkm.def, pkm.level)));
    fmt = std::regex_replace(fmt, std::regex{"%bs"}, std::to_string(base.baseSta));
    const bool isRaidBoss = pkm.sta > 15;
    const auto sta = isRaidBoss ? pkm.sta : ComputeStat(base.baseSta, pkm.sta, pkm.level);
    fmt = std::regex_replace(fmt, std::regex{"%s"}, std::to_string(sta));
    fmt = std::regex_replace(fmt, std::regex{"%Tt"}, types);
    fmt = std::regex_replace(fmt, std::regex{"%T"}, type);
    fmt = std::regex_replace(fmt, std::regex{"%t"}, type2);
    fmt = std::regex_replace(fmt, std::regex{"%o"}, FloatToString(PropertyValueByName(base, sortCriteria)));
    const auto cp = isRaidBoss ? ComputeRaidBossCp(base, pkm) : ComputeCp(base, pkm);
    fmt = std::regex_replace(fmt, std::regex{"%cp"}, std::to_string(cp));

    auto moves = base.fastMoves;
    std::transform(moves.begin(), moves.end(), moves.begin(), [](auto m) { SnakeCaseToTitleCase(m); return m; });
    fmt = std::regex_replace(fmt, std::regex{"%fm"}, "[" + StringUtils::Join(moves, ", ") + "]");

    moves = base.chargeMoves;
    std::transform(moves.begin(), moves.end(), moves.begin(), [](auto m) { SnakeCaseToTitleCase(m); return m; });
    fmt = std::regex_replace(fmt, std::regex{"%cm"}, "[" + StringUtils::Join(moves, ", ") + "]");

    fmt = std::regex_replace(fmt, std::regex{"%b"}, std::to_string(base.buddyDistance));
    //fmt = std::regex_replace(fmt, std::regex("%%"), "%");
    fmt = std::regex_replace(fmt, std::regex{"%g"}, FormatGender(base.malePercent, base.femalePercent));

    fmt = std::regex_replace(fmt, std::regex{"\\\\n"}, "\n");
    fmt = std::regex_replace(fmt, std::regex{"\\\\t"}, "\t");
    return fmt;
}

const std::vector<ProgramOption> programsOptions{
    { "-h", "--help", L"Print help." },
    { "-v", "--version", L"Print version information and exit."},
    //{ "", "--verbose", L"Verbose log prints."},
    //{ "-a", "--ascending", L"Sort the results in ascending order (default)." },
    { "-d", "--descending", L"Sort the results in descending order (ascending by default)." },
    { "-i", "--include", L"Specify Pokémon or range of Pokémon to be included: "
        "'all' (default), 'gen<X>' (1/2/3/4), 'unknown', '<X>[,Y]' (inclusive Pokedex range, both numbers and names supported. "
        "For Pokémon's name, either the base name (e.g. 'Rattata') of form name (e.g. 'Rattata Alola') can be used. In case of "
        "a base name, all forms of the Pokémon are included in the results. In case of a form name, only the specific form is "
        "included in the results. A form name can be used only when specifying a single-Pokémon range. Multiple options supported."},
    { "-it", "--includeType", L"Specify Pokémon to be included by type(s): normal, fighting, flying, poison, ground, "
        "rock, bug, ghost, steel, fire, water, grass, electric, psychic, ice, dragon, dark, or fairy. Multiple options supported."},
    { "-r", "--results", L"Show only first N entries of the results, e.g. '-r 5' (negative number means 'show all')." },
    { "-f", "--format",
        L"Specify format for the output,'" + Utf8::ToWString(defaultFormat) + L"' by default: "
        "%nu (number), %na (name), %ba (base attack), %bd (base defense), %bs (base stamina), "
        "%a, %d, %s (effective stats using the specified --level and --ivs), %T (primary type), %t (secondary type) "
        "%Tt (both types, 2nd type only if applicable), %o (sorting criteria), %cp (max. CP), %fm (fast moves), %cm (charge moves) "
        "%b (buddy distance, km), %g gender ratio by male percentage) \\n (new line), \\t (tab)"
        "Max. level and perfect IVs by default. See also --ivs and --level."
    },
    {"", "--ivs",
        L"Specify IVs of the Pokémon for the CP calculation: <attack><defense><stamina>, hexadecimal 0-F. "
        L"E.g. 'A9F', perfect IVs 'FFF' by default."},
    {"", "--level",
        L"Specify level of the Pokémon for the CP calculation, [1," + std::to_wstring(PoGoCmp::PlayerLevel.cpMultiplier.size()) + L"]"},
    {"", "--raidLevel",
        L"Specify raid level for the Pokémon's CP calculation to simulate if the Pokémon would be a raid boss. "
        L"Overrides possible --level and --ivs options."},
    { "", "--rarity",
        L"Show only Pokémon with matching rarity type (normal/legendary/mythic). "
        "By default all rarities are included."},
    { "", "--showDuplicateForms",
        L"Show duplicate Pokémon forms, i.e. forms that differ only by name and type. "
        "By default, duplicates (same base stats) are not shown."},
    //! @todo --effective - show the effective IVs, (iv + base stat) * CPM
    // Commands
    { "sort", "",
        L"Sort the Pokémon by certain criteria: "
        "'number' (default), base 'attack'/'atk', base 'defense'/'def', base 'stamina'/'sta'/'hp', bulk (def*sta), "
        "'total' (atk+def+sta), 'gender', or 'buddy' (buddy distance). Comparison operator (<, <=, =, >, or >=) and value "
        "can be appended to the criteria, e.g. \"(atk>=200\" (make sure to use double quotes) will only include Pokémon "
        "with base attack larger than or equal to 200 to the results. A name of Pokémon can also be used as the value: "
        "in this case the property of the specified Pokémon is used as the reference point."
    },
    { "powerup", "",
        L"Calculate resources required to power up a Pokémon from certain level to another, e.g. "
        "'powerup 15,31.5'"
    },
    { "perfect-ivs", "",
        L"Generate a search string that can be used to filter perfect wild Pokémon catches. "
        L"Only single name/ID/number as an argument supported."
    },
    {
        "info", "",
        L"Print information about the available data set. Supported arguments: '' (print general info), "
        "'moves' (list available moves)."},
    { "typeinfo", "",
        L"Print typing information for the given type(s) or Pokémon."
        "'typeinfo atk,<type1>[,type2]' prints attack(-combination)'s effectiveness against all types. "
        "'typeinfo def,<type1>[,type2]' prints defending type(-combination)'s information against all types. "
        "'typeinfo <pokemon> prints a single Pokémon's type information."
    }
};

//! @todo indentation
void PrintHelp()
{
    std::stringstream ss;
    ss << "Supported commands:\n";
    for (const auto& opt : programsOptions)
        if (opt.type == ProgramOption::Cmd)
            ss << "  " << opt << "\n";
    ss << "Supported arguments:\n";
    for (const auto& opt : programsOptions)
        if (opt.type == ProgramOption::Arg)
            ss << "  " << opt << "\n";
    Utf8::PrintLine(ss.str());
}

template<typename Target, typename Source>
Target LexicalCast(Source arg)
{
    std::stringstream ss;
    Target result;
    if (!(ss << arg) || !(ss >> result) || !(ss >> std::ws).eof())
    {
        throw std::runtime_error("Bad lexical cast.");
    }

    return result;
}

template <typename T>
T ParseValue(const std::string& str, T minVal, T maxVal)
{
    T val = LexicalCast<T>(str);
    if (val < minVal || val > maxVal)
        throw std::runtime_error("Value out of range.");
    return val;
}

using FloatComparator = std::function<bool(float, float)>;
FloatComparator MakeComparator(const std::string& comp)
{
    if (comp == "<") return FloatComparator([](float a, float b) { return a < b; });
    if (comp == "<=") return FloatComparator([](float a, float b) { return a <= b; });
    if (comp == ">") return FloatComparator([](float a, float b) { return a > b; });
    if (comp == ">=") return FloatComparator([](float a, float b) { return a >= b; });
    if (comp == "=") return FloatComparator([](float a, float b) { return MathUtils::Equals(a, b); });
    //! \todo throw or some else error mechanism
    return FloatComparator([](float /*a*/, float /*b*/) { return true; });
}

bool IsFormName(const std::string& id)
{
    return std::find_if(
        PoGoCmp::FormNames.begin(), PoGoCmp::FormNames.end(),
        [&id](const auto& kvp) { return Utf8::CompareI(kvp.second.c_str(), id.c_str()) == 0; }
    ) != PoGoCmp::FormNames.end();
}

int main(int argc, char **argv)
{
    ProgamOptionMap opts{Utf8::ParseArguments(argc, argv)};
    if (opts.args.empty())
    {
        LogE("Invalid usage.");
        PrintHelp();
        return EXIT_FAILURE;
    }

    //const bool verbose = opts.HasOption("--verbose");

    std::vector<std::string> knownArgs;
    for (const auto& opt : programsOptions)
    {
        knownArgs.push_back(opt.shortName);
        knownArgs.push_back(opt.longName);
    }

    if (opts.HasOption("-v", "--version"))
    {
        Log("PoGoCmpCli " + Utf8::String(PoGoCmpVersionString()));
        return EXIT_SUCCESS;
    }

    using namespace StringUtils;
    using namespace PoGoCmp;

    if (opts.HasOption("-h", "--help"))
    {
        PrintHelp();
        return EXIT_SUCCESS;
    }

    int ret = EXIT_FAILURE;

    //! @todo Use info also to list type-effectiveness, attacks, etc.
    if (opts.HasOption("info"))
    {
        if (auto info = opts.OptionValues("info"); std::find(info.begin(), info.end(), "moves") != info.end())
        {
            Log(std::to_string(PoGoCmp::Moves.size()) + " moves available:");
            auto isMoveInUse = [](const std::string& id)
            {
                return std::any_of(
                    PoGoCmp::PokemonByNumber.begin(), PoGoCmp::PokemonByNumber.end(),
                    [&id](const auto& kvp)
                    {
                        return
                            std::any_of(kvp.second.fastMoves.begin(), kvp.second.fastMoves.end(),
                                        [&id](const auto& fmId) { return fmId == id; }) ||
                            std::any_of(kvp.second.chargeMoves.begin(), kvp.second.chargeMoves.end(),
                                        [&id](const auto& cmId) { return cmId == id; });
                    }
                );
            };

            for (const auto& m : PoGoCmp::Moves)
            {
                std::cout << m.id;
                if (!isMoveInUse(m.id)) Utf8::Print("*");
                Utf8::PrintLine("");
            }
            Log(L"*) Move currently available in any Pokémon's moveset.");

            ret = EXIT_SUCCESS;
        }
        else if (info.empty())
        {
            //! @todo
            //Log("Available Pokedex range: " + std::to_string(MaxRange.first) + "-" + std::to_string(MaxRange.second));
            Log("Number of Trainer/Pokemon levels: " + std::to_string(PoGoCmp::PlayerLevel.cpMultiplier.size()));

            ret = EXIT_SUCCESS;
        }
        else
        {
            LogErrorAndExit("Unknown argument for 'info' command.");
        }
    }
    else if (opts.HasOption("typeinfo"))
    {
        auto typeinfo = opts.OptionValue("typeinfo");
        if (typeinfo.empty())
            LogErrorAndExit("Arguments missing for typeinfo.");

        const auto allTypes = []
        {
            std::vector<std::pair<PoGoCmp::PokemonType, PoGoCmp::PokemonType>> types;
            for (int i = 0; i < (int)PoGoCmp::PokemonType::NUM_TYPES; ++i)
                types.emplace_back((PoGoCmp::PokemonType)i, PoGoCmp::PokemonType::NONE);
            return types;
        };

        // types as indices to TypeEffectiveness table
        std::vector<std::pair<PoGoCmp::PokemonType, PoGoCmp::PokemonType>> attackTypes, defenderTypes;

        const auto types = Split(typeinfo, ',', StringUtils::RemoveEmptyEntries);
        const auto cmd = types[0];
        if (cmd == "atk" || cmd == "def") // typeinfo <atk|def>,<type1>[,type2]
        {
            if (types.size() < 2)
                LogErrorAndExit("At least one type required as an argument.");

            PoGoCmp::PokemonType type1{StringToPokemonType(types[1].c_str())};
            if (type1 == PoGoCmp::PokemonType::NONE)
                LogErrorAndExit(types[1] + " is not a valid type.");

            PoGoCmp::PokemonType type2{PoGoCmp::PokemonType::NONE};
            if (types.size() > 2)
            {
                type2 = StringToPokemonType(types[2].c_str());
                if (type2 == PoGoCmp::PokemonType::NONE)
                    LogErrorAndExit(types[2] + " is not a valid type.");
            }

            if (cmd == "def")
            {
                attackTypes = allTypes();
                defenderTypes.emplace_back(type1, type2);
            }
            else
            {
                attackTypes.emplace_back(type1, type2);
                defenderTypes = allTypes();
            }
        }
        else // typeinfo <pokemon>
        {
            const auto id = PoGoCmp::PokemonNameToId(StringUtils::TrimCopy(cmd));
            auto pkm = PoGoCmp::PokemonByIdName(id);
            if (pkm.number == 0)
                LogErrorAndExit("'" + types[0] + "'"+ Utf8::FromWString(L" is not a valid argument nor a Pokémon."));

            attackTypes = allTypes();

            defenderTypes.emplace_back(pkm.type, pkm.type2);
        }

        assert(!attackTypes.empty());
        assert(!defenderTypes.empty());

        const auto computeEffectiveness = [](PoGoCmp::PokemonType at, PoGoCmp::PokemonType dt1, PoGoCmp::PokemonType dt2)
        {
            auto scalar = PoGoCmp::TypeEffectiveness[(int)at][(int)dt1];
            if (dt2 != PoGoCmp::PokemonType::NONE)
                scalar *= PoGoCmp::TypeEffectiveness[(int)at][(int)dt2];
            return scalar;
        };


        struct TypeEffectiveness
        {
            PoGoCmp::PokemonType at1, at2;
            PoGoCmp::PokemonType dt1, dt2;
            float scalar;
        };

        std::vector<TypeEffectiveness> results;
        for (const auto& at : attackTypes)
        {
            for (const auto& dt : defenderTypes)
            {
                auto scalar = computeEffectiveness(at.first, dt.first, dt.second);
                if (at.second != PoGoCmp::PokemonType::NONE)
                {
                    scalar = std::max(scalar, computeEffectiveness(at.second, dt.first, dt.second));
                }

                results.push_back({at.first, at.second, dt.first, dt.second, scalar});
            }
        }

        // sort the results, NVE first, neutral second, SE last
        std::sort(
            results.begin(), results.end(),
            [](const auto& a, const auto& b) { return a.scalar < b.scalar; }
        );

        for (const auto& te: results)
        {
            std::cout << SnakeCaseToTitleCase(PokemonTypeToString(te.at1));
            if (te.at2 != PoGoCmp::PokemonType::NONE)
                std::cout << "/" << SnakeCaseToTitleCase(PokemonTypeToString(te.at2));
            std::cout << " vs. " << SnakeCaseToTitleCase(PokemonTypeToString(te.dt1));
            if (te.dt2 != PoGoCmp::PokemonType::NONE)
                std::cout << "/" << SnakeCaseToTitleCase(PokemonTypeToString(te.dt2));
            std::cout << ": " << FloatToString(te.scalar) << "\n";
        }

        ret = EXIT_SUCCESS;
    }
    else if (opts.HasOption("sort"))
    {
        // E.g. "16,32", or "bulbasaur,ivysaur", or "250". The names can contain following
        // Unicode characters: ♀ (\u2640), ♂ (\u2642), é (\u00e9), É (\u00c9).
        //! @todo Test for this
        const std::wregex rangePattern{
            LR"(([\w-.' \u2640\u2642\u00e9\u00c9]+)(,)?([\w-.' \u2640\u2642\u00e9\u00c9]+)?)"
        };
        std::wsmatch rangeMatches;

        struct RangeWithForm
        {
            RangeWithForm() = default;
            RangeWithForm(const PokedexRange& range) :
                first{range.first},
                second{range.second}
            {
            }

            PokedexNumber first{}, second{};
            std::string formId; // used if single form requested
        };

        std::vector<RangeWithForm> ranges;
        auto includes = opts.OptionValues("-i", "--include");
        if (includes.empty())
            includes.push_back("all");

        for (const auto& include : includes)
        {
            // If first == second, only a single Pokémon is wanted.
            RangeWithForm range;
            const auto winclude = Utf8::ToWString(include);
            if (winclude == L"all") { range = MaxRange; }
            else if (winclude == L"gen1") { range = Gen1Range; }
            else if (winclude == L"gen2") { range = Gen2Range; }
            else if (winclude == L"gen3") { range = Gen3Range; }
            else if (winclude == L"gen4") { range = Gen4Range; }
            else if (winclude == L"unknown") { range = UnknownRange; }
            else if (std::regex_match(winclude, rangeMatches, rangePattern))
            {
                try
                {
                    auto rangeFirst = Utf8::FromWString(rangeMatches[1].str());
                    //! @todo trim in the regex instead of using trim() functions
                    Trim(rangeFirst);
                    //auto type = rangeMatches[2].str();
                    auto rangeSecond = rangeMatches.size() > 2 ? Utf8::FromWString(rangeMatches[3].str()) : "";
                    Trim(rangeSecond);

                    if (IsNumber(rangeFirst))
                    {
                        range.first = (PokedexNumber)std::stoul(rangeFirst);
                    }
                    else
                    {
                        const auto id = PoGoCmp::PokemonNameToId(rangeFirst);
                        range.first = PoGoCmp::PokemonByIdName(id).number;
                        if (range.first == 0)
                            LogErrorAndExit("'" + rangeFirst + "' is not a valid name/ID.");
                        if (IsFormName(id))
                            range.formId = id;
                    }

                    if (rangeSecond.empty())
                    {
                        range.second = range.first;
                    }
                    else if (IsNumber(rangeFirst))
                    {
                        range.second = (PokedexNumber)std::stoul(rangeSecond);
                    }
                    else
                    {
                        const auto id = PoGoCmp::PokemonNameToId(rangeSecond);
                        range.second = PoGoCmp::PokemonByIdName(id).number;
                        if (range.second == 0)
                            LogErrorAndExit("'" + rangeSecond + "' is not a valid name/ID.");
                    }

                    if (range.first != range.second && !range.formId.empty())
                    {
                        LogErrorAndExit(L"A form name can be used only when specifying a single-Pokémon range.");
                    }

                    if (range.first < 1)
                    {
                        LogErrorAndExit("Range's index min. (" + std::to_string(range.first) +
                            ") cannot be less than 1");
                    }
                    if (range.first > range.second)
                    {
                        LogErrorAndExit("Range's index min. (" + std::to_string(range.first) +
                            ") cannot be greater than max. (" + std::to_string(range.second) + ")");
                    }

                    auto isWithinRange = [](const auto& r, const auto& value)
                    {
                        return value >= r.first && value <= r.second;
                    };

                    if (std::none_of(ValidRanges.begin(), ValidRanges.end(),
                        [&](const auto& r) { return isWithinRange(r, range.first); }))
                    {
                        LogErrorAndExit("Range's min. " + std::to_string(range.first) +
                            " not within valid ranges.");
                    }
                    if (std::none_of(ValidRanges.begin(), ValidRanges.end(),
                        [&](const auto& r) { return isWithinRange(r, range.second); }))
                    {
                        LogErrorAndExit("Range's max. " + std::to_string(range.second) +
                            " not within valid ranges.");
                    }
                }
                catch (const std::exception& e)
                {
                    LogErrorAndExit(Concat(
                        "Failed to parse include's value, '", include, "': ", e.what()));
                }
            }
            else
            {
                LogErrorAndExit(Concat("Invalid value for 'include', '", include, "'"));
            }

            ranges.push_back(range);
        }

        assert(!ranges.empty());

        int numResults = (int)PoGoCmp::PokemonByNumber.size();
        auto resultsVal = opts.OptionValue("-r", "--results");
        if (!resultsVal.empty())
        {
            try
            {
                numResults = std::min(ParseValue(resultsVal, INT_MIN, numResults), numResults);
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Failed to parse -r/--results value, '", resultsVal, "': ", e.what()));
            }
        }

        std::string format = defaultFormat;
        if (opts.HasOption("-f", "--format"))
        {
            format = opts.OptionValue("-f", "--format");
            if (format.empty())
            {
                LogErrorAndExit("Value missing for -f/--format\n");
            }
        }

        Pokemon pokemon{};
        pokemon.level = 40.f;
        pokemon.atk = 15;
        pokemon.def = 15;
        pokemon.sta = 15;

        if (auto level = opts.OptionValue("--level"); !level.empty())
        {
            try
            {
                pokemon.level = ParseValue(level, 1.f, (float)PoGoCmp::PlayerLevel.cpMultiplier.size());
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Not a valid level '", level, "': ", e.what()));
            }
        }

        if (auto ivs = opts.OptionValue("--ivs"); !ivs.empty())
        {
            if (std::count_if(ivs.begin(), ivs.end(),
                [](auto c) { return std::isxdigit(c, std::locale::classic()); }) != 3)
            {
                LogErrorAndExit("Value must consist of exactly three hexadecimal digits.");
            }

            try
            {
                auto values = std::stoul(ivs, nullptr, 16);
                pokemon.atk = (values & 0xF00) >> 8;
                pokemon.def = (values & 0x0F0) >> 4;
                pokemon.sta = (values & 0x00F);
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Failed to parse --ivs value '", ivs, "': ", e.what()));
            }
        }

        if (auto level = opts.OptionValue("--raidLevel"); !level.empty())
        {
            if (opts.HasOption("--level"))
                Log("--level value is ignored due to --raidLevel.");
            if (opts.HasOption("--ivs"))
                Log("--ivs value is ignored due to --raidLevel.");

            try
            {
                auto raidLevel = ParseValue(level, 1, (int)RaidLevels.size());
                pokemon = RaidLevels[raidLevel - 1];
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Not a valid level '", level, "': ", e.what()));
            }
        }

        const bool ascending = !opts.HasOption("-d", "--descending");
        const auto sortCmp = MakeComparator(ascending ? "<" : ">");
        std::string sortCriteria = opts.OptionValue("sort");
        std::string compOpType, compValStr;
        float compVal;
        if (auto opBegin = sortCriteria.find_first_of("<>="), opEnd = sortCriteria.find_last_of("<>=");
            opBegin != std::string::npos && opEnd != std::string::npos)
        {
            compOpType = sortCriteria.substr(opBegin, opEnd - opBegin + 1);
            compValStr = sortCriteria.substr(opEnd + 1, sortCriteria.length() - opEnd);
            sortCriteria = sortCriteria.substr(0, opBegin);
            try
            {
                compVal = IsNumber(compValStr)
                    ? std::stof(compValStr)
                    : PropertyValueByName(PoGoCmp::PokemonByIdName(PoGoCmp::PokemonNameToId(compValStr)), sortCriteria);
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Not a valid number or name '", compValStr, "': ", e.what()));
            }
        }

        if (std::isnan(PropertyValueByName({}, sortCriteria)))
            LogErrorAndExit("Invalid sorting criteria: '" + sortCriteria + "'.");

        std::vector<PoGoCmp::PokemonSpecie> results;
        for (const auto& range : ranges)
        {
            auto begin = range.formId.empty()
                ? PokemonByNumber.lower_bound(range.first)
                : std::find_if(
                    PokemonByNumber.begin(), PokemonByNumber.end(),
                    [&range](const auto& kvp) { return kvp.first == range.first && kvp.second.id == range.formId; }
                );

            auto end = PokemonByNumber.upper_bound(range.second);

            std::vector<PoGoCmp::PokemonSpecie> rangeResult;
            rangeResult.reserve(std::distance(begin, end) + 1);

            if (!range.formId.empty())
            {
                assert(range.first == range.second);
                rangeResult.push_back(begin->second);
                begin = PokemonByNumber.upper_bound(range.second);
            }

            for (; begin != end; ++begin)
            {
                rangeResult.push_back(begin->second);
            }

            const auto filterCmp = MakeComparator(compOpType);
            std::copy_if(
                rangeResult.begin(), rangeResult.end(), std::back_inserter(results),
                [&](const auto &pkm) { return filterCmp(PropertyValueByName(pkm, sortCriteria), compVal); }
            );
        }

        // remove duplicate and overlapping results
        std::sort(
            results.begin(), results.end(),
            [](const auto& a, const auto& b)
            {
                if (a.number < b.number) return true;
                if (a.number > b.number) return false;

                if (Utf8::CompareI(a.id.c_str(), b.id.c_str()) < 0) return true;

                return false;
            }
        );

        results.erase(
            std::unique(
                results.begin(), results.end(),
                [](const auto& a, const auto& b) { return a.number == b.number && a.id == b.id; }
            ),
            results.end()
        );

        // --showDuplicateForms

        const auto formsHaveSameStats = [](const auto& a, const auto& b)
        {
            return a.number == b.number &&
                a.baseAtk == b.baseAtk && a.baseDef == b.baseDef && a.baseSta == b.baseSta;
        };

        const auto showDuplicateForms = opts.HasOption("", "--showDuplicateForms");
        if (!showDuplicateForms)
        {
            results.erase(
                std::unique(results.begin(), results.end(), formsHaveSameStats),
                results.end()
            );
        }

        // --rarity

        std::vector<PoGoCmp::PokemonRarity> rarities{
            PoGoCmp::PokemonRarity::NORMAL,
            PoGoCmp::PokemonRarity::LEGENDARY,
            PoGoCmp::PokemonRarity::MYTHIC
        };
        if (auto rarityStrings = opts.OptionValues("--rarity"); !rarityStrings.empty())
        {
            rarities.clear();
            for (const auto& rarityStr : rarityStrings)
            {
                auto rarity = PoGoCmp::StringToPokemonRarity(rarityStr.c_str());
                if (rarity == PoGoCmp::PokemonRarity::NONE)
                    LogErrorAndExit("Unknown rarity '" + rarityStr + "'");
                rarities.push_back(rarity);
            }
        }

        results.erase(
            std::remove_if(results.begin(), results.end(), [&rarities](const auto& pkm)
            {
                return std::find(rarities.begin(), rarities.end(), pkm.rarity) == rarities.end();
            }),
            results.end()
        );

        // --include-types
        std::vector<PoGoCmp::PokemonType> types;
        auto includeTypes = opts.OptionValues("-it", "--includeType");
        for (const auto& typeStr : includeTypes)
        {
            auto type = PoGoCmp::StringToPokemonType(typeStr.c_str());
            if (type == PoGoCmp::PokemonType::NONE)
                LogErrorAndExit("Unknown type '" + typeStr + "'");
            types.push_back(type);
        }

        if (!types.empty())
        {
            results.erase(
                std::remove_if(
                    results.begin(),
                    results.end(), [&types](const auto& pkm)
                    {
                        return std::find_if(types.begin(), types.end(),
                            [&pkm](auto type) { return pkm.type == type || pkm.type2 == type; }
                        ) == types.end();
                    }
                ),
                results.end()
            );
        }

        // Finally sort according the sorting criteria...
        std::sort(
            results.begin(), results.end(),
            [&sortCmp, &sortCriteria](const auto& lhs, const auto& rhs)
            {
                return sortCmp(PropertyValueByName(lhs, sortCriteria), PropertyValueByName(rhs, sortCriteria));
            }
        );

        // ...and show the results.
        const auto numMatches = (int)results.size();
        Utf8::Print(std::to_string(numMatches) + " matches (showing ");
        // negative number means 'show all'
        numResults = numResults < 0 ? numMatches : std::min(numResults, numMatches);
        Utf8::PrintLine(std::to_string(numResults) + (ascending ? " last" : " first") + " results):");

        auto formNameSpecified = [&includes](const auto& id) -> bool
        {
            return std::find_if(
                includes.begin(), includes.end(),
                [&id](const auto& include) { return PokemonNameToId(include) == id;}
            ) != includes.end();
        };

        for (int i = 0; i < numMatches && i < numResults; ++i)
        {
            const auto& base = results[i];
            /* if (verbose) Log("Pokédex range " + std::to_string(result.first.first) + "-" +
                std::to_string(result.first.second) + ":"); */
            // if (verbose) Utf8::Print(std::to_string(i+1) + ": ");

            const auto numSameStatsForms = std::unique(results.begin(), results.end(), formsHaveSameStats) - results.begin();
            const bool useBaseName = !formNameSpecified(base.id) &&
                !(numSameStatsForms > 1) && !showDuplicateForms;

            Utf8::Print(PokemonToString(base, pokemon, format, sortCriteria, useBaseName));
        }

        ret = EXIT_SUCCESS;
    }
    else if (auto powerup = opts.OptionValue("powerup"); !powerup.empty())
    {
        auto powerupRange = Split(powerup, ',', StringUtils::RemoveEmptyEntries);
        if (powerupRange.size() != 2)
            LogErrorAndExit("Power-up range must consist of two comma-separated numbers.");

        try
        {
            const auto max = (float)PoGoCmp::PokemonUpgrades.candyCost.size();
            auto end = ParseValue(powerupRange[1], 1.f, max);
            auto begin = ParseValue(powerupRange[0], 1.f, end);
            const auto step = 1.f / PoGoCmp::PokemonUpgrades.upgradesPerLevel;
            float dummy;
            const auto beginFract = std::modf(begin, &dummy);
            if (!MathUtils::Equals(beginFract, 0.f) && !MathUtils::Equals(beginFract, step))
                LogErrorAndExit("Invalid factorial for range's begin.");
            const auto endFract = std::modf(end, &dummy);
            if (!MathUtils::Equals(endFract, 0.f) && !MathUtils::Equals(endFract, step))
                LogErrorAndExit("Invalid factorial for range's end.");

            int candyTotal{}, dustTotal{};
            for (float i = begin - 1; i < end - 1; i += step)
            {
                auto idx = (size_t)std::floor(i);
                candyTotal += PoGoCmp::PokemonUpgrades.candyCost[idx];
                dustTotal += PoGoCmp::PokemonUpgrades.stardustCost[idx];
            }

            Log("Power-up costs from level " + powerupRange[0] + " to " + powerupRange[1] + ":");
            Log(" candy: " + std::to_string(candyTotal));
            Log(" stardust: " + std::to_string(dustTotal));
        }
        catch(const std::exception& e)
        {
            LogErrorAndExit(Concat("Not a valid range: ", e.what()));
        }
        ret = EXIT_SUCCESS;
    }
    else if (auto val = opts.OptionValue("perfect-ivs"); !val.empty())
    {
        try
        {
            auto number = IsNumber(val)
                ? (PokedexNumber)std::stoul(val)
                : PoGoCmp::PokemonByIdName(PoGoCmp::PokemonNameToId(val)).number;
            //! @todo form support
            auto pkm = PoGoCmp::PokemonByNumber.equal_range(number).first->second;

            // "<number>&cp<cpAtLevel1>,cp<cpAtLevel2>,..."
            std::stringstream ss;
            ss << number << "&";
            const auto maxWildLevel = PoGoCmp::PlayerLevel.maxEncounterPlayerLevel
                + PoGoCmp::WeatherBonus.cpBaseLevelBonus;
            for (float level = 1; level <= maxWildLevel; ++level)
                ss << "cp" << ComputeCp(pkm, { level, 15, 15, 15 }) << ",";
            auto searchString = ss.str();
            searchString.erase(searchString.end() - 1);
            Log(searchString);
        }
        catch(const std::exception& e)
        {
            LogErrorAndExit(Concat("Not a valid name or number: ", e.what()));
        }
        ret = EXIT_SUCCESS;
    }

    //! @todo move this to ProgramOptionMap
    for (auto it = opts.args.begin(); it != opts.args.end(); ++it)
    {
        if (std::count(knownArgs.begin(), knownArgs.end(), *it) == 0 && !opts.IsValue(it))
        {
            LogE("Unknown parameter '" + *it  + "'");
        }
    }

    if (ret != EXIT_SUCCESS)
    {
        LogE("No command given.");
    }

    return ret;
}
