﻿#include "../Lib/PoGoCmp.h"
#include "../Lib/PoGoDb.h"
#include "ProgramOptions.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <regex>
#include <functional>
#include <numeric>
#include <cmath>

const Utf8::String defaultFormat{ "%nu %na ATK %a DEF %d STA %s TYPE %Tt CP %cp\\n" };
//! Pokedex number range, number - 1 for the index in PoGoCmp::PokemonByNumber.
using PokedexRange = std::pair<size_t, size_t>;
//! @todo generate values for these in PoGoCmpDb.h
const PokedexRange gen1Range{ 1, 151 };
const PokedexRange gen2Range{ 152, 251 };
const PokedexRange gen3Range{ 252, 386 };
const PokedexRange maxRange{ 1, PoGoCmp::PokemonByNumber.size() };

void LogE(const Utf8::String& msg)
{
    Utf8::PrintLine("Error: " + msg, Utf8::OutputStream::Err);
}

void LogErrorAndExit(const Utf8::String& msg)
{
    LogE(msg);
    std::exit(EXIT_FAILURE);
}

void Log(const Utf8::String& msg)
{
    Utf8::PrintLine(msg, Utf8::OutputStream::Out);
}

//! @todo Move CP functions to the PoGoCmp API

//! https://pokemongo.gamepress.gg/pokemon-stats-advanced
//! @param level [1,maxLevel], 0.5 steps, maxLevel 40 for now.
//! @param atk [0, 15], integer.
//! @param def [0, 15], integer.
//! @param sta [0, 15], integer.
//! @param pkm Pokémon's base stats.
//! @note The game doesn't show CP under 10 but this function returns the actual CP even for values below 10.
//! @return < 0 on invalid input, > 0 otherwise
int ComputeCp(float level, float atk, float def, float sta, const PoGoCmp::PokemonSpecie& pkm)
{
    const auto numLevels = (float)PoGoCmp::PlayerLevel.cpMultiplier.size();
    if (level  < 1 || level > numLevels) return -1;
    if (atk < 0 || atk > 15) return -1;
    if (def < 0 || def > 15) return -1;
    if (sta < 0 || sta > 15) return -1;

    float levelIdx;
    auto levelFract = std::modf(level, &levelIdx);
    if (levelFract != 0.f && levelFract != 0.5f) return -1;
    levelIdx -= 1;
    auto nextLevelIdx = std::min(levelIdx + 1, numLevels -1);
    auto cpmBase = PoGoCmp::PlayerLevel.cpMultiplier[(size_t)levelIdx];
    auto cpmNext = PoGoCmp::PlayerLevel.cpMultiplier[(size_t)nextLevelIdx];
    auto cpmStep = (std::pow(cpmNext, 2) - std::pow(cpmBase, 2)) / 2.f;
    auto cpm = levelFract != 0.f ? std::sqrt(std::pow(cpmBase, 2) + cpmStep) : cpmBase;
    atk = pkm.baseAtk + atk;
    def = pkm.baseDef + def;
    sta = pkm.baseSta + sta;
    return (int)std::floor(atk * std::pow(def, 0.5f) * std::pow(sta, 0.5f) * std::pow(cpm, 2) / 10.f);
}

int MinCp(const PoGoCmp::PokemonSpecie& pkm)
{
    return ComputeCp(1, 0, 0, 0, pkm);
}

int MaxCp(const PoGoCmp::PokemonSpecie& pkm)
{
    return ComputeCp((float)PoGoCmp::PlayerLevel.cpMultiplier.size(), 15, 15, 15, pkm);
}

//! returns negative number if unknown criteria given
int PropertyValueByName(const PoGoCmp::PokemonSpecie& pkm, const std::string& prop)
{
    if (prop.empty() || prop == "number") { return pkm.number; }
    else if (prop ==  "atk" || prop == "attack") { return pkm.baseAtk; }
    else if (prop ==  "def" || prop == "defense") { return pkm.baseDef; }
    else if (prop ==  "sta" || prop ==  "hp" || prop == "stamina") { return pkm.baseSta; }
    else if (prop == "bulk") { return pkm.baseSta * pkm.baseDef; }
    else if (prop == "total") { return pkm.baseAtk + pkm.baseDef + pkm.baseSta ; }
    else if (prop == "cp") { return MaxCp(pkm); }
    else { return -1; }
}

