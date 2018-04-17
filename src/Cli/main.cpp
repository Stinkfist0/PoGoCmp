#include "../Lib/PoGoCmp.h"
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
//! @param base Pokémon's base stats.
//! @param level [1,maxLevel], 0.5 steps, maxLevel 40 for now.
//! @param atk [0, 15], integer.
//! @param def [0, 15], integer.
//! @param sta [0, 15], integer.
//! @note The game doesn't show CP under 10 but this function returns the actual CP even for values below 10.
//! @return < 0 on invalid input, > 0 otherwise
int ComputeCp(const PoGoCmp::PokemonSpecie& base, float level, float atk, float def, float sta)
{
    const auto numLevels = (float)PoGoCmp::PlayerLevel.cpMultiplier.size();
    if (level  < 1 || level > numLevels) return -1;
    if (atk < 0 || atk > 15) return -1;
    if (def < 0 || def > 15) return -1;
    if (sta < 0 || sta > 15) return -1;

    float levelIdx;
    auto levelFact = std::modf(level, &levelIdx);
    if (levelFact != 0.f && levelFact != 0.5f) return -1;
    levelIdx -= 1;
    auto nextLevelIdx = std::min(levelIdx + 1, numLevels -1);
    auto cpmBase = PoGoCmp::PlayerLevel.cpMultiplier[(size_t)levelIdx];
    auto cpmNext = PoGoCmp::PlayerLevel.cpMultiplier[(size_t)nextLevelIdx];
    auto cpmStep = (std::pow(cpmNext, 2) - std::pow(cpmBase, 2)) / 2.f;
    auto cpm = levelFact != 0.f ? std::sqrt(std::pow(cpmBase, 2) + cpmStep) : cpmBase;
    atk = base.baseAtk + atk;
    def = base.baseDef + def;
    sta = base.baseSta + sta;
    return (int)std::floor(atk * std::pow(def, 0.5f) * std::pow(sta, 0.5f) * std::pow(cpm, 2) / 10.f);
}

int ComputeCp(const PoGoCmp::PokemonSpecie& base, const PoGoCmp::Pokemon& pkm)
{
    return ComputeCp(base, pkm.level, pkm.atk, pkm.def, pkm.sta);
}

//! https://www.reddit.com/r/TheSilphRoad/comments/6wrw6a/raid_boss_cp_explained_if_it_hasnt_been_already/
//! \todo Weird that raid bosses would have different formulat without CPM.
//! Investigate if ComputeCp() could made work somehow.
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

bool Equals(float a, float b, float eps = 1e-5f) { return std::abs(a - b) < eps; }
bool IsZero(float a, float eps = 1e-5f) { return Equals(a, 0, eps); }

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
    else if (prop == "gender") { return IsZero(pkm.malePercent) && IsZero(pkm.femalePercent) ? INFINITY : pkm.malePercent; }
    else { return NAN; }
}

std::string FloatToString(float f) { std::stringstream ss; ss << f; return ss.str(); };

Utf8::String FormatGender(float malePercent, float femalePercent)
{
    std::wstring genderText = L"○";
    if (!IsZero(malePercent))
    {
        genderText = Utf8::ToWString(FloatToString(malePercent)) + L" % ♂";
    }
    if (!IsZero(femalePercent))
    {
        if (IsZero(malePercent))
            genderText = Utf8::ToWString(FloatToString(femalePercent)) + L" % ♀";
        else
            genderText += L" " + Utf8::ToWString(FloatToString(femalePercent)) + L" % ♀";
    }
    return Utf8::FromWString(genderText);
}

