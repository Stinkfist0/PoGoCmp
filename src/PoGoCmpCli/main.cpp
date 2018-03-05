#include "../PoGoCmp/PoGoCmp.h"
#include "../PoGoCmp/PoGoCmpDb.h"
#include "../PoGoCmp/StringUtils.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <regex>
#include <functional>

const std::string defaultFormat{ "%nu %na ATK %a DEF %d STA %s TYPE %Tt" };
/// Pokedex number range, number - 1 for the index in PoGoCmp::PokemonByNumber.
/// @todo generate values for these in PoGoCmpDb.h
using PokedexRange = std::pair<size_t, size_t>;
const PokedexRange gen1Range{ 1, 151 };
const PokedexRange gen2Range{ 152, 251 };
const PokedexRange gen3Range{ 252, 386 };
const PokedexRange maxRange{ 1, PoGoCmp::PokemonByNumber.size() };

struct ProgramOption
{
    ProgramOption() {}
    /// type is deduced automatically from the shortName or longName.
    ProgramOption(const std::string& sn, const std::string& ln, const std::string& h) :
        shortName(sn), longName(ln), help(h)
    {
        type = IsArg(shortName) || IsArg(longName) ? Arg : Cmd;
    }
    /// Commands have no prefix, arguments have either "-" or "--" prefix.
    /// @todo could support also /-prefix, also e.g. would be nice to ignore/warn about e.g. ---prefix
    static bool IsArg(const std::string &str)
    {
        return str.find_first_of("-") == 0 && !StringUtils::IsNumber(str);
    }

    enum Type : bool { Cmd, Arg };

    Type type;
    std::string shortName;
    std::string longName;
    std::string help;
    ///std::string longHelp; // when -h <command> is used?
};

std::ostream& operator<< (std::ostream& out, const ProgramOption& opt)
{
    out << opt.shortName << (opt.longName.empty() ? "" : ",") << opt.longName << ": " << opt.help;
    return out;
}

const std::vector<ProgramOption> programsOptions {
    { "-h", "--help", "Print help." },
    { "-v", "--version", "Print version information."},
    { "sort", "", "Sort the Pokemon by certain criteria: "
        "'number' (default), 'attack', 'defense' or 'stamina'."
    },
    //{ "-a", "--ascending", "Sort the results in ascending order (default)." },
    { "-d", "--descending", "Sort the results in descending order (ascending by default)." },
    { "-i", "--include", "List the first Pokemon in order by certain criteria: "
        "'all' (default), 'gen<X>' (1/2/3), '<X>-<Y>' (inclusive Pokedex range, e.g.'16-32)'"},
    { "-r", "--results", "Show only first N entries of the results, e.g. '-r 5' (negative number means 'show all')." },
    { "-f", "--format",
        "Specify format for the output,'" + defaultFormat + "' by default: "
        "%nu (number), %na (name), %a (attack), %d (defense), %s (stamina), %T (primary type), %t (secondary type) "
        "%Tt (both types, 2nd type only if applicable), %o (sorting criteria)"
    }
};

struct ProgamOptionMap
{
    using ArgConstIterator = std::vector<std::string>::const_iterator;
    /// (shortName, longName)
    //using NamePair = std::pair <std::string, std::string>;

    /// The program invokation argument argv[0] is ignored.
    ProgamOptionMap(int argc, char **argv/*, const std::vector<ProgramOption>& options*/) :
        args(argv + 1, argv + argc)
    {
        //options
    }

    bool HasOption(const std::string& shortName, const std::string& longName) const
    {
        return std::count_if(args.begin(), args.end(),
            [&](const auto& arg) { return arg == shortName || arg == longName; }) > 0;
    }

    bool HasOption(const std::string& name) const { return HasOption(name, name); }