struct Pokemon// : PoGoCmp::PokemonSpecie
{
    //! Level [1,maxLevel], 0.5 steps.
    float level;
    //! Indivial value (IVs), each IV has value of [0,15].
    uint8_t atk;
    uint8_t def;
    uint8_t sta;
};

Utf8::String PokemonToString(const Pokemon& pkm, const PoGoCmp::PokemonSpecie& base, Utf8::String fmt, const std::string& sortCriteria)
{
    using namespace StringUtils;
    const auto type = SnakeCaseToTitleCase(PoGoCmp::PokemonTypeToString(base.type));
    const auto type2 = base.type2 == PoGoCmp::PokemonType::NONE
        ? "" : SnakeCaseToTitleCase(PoGoCmp::PokemonTypeToString(base.type2));
    const auto types = Concat(type, (!type2.empty() ? "/" : ""), type2);

    fmt = std::regex_replace(fmt, std::regex("%nu"), std::to_string(base.number));
    fmt = std::regex_replace(fmt, std::regex("%na"), SnakeCaseToTitleCase(PoGoCmp::PokemonIdToName(base.id)));
    fmt = std::regex_replace(fmt, std::regex("%a"), std::to_string(base.baseAtk));
    fmt = std::regex_replace(fmt, std::regex("%d"), std::to_string(base.baseDef));
    fmt = std::regex_replace(fmt, std::regex("%s"), std::to_string(base.baseSta));
    fmt = std::regex_replace(fmt, std::regex("%Tt"), types);
    fmt = std::regex_replace(fmt, std::regex("%T"), type);
    fmt = std::regex_replace(fmt, std::regex("%t"), type2);
    fmt = std::regex_replace(fmt, std::regex("%o"), std::to_string(PropertyValueByName(base, sortCriteria)));
    //fmt = std::regex_replace(fmt, std::regex("%%"), "%");
    fmt = std::regex_replace(fmt, std::regex("%cp"), std::to_string(ComputeCp(pkm.level, pkm.atk, pkm.def, pkm.sta, base)));
    fmt = std::regex_replace(fmt, std::regex("\\\\n"), "\n");
    fmt = std::regex_replace(fmt, std::regex("\\\\t"), "\t");
    return fmt;
}