Utf8::String PokemonToString(
    const PoGoCmp::PokemonSpecie& base,
    //const Pokemon& pkm,
    int cp, Utf8::String fmt,
    const std::string& sortCriteria)
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
    fmt = std::regex_replace(fmt, std::regex("%o"), FloatToString(PropertyValueByName(base, sortCriteria)));
    fmt = std::regex_replace(fmt, std::regex("%cp"), std::to_string(cp));
    fmt = std::regex_replace(fmt, std::regex("%b"), std::to_string(base.buddyDistance));
    //fmt = std::regex_replace(fmt, std::regex("%%"), "%");
    fmt = std::regex_replace(fmt, std::regex("%g"), FormatGender(base.malePercent, base.femalePercent));

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
        "%nu (number), %na (name), %a (base attack), %d (base defense), %s (base stamina), %T (primary type), %t (secondary type) "
        "%Tt (both types, 2nd type only if applicable), %o (sorting criteria), %cp (max. CP), %b (buddy distance, km), "
        "(%g gender ratio by male percentage) \\n (new line), \\t (tab)"
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
    // Commands
    { "sort", "",
        L"Sort the Pokémon by certain criteria: "
        "number (default), attack/atk, defense/def, stamina/sta/hp, bulk (def*sta), total(atk+def+sta), or gender."
    },
    { "powerup", "",
        L"Calculate resources required to power up a Pokémon from certain level to another, e.g. "
        "'powerup 15.5,31.5'"
    },
    { "perfect-ivs", "",
        L"Generate a search string that can be used to filter perfect wild Pokémon catches. "
        L"Only single name/ID as an argument supported."
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

    using namespace StringUtils;
    using namespace PoGoCmp;

    //! @todo Use info also to list type-effectiveness, attacks, etc.
    if (opts.HasOption("info"))
    {
        Log("Available Pokedex range: " + std::to_string(MaxRange.first) + "-" + std::to_string(MaxRange.second));
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
        using Cmp = std::function<bool(float, float)>;
        const auto cmp = ascending ? Cmp(std::less<>()) : Cmp(std::greater<>());
        const std::string sortCriteria = opts.OptionValue("sort");
        auto sortFunction = [&cmp, &sortCriteria](const PoGoCmp::PokemonSpecie& lhs, const PoGoCmp::PokemonSpecie& rhs) {
            auto lval = PropertyValueByName(lhs, sortCriteria), rval = PropertyValueByName(rhs, sortCriteria);
            if (std::isnan(lval) || std::isnan(rval)) LogErrorAndExit("Invalid sorting criteria: '" + sortCriteria + "'.");
            return cmp(lval, rval);
        };

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
            if (winclude == L"all") { range = MaxRange; }
            else if (winclude == L"gen1") { range = Gen1Range; }
            else if (winclude == L"gen2") { range = Gen2Range; }
            else if (winclude == L"gen3") { range = Gen3Range; }
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
                    if (range.second > MaxRange.second)
                    {
                        LogErrorAndExit("Range's max. cannot be than " + std::to_string(MaxRange.second));
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

        if (opts.HasOption("--level"))
        {
            auto level = opts.OptionValue("--level");
            if (level.empty())
                LogErrorAndExit("Value missing for --level.");

            try
            {
                pkm.level = std::stof(level);
                if (pkm.level < 1 || pkm.level > PoGoCmp::PlayerLevel.cpMultiplier.size())
                    throw std::runtime_error("Level not within valid range.");
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Not a valid level '", level, "': ", e.what()));
            }
        }

        if (opts.HasOption("--ivs"))
        {
            auto ivs = opts.OptionValue("--ivs");
            if (ivs.empty())
                LogErrorAndExit("Value missing for --ivs.");

            if (std::count_if(ivs.begin(), ivs.end(),
                [](auto c) { return std::isxdigit(c, std::locale::classic()); }) != 3)
            {
                LogErrorAndExit("Value must consist of exactly three hexadecimal digits.");
            }

            try
            {
                auto values = std::stoul(ivs, nullptr, 16);
                pkm.atk = (values & 0xF00) >> 8;
                pkm.def = (values & 0x0F0) >> 4;
                pkm.sta = (values & 0x00F);
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Failed to parse --ivs value '", ivs, "': ", e.what()));
            }
        }

        const bool isRaidBoss = opts.HasOption("--raidLevel");
        if (isRaidBoss)
        {
            auto level = opts.OptionValue("--raidLevel");
            if (level.empty())
                LogErrorAndExit("Value missing for --raidLevel.");

            if (opts.HasOption("--level"))
                Log("--level value is ignored due to --raidLevel.");
            if (opts.HasOption("--ivs"))
                Log("--ivs value is ignored due to --raidLevel.");

            try
            {
                auto raidLevel = std::stof(level);
                if (raidLevel < 1 || raidLevel > RaidLevels.size())
                    throw std::runtime_error("Level not within valid range.");
                pkm = RaidLevels[(size_t)raidLevel - 1];
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(Concat("Not a valid level '", level, "': ", e.what()));
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
            //std::sort(result.begin(), result.end(), sortFunction);
            //results.push_back({ range, result });
            results.insert(results.end(), result.begin(), result.end());
        }

        std::sort(results.begin(), results.end(), sortFunction);
        results.erase(std::unique(results.begin(), results.end(),
            [](const auto& a, const auto& b) { return a.number == b.number; }),
            results.end());

        std::vector<PoGoCmp::PokemonRarity> rarities{
            PoGoCmp::PokemonRarity::NORMAL,
            PoGoCmp::PokemonRarity::LEGENDARY,
            PoGoCmp::PokemonRarity::MYTHIC
        };
        auto rarityStrings = opts.OptionValues("--rarity");
        if (!rarityStrings.empty())
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
            const auto& base = results[i];
            int cp = isRaidBoss ? ComputeRaidBossCp(base, pkm) : ComputeCp(base, pkm);
            /* if (verbose) Log("Pokédex range " + std::to_string(result.first.first) + "-" +
                std::to_string(result.first.second) + ":"); */
            // if (verbose) Utf8::Print(std::to_string(i+1) + ": ");

            Utf8::Print(PokemonToString(base, cp, format, sortCriteria));
        }

        ret = EXIT_SUCCESS;
    }
    else if (opts.HasOption("powerup"))
    {
        auto powerupRange = Split(opts.OptionValue("powerup"), ',', StringUtils::RemoveEmptyEntries);
        if (powerupRange.size() != 2)
            LogErrorAndExit("Power-up range must consist of two comma-separated numbers.");

        try
        {
            auto begin = std::stof(powerupRange[0]) - 1;
            auto end = std::stof(powerupRange[1]) - 1;
            if (begin < 0 || end >= PoGoCmp::PokemonUpgrades.candyCost.size())
                LogErrorAndExit("Range out of bounds.");
            if (end < begin)
                LogErrorAndExit("Range's end cannot be greater than range's begin.");
            if (Equals(begin, end))
                LogErrorAndExit("Range's begin and end cannot be equal.");

            const auto step = 1.f / PoGoCmp::PokemonUpgrades.upgradesPerLevel;
            float dummy;
            const auto beginFract = std::modf(begin, &dummy);
            if (!Equals(beginFract, 0.f) && !Equals(beginFract, step))
                LogErrorAndExit("Invalid factorial for range's begin.");
            const auto endFract = std::modf(end, &dummy);
            if (!Equals(endFract, 0.f) && !Equals(endFract, step))
                LogErrorAndExit("Invalid factorial for range's end.");

            const auto& candy = PoGoCmp::PokemonUpgrades.candyCost;
            const auto& dust = PoGoCmp::PokemonUpgrades.stardustCost;
            int candyTotal{}, dustTotal{};
            for (float i = begin; i < end; i += step)
            {
                auto idx = (size_t)std::floor(i);
                candyTotal += candy[idx];
                dustTotal += dust[idx];
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
    else if (opts.HasOption("perfect-ivs"))
    {
        auto val = opts.OptionValue("perfect-ivs");
        try
        {
            auto number = IsNumber(val)
                ? std::stoul(val)
                : PoGoCmp::PokemonByIdName.at(PoGoCmp::PokemonNameToId(val))->number;
            auto pkm = PoGoCmp::PokemonByNumber.at(number - 1);

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