    std::string OptionValue(const std::string& shortName, const std::string& longName) const
    {
        auto optIt = std::find_if(args.begin(), args.end(),
            [&](const auto& arg) { return arg == shortName || arg == longName; });
        auto valueIt = optIt != args.end() ? optIt + 1 : args.end();
        return IsValue(valueIt) ? *valueIt : "";
    }

    std::string OptionValue(const std::string& name) const { return OptionValue(name, name); }

    /// @param valueIt Iterator to program option which should be considered a value.
    bool IsValue(ArgConstIterator it) const
    {
        if (it == args.begin() || it == args.end()) return false;
        auto prevIt = (it - 1);
        return (prevIt == args.begin() || ProgramOption::IsArg(*prevIt)) && !ProgramOption::IsArg(*it);
    }

    std::vector<std::string> args;
    //std::map<NamePair, std::string> opts;
};

void LogErrorAndExit(const std::string& msg)
{
    std::cerr << msg << "\n";
    std::exit(EXIT_FAILURE);
}

uint16_t PropertyByName(const PoGoCmp::PokemonSpecie& pkm, const std::string& prop)
{
    if (prop.empty() || prop == "number") { return pkm.number; }
    else if (prop == "attack") { return pkm.baseAtk; }
    else if (prop == "defense") { return pkm.baseDef; }
    else if (prop == "stamina") { return pkm.baseSta; }
    else { LogErrorAndExit("Invalid sorting criteria: '" + prop + "'."); return UINT16_MAX;  }
}

std::string PokemonToString(const PoGoCmp::PokemonSpecie& pkm, std::string fmt, const std::string& sortCriteria)
{
    using namespace StringUtils;
    const auto type = SnakeCaseToTitleCase(PoGoCmp::PokemonTypeToString(pkm.type));
    const auto type2 = pkm.type2 == PoGoCmp::PokemonType::NONE
        ? "" : SnakeCaseToTitleCase(PoGoCmp::PokemonTypeToString(pkm.type2));
    const auto types = StringUtils::Concatenate(type, (!type2.empty() ? "/" : ""), type2);

    fmt = std::regex_replace(fmt, std::regex("%nu"), std::to_string(pkm.number));
    fmt = std::regex_replace(fmt, std::regex("%na"), SnakeCaseToTitleCase(pkm.name)); /**< @todo handle Ho-Oh and other special cases */
    fmt = std::regex_replace(fmt, std::regex("%a"), std::to_string(pkm.baseAtk));
    fmt = std::regex_replace(fmt, std::regex("%d"), std::to_string(pkm.baseDef));
    fmt = std::regex_replace(fmt, std::regex("%s"), std::to_string(pkm.baseSta));
    fmt = std::regex_replace(fmt, std::regex("%Tt"), types);
    fmt = std::regex_replace(fmt, std::regex("%T"), type);
    fmt = std::regex_replace(fmt, std::regex("%t"), type2);
    fmt = std::regex_replace(fmt, std::regex("%o"), std::to_string(PropertyByName(pkm, sortCriteria)));
    //str = std::regex_replace(str, std::regex("%%"), "%");
    return fmt;
}

/// @todo indentation
void PrintHelp()
{
    std::cout << "Supported commands:\n";
    for (const auto& opt : programsOptions)
        if (opt.type == ProgramOption::Cmd)
            std::cout << "  " << opt << "\n";
    std::cout << "Supported arguments:\n";
    for (const auto& opt : programsOptions)
        if (opt.type == ProgramOption::Arg)
            std::cout << "  " << opt << "\n";
}

int main(int argc, char **argv)
{
    ProgamOptionMap opts(argc, argv);
    if (opts.args.empty())
    {
        std::cerr << "Invalid usage.\n";
        PrintHelp();
        return EXIT_FAILURE;
    }

    std::vector<std::string> knownArgs;
    for (const auto& opt : programsOptions)
    {
        knownArgs.push_back(opt.shortName);
        knownArgs.push_back(opt.longName);
    }

    if (opts.HasOption("-v", "--version"))
    {
        std::cout << "PoGoCmpCli " << PoGoCmpVersionString() << "\n";
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
        const std::string sortCriteria = opts.OptionValue("sort");
        auto sortFunction = [ascending, &sortCriteria](const PoGoCmp::PokemonSpecie& lhs, const PoGoCmp::PokemonSpecie& rhs) {
            return ascending
                ? PropertyByName(lhs, sortCriteria) < PropertyByName(rhs, sortCriteria)
                : PropertyByName(lhs, sortCriteria) > PropertyByName(rhs, sortCriteria);
        };

        const std::regex rangePattern{ "(\\d+)-(\\d+)" }; // e.g. "16-32"
        std::smatch rangeMatches;

        std::string include = opts.OptionValue("-i", "--include");
        PokedexRange range;
        if (include == "all" || include.empty()) { range = maxRange; }
        else if (include == "gen1") { range = gen1Range; }
        else if (include == "gen2") { range = gen2Range; }
        else if (include == "gen3") { range = gen3Range; }
        else if (std::regex_match(include, rangeMatches, rangePattern))
        {
            try
            {
                range.first = std::stoul(rangeMatches[1]);
                range.second = std::stoul(rangeMatches[2]);
                if (range.first < 1)
                    LogErrorAndExit("Range's min. cannot be less than 1");
                if (range.first > range.second)
                    LogErrorAndExit("Range's min. cannot be greater than max.");
                if (range.second > maxRange.second)
                    LogErrorAndExit("Range's max. cannot be than " + std::to_string(maxRange.second));
            }
            catch (const std::exception& e)
            {
                LogErrorAndExit(StringUtils::Concatenate(
                    "Failed to parse include's value, '", include, "': ", e.what()));
            }
        }
        else
        {
            LogErrorAndExit(StringUtils::Concatenate("Invalid value for 'include', '", include, "'"));
        }

        size_t numResults = PoGoCmp::PokemonByNumber.size();
        auto resultsVal = opts.OptionValue("-r", "--results");
        if (!resultsVal.empty())
        {
            try
            {
                numResults = std::min((size_t)std::stoul(resultsVal), numResults);
            }
            catch(const std::exception& e)
            {
                LogErrorAndExit(StringUtils::Concatenate(
                    "Failed to parse -r/--results value, '", resultsVal, "': ", e.what()));
            }
        }

        std::cout << "Pokedex range " << range.first << "-" << range.second << "\n";
        auto dataBegin = PoGoCmp::PokemonByNumber.begin() + (range.first - 1);
        auto dataSize = (size_t)std::distance(dataBegin, dataBegin + (range.second - range.first + 1));
        std::vector<PoGoCmp::PokemonSpecie> data(dataBegin, dataBegin + dataSize);

        std::cout << data.size() << " matches (showing ";
        numResults = std::min(numResults, dataSize);
        std::cout << numResults << " results):\n";

        std::sort(data.begin(), data.end(), sortFunction);

        std::vector<PoGoCmp::PokemonSpecie> results;
        results.insert(results.begin(), data.begin(), data.begin() + numResults);

        std::string format = defaultFormat;
        if (opts.HasOption("-f", "--format"))
        {
            format = opts.OptionValue("-f", "--format");
            if (format.empty())
            {
                LogErrorAndExit("Value missing for -f/--format\n");
            }
        }

        for(const auto& pkm : results)
        {
            std::cout << PokemonToString(pkm, format, sortCriteria) << "\n";
        }

        ret = EXIT_SUCCESS;
    }

    /// @todo move this to ProgramOptionMap
    for (auto it = opts.args.begin(); it != opts.args.end(); ++it)
    {
        if (std::count(knownArgs.begin(), knownArgs.end(), *it) == 0 && !opts.IsValue(it))
        {
            std::cerr << "Unknown parameter '" << *it << "'\n";
        }
    }

    return ret;
}