const std::vector<ProgramOption> programsOptions{
    { "-h", "--help", L"Print help." },
    { "-v", "--version", L"Print version information and exit."},
    //{ "", "--verbose", L"Verbose log prints."},
    //{ "-a", "--ascending", L"Sort the results in ascending order (default)." },
    { "-d", "--descending", L"Sort the results in descending order (ascending by default)." },
    { "-i", "--include", L"Specify Pokémon or range of Pokémon to be included: "
        "'all' (default), 'gen<X>' (1/2/3), '<X>[,Y]' (inclusive Pokedex range, both numbers and names supported. "
        "Multiple options supported."},
    { "-r", "--results", L"Show only first N entries of the results, e.g. '-r 5' (negative number means 'show all')." },
    { "-f", "--format",
        L"Specify format for the output,'" + Utf8::ToWString(defaultFormat) + L"' by default: "
        "%nu (number), %na (name), %a (attack), %d (defense), %s (stamina), %T (primary type), %t (secondary type) "
        "%Tt (both types, 2nd type only if applicable), %o (sorting criteria), %cp (max.* CP), \\n (new line), \\t (tab)"
        "Max. level and perfect IVs by default. See also --ivs and --level."
    },
    {"", "--ivs",
        L"Specify IVs of the Pokémon for the CP calculation: <attack><defense><stamina>, hexadecimal 0-F. "
        L"E.g. 'A9F', perfect IVs 'FFF' by default."},
    {"", "--level",
        L"Specify level of the Pokémon for the CP calculation, [1," + std::to_wstring(PoGoCmp::PlayerLevel.cpMultiplier.size()) + L"]"},
    { "", "--rarity",
        L"Show only Pokémon with matching rarity type (normal/legendary/mythic). "
        "By default all rarities are included."},
    // Commands
    { "sort", "", L"Sort the Pokémon by certain criteria: "
        "number (default), attack/atk, defense/def, stamina/sta/hp', bulk (def*sta), or total(atk+def+sta)."
    },
    { "info", "", L"Print information about the available data set." }
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

int main(int argc, char **argv)
{
    ProgamOptionMap opts{ Utf8::ParseArguments(argc, argv) };
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

    //! @todo Use info also to list type-effectiveness, attacks, etc.
    if (opts.HasOption("info"))
    {
        Log("Available Pokedex range: " + std::to_string(maxRange.first) + "-" + std::to_string(maxRange.second));
        Log("Number of Trainer/Pokemon levels: " + std::to_string(PoGoCmp::PlayerLevel.cpMultiplier.size()));
        return EXIT_SUCCESS;
    }

    if (opts.HasOption("-h", "--help"))
    {
        PrintHelp();
        return EXIT_SUCCESS;
    }

    int ret = EXIT_FAILURE;
    if (opts.HasOption("sort"))
    {
        const bool ascending = !opts.HasOption("-d", "--descending");
        using Cmp = std::function<bool(int, int)>;
        const auto cmp = ascending ? Cmp(std::less<>()) : Cmp(std::greater<>());
        const std::string sortCriteria = opts.OptionValue("sort");
        auto sortFunction = [&cmp, &sortCriteria](const PoGoCmp::PokemonSpecie& lhs, const PoGoCmp::PokemonSpecie& rhs) {
            auto lval = PropertyValueByName(lhs, sortCriteria), rval = PropertyValueByName(rhs, sortCriteria);
            if (lval < 0 || rval < 0) LogErrorAndExit("Invalid sorting criteria: '" + sortCriteria + "'.");
            return cmp(lval, rval);
        };

        using namespace StringUtils;

        // E.g. "16,32", or "bulbasaur,ivysaur", or "250". The names can contain following
        // Unicode characters: ♀ (\u2640), ♂ (\u2642), é (\u00e9), É (\u00c9).
        //! @todo Test for this
        const std::wregex rangePattern{
            LR"(([\w-.' \u2640\u2642\u00e9\u00c9]+)(,)?([\w-.' \u2640\u2642\u00e9\u00c9]+)?)"
        };
        std::wsmatch rangeMatches;

        std::vector<PokedexRange> ranges;
        auto includes = opts.OptionValues("-i", "--include");
        if (includes.empty())
            includes.push_back("all");

        for (const auto& include : includes)
        {
            // If first == second, only a single Pokémon is wanted.
            PokedexRange range;
            const auto winclude = Utf8::ToWString(include);
            if (winclude == L"all") { range = maxRange; }
            else if (winclude == L"gen1") { range = gen1Range; }
            else if (winclude == L"gen2") { range = gen2Range; }
            else if (winclude == L"gen3") { range = gen3Range; }
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

                    range.first = IsNumber(rangeFirst) ? std::stoul(rangeFirst)
                        : PoGoCmp::PokemonByIdName.at(PoGoCmp::PokemonNameToId(rangeFirst))->number;

                    range.second = rangeSecond.empty() ? range.first
                        : IsNumber(rangeSecond) ? std::stoul(rangeSecond)
                        : PoGoCmp::PokemonByIdName.at(PoGoCmp::PokemonNameToId(rangeSecond))->number;

                    if (range.first < 1)
                    {
                        LogErrorAndExit("Range's min. (" + std::to_string(range.first) + ") cannot be less than 1");
                    }
                    if (range.first > range.second)
                    {
                        LogErrorAndExit("Range's min. (" + std::to_string(range.first) + ") cannot be greater than max. ("
                            + std::to_string(range.second) + ")");
                    }
                    if (range.second > maxRange.second)
                    {
                        LogErrorAndExit("Range's max. cannot be than " + std::to_string(maxRange.second));
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

        //std::sort(ranges.begin(), ranges.end());
        //ranges.erase(std::unique(ranges.begin(), ranges.end()), ranges.end());

        int numResults = (int)PoGoCmp::PokemonByNumber.size();
        auto resultsVal = opts.OptionValue("-r", "--results");
        if (!resultsVal.empty())
        {
            try
            {
                numResults = std::min(std::stoi(resultsVal), numResults);
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

        Pokemon pkm{};
        pkm.level = 40.f;
        pkm.atk = 15;
        pkm.def = 15;
        pkm.sta = 15;

        if (opts.HasOption("--level")) {
            auto level = opts.OptionValue("--level");
            if (level.empty())
                LogErrorAndExit("Value missing for --level.");

            pkm.level = (float)std::stod(level);
            if (pkm.level < 1 || pkm.level > PoGoCmp::PlayerLevel.cpMultiplier.size())
                LogErrorAndExit("Not a valid level.");
        }

        if (opts.HasOption("--ivs"))
        {
            auto ivs = opts.OptionValue("--ivs");
            if (ivs.empty())
                LogErrorAndExit("Value missing for --ivs.");

            if (ivs.size() != 3)
                LogErrorAndExit("Value must consist of exactly three values.");

            try
            {
                auto values = std::stoul(ivs, nullptr, 16);
                pkm.atk = (values & 0xF00) >> 8;
                pkm.def = (values & 0x0F0) >> 4;
                pkm.sta = (values & 0x00F);
                if (pkm.atk > 15 || pkm.def > 15 || pkm.sta > 15)
                    LogErrorAndExit("IV cannot be higher than F (15).");
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Failed to parse --ivs value '", ivs, "': ", e.what()));
            }
        }

        //ranges.erase(std::unique(ranges.begin(), ranges.end()), ranges.end());

        //std::vector<std::pair<PokedexRange, std::vector<PoGoCmp::PokemonSpecie>>> results;
        std::vector<PoGoCmp::PokemonSpecie> results;

        for (const auto& range : ranges)
        {
            auto dataBegin = PoGoCmp::PokemonByNumber.begin() + (range.first - 1);
            auto dataSize = (size_t)std::distance(dataBegin, dataBegin + (range.second - range.first + 1));
            std::vector<PoGoCmp::PokemonSpecie> result(dataBegin, dataBegin + dataSize);
            std::sort(result.begin(), result.end(), sortFunction);
            //results.push_back({ range, result });
            results.insert(results.end(), result.begin(), result.end());
        }

        std::sort(results.begin(), results.end(), sortFunction);
        results.erase(std::unique(results.begin(), results.end(),
            [](const auto& a, const auto& b) { return a.number == b.number; }),
            results.end());

        std::vector<PoGoCmp::PokemonRarity> rarities;
        auto rarityStrings = opts.OptionValues("--rarity");
        if (rarityStrings.empty())
        {
            rarities.push_back(PoGoCmp::PokemonRarity::NORMAL);
            rarities.push_back(PoGoCmp::PokemonRarity::LEGENDARY);
            rarities.push_back(PoGoCmp::PokemonRarity::MYTHIC);
        }
        else
        {
            for (const auto& rarityStr : rarityStrings)
            {
                auto rarity = PoGoCmp::StringToPokemonRarity(rarityStr.c_str());
                if (rarity == PoGoCmp::PokemonRarity::NONE)
                    LogErrorAndExit("Unknown rarity '" + rarityStr + "'");
                rarities.push_back(rarity);
            }
        }

        results.erase(std::remove_if(results.begin(), results.end(), [&rarities](const auto& pkm) {
            return std::find(rarities.begin(), rarities.end(), pkm.rarity) == rarities.end();
        }), results.end());

        const auto numMatches = (int)results.size();
            //std::accumulate(results.begin(), results.end(), 0,
            //[](const auto& a, const auto& b) { return a + (int)b.second.size(); });

        Utf8::Print(std::to_string(numMatches) + " matches (showing ");

        // negative number means 'show all'
        numResults = numResults < 0 ? numMatches : std::min(numResults, numMatches);

        Utf8::PrintLine(std::to_string(numResults) + (ascending ? " last" : " first") + " results):");

        for (int i = 0; i < numMatches && i < numResults; ++i)
        {
            /* if (verbose) Log("Pokédex range " + std::to_string(result.first.first) + "-" +
                std::to_string(result.first.second) + ":"); */
            // if (verbose) Utf8::Print(std::to_string(i+1) + ": ");

            Utf8::Print(PokemonToString(pkm, results[i], format, sortCriteria));
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