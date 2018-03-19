#include "../PoGoCmp/PoGoCmp.h"
#include "../PoGoCmp/PoGoCmpDb.h"
#include "../PoGoCmp/StringUtils.h"
#include "../PoGoCmp/Utf8.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <regex>
#include <functional>
#include <set>

const std::string defaultFormat{ "%nu %na ATK %a DEF %d STA %s TYPE %Tt\\n" };
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
    //bool multiple; // are multiple same options supported
};

std::ostream& operator<< (std::ostream& out, const ProgramOption& opt)
{
    out << opt.shortName << (opt.longName.empty() ? "" : ",") << opt.longName << ": " << opt.help;
    return out;
}

const std::vector<ProgramOption> programsOptions {
    { "-h", "--help", "Print help." },
    { "-v", "--version", "Print version information and exit."},
    //{ "", "--verbose", "Verbose log prints."},
    //{ "-a", "--ascending", "Sort the results in ascending order (default)." },
    { "-d", "--descending", "Sort the results in descending order (ascending by default)." },
    { "-i", "--include", "Specify Pokemon or range of Pokemon to be included (use multiple options to specify multiple individual Pokemon): "
        "'all' (default), 'gen<X>' (1/2/3), '<X>[,Y]' (inclusive Pokedex range, both numbers and names supported."},
    { "-r", "--results", "Show only first N entries of the results, e.g. '-r 5' (negative number means 'show all')." },
    { "-f", "--format",
        "Specify format for the output,'" + defaultFormat + "' by default: "
        "%nu (number), %na (name), %a (attack), %d (defense), %s (stamina), %T (primary type), %t (secondary type) "
        "%Tt (both types, 2nd type only if applicable), %o (sorting criteria), \n (new line), \t (tab)"
    },
    // Commands
    { "sort", "", "Sort the Pokemon by certain criteria: "
        "'number' (default), 'attack', 'defense' or 'stamina'."
    },
    { "info", "", "Print information about the available data set." }
};

struct ProgamOptionMap
{
    using StringVector = std::vector<std::string>;
    /// (shortName, longName)
    //using NamePair = std::pair <std::string, std::string>;

    ProgamOptionMap(const StringVector& args) : args(args) {}
    /// @param argv The program invokation argument argv[0] is ignored.
    ProgamOptionMap(int argc, char **argv/*, const std::vector<ProgramOption>& options*/)
        : ProgamOptionMap({ argv + 1, argv + argc })
    {
        //for (const auto& opt : options)
        //{
        //    opts[std::make_pair(opt.shortName, opt.longName)] = OptionValue(opt.shortName, opt.longName);
        //}
    }

    bool HasOption(const std::string& shortName, const std::string& longName) const
    {
        return std::count_if(args.begin(), args.end(),
            [&](const auto& arg) { return arg == shortName || arg == longName; }) > 0;
    }

    bool HasOption(const std::string& name) const { return HasOption(name, name); }

    std::string OptionValue(const std::string& shortName, const std::string& longName) const
    {
        auto optIt = std::find_if(args.begin(), args.end(), [&](const auto& arg) {
            return arg == shortName || arg == longName;
        });
        auto valueIt = optIt != args.end() ? optIt + 1 : args.end();
        return IsValue(valueIt) ? *valueIt : "";
    }

    std::string OptionValue(const std::string& name) const { return OptionValue(name, name); }

    StringVector OptionValues(const std::string& shortName, const std::string& longName) const
    {
        StringVector ret;
        auto it = args.begin();
        while (it != args.end())
        {
            auto optIt = std::find_if(it, args.end(), [&](const auto& arg) {
                return arg == shortName || arg == longName;
            });
            auto valueIt = optIt != args.end() ? optIt + 1 : args.end();
            if (IsValue(valueIt))
                ret.push_back(*valueIt);
            it = valueIt;
            if (it != args.end())
                ++it;
        }
        return ret;
    }

    /// @param valueIt Iterator to program option which should be considered a value.
    bool IsValue(StringVector::const_iterator it) const
    {
        if (it == args.begin() || it == args.end()) return false;
        auto prevIt = (it - 1);
        return (prevIt == args.begin() || ProgramOption::IsArg(*prevIt)) && !ProgramOption::IsArg(*it);
    }
    /// The original arguments
    std::vector<std::string> args;
    /// Program options and their values
    //std::map<NamePair, std::string> opts;
};

void LogErrorAndExit(const std::string& msg)
{
    std::cerr << msg << "\n";
    std::exit(EXIT_FAILURE);
}

uint16_t PropertyValueByName(const PoGoCmp::PokemonSpecie& pkm, const std::string& prop)
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
    const auto types = Concat(type, (!type2.empty() ? "/" : ""), type2);

    fmt = std::regex_replace(fmt, std::regex("%nu"), std::to_string(pkm.number));
    fmt = std::regex_replace(fmt, std::regex("%na"), SnakeCaseToTitleCase(PoGoCmp::PokemonIdToName(pkm.name)));
    fmt = std::regex_replace(fmt, std::regex("%a"), std::to_string(pkm.baseAtk));
    fmt = std::regex_replace(fmt, std::regex("%d"), std::to_string(pkm.baseDef));
    fmt = std::regex_replace(fmt, std::regex("%s"), std::to_string(pkm.baseSta));
    fmt = std::regex_replace(fmt, std::regex("%Tt"), types);
    fmt = std::regex_replace(fmt, std::regex("%T"), type);
    fmt = std::regex_replace(fmt, std::regex("%t"), type2);
    fmt = std::regex_replace(fmt, std::regex("%o"), std::to_string(PropertyValueByName(pkm, sortCriteria)));
    //fmt = std::regex_replace(fmt, std::regex("%%"), "%");
    fmt = std::regex_replace(fmt, std::regex("\\\\n"), "\n");
    fmt = std::regex_replace(fmt, std::regex("\\\\t"), "\t");
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
    std::vector<Utf8::String> args = Utf8::GetCommandLineUtf8(argc, argv);
    ProgamOptionMap opts{ {args.begin() + 1, args.end()} };
    if (opts.args.empty())
    {
        std::cerr << "Invalid usage.\n";
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
        std::cout << "PoGoCmpCli " << PoGoCmpVersionString() << "\n";
        return EXIT_SUCCESS;
    }

    /// @todo Use info also to list type-effectiveness, attacks, etc.
    if (opts.HasOption("info"))
    {
        std::cout << "Available Pokedex range " << std::to_string(maxRange.first)
            << "-" << std::to_string(maxRange.second) << "\n";
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
        const std::string sortCriteria = opts.OptionValue("sort");
        auto sortFunction = [ascending, &sortCriteria](const PoGoCmp::PokemonSpecie& lhs, const PoGoCmp::PokemonSpecie& rhs) {
            return ascending
                ? PropertyValueByName(lhs, sortCriteria) < PropertyValueByName(rhs, sortCriteria)
                : PropertyValueByName(lhs, sortCriteria) > PropertyValueByName(rhs, sortCriteria);
        };

        using namespace StringUtils;

        /// @todo Unicode support
        //const std::wregex rangePattern{
        //    R"(([\w+\u2640\u2642\u00e9\u00c9)-.' ]+)- ?(\b[\w+\u2640\u2642\u00e9\u00c9)-.' ]+))"
        //};
        /// @todo trim in the regex instead of using trim() functions
        const std::regex rangePattern{ R"(([\w-.' ]+)(,)?([\w-.' ]+)?)" }; // e.g. "16,32", or "bulbasaur,ivysaur", or "250"
        std::smatch rangeMatches;

        std::set<PokedexRange> ranges;
        auto includes = opts.OptionValues("-i", "--include");
        if (includes.empty())
            includes.push_back("all");

        for (const auto& include : includes)
        {
            PokedexRange range;
            if (include == "all") { range = maxRange; }
            else if (include == "gen1") { range = gen1Range; }
            else if (include == "gen2") { range = gen2Range; }
            else if (include == "gen3") { range = gen3Range; }
            else if (std::regex_match(include, rangeMatches, rangePattern))
            {
                try
                {
                    auto rangeFirst = rangeMatches[1].str();
                    Trim(rangeFirst);
                    //auto type = rangeMatches[2].str();
                    auto rangeSecond = rangeMatches.size() > 2 ? rangeMatches[3].str() : "";
                    Trim(rangeSecond);

                    range.first = IsNumber(rangeFirst) ? std::stoul(rangeFirst)
                        : PoGoCmp::PokemonByName.at(PoGoCmp::PokemonNameToId(rangeFirst))->number;

                    range.second = rangeSecond.empty() ? range.first
                        : IsNumber(rangeSecond) ? std::stoul(rangeSecond)
                        : PoGoCmp::PokemonByName.at(PoGoCmp::PokemonNameToId(rangeSecond))->number;

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

            ranges.insert(range);
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


        for (const auto& range : ranges)
        {
            std::cout << "Pokedex range " << range.first << "-" << range.second << ":\n";
            auto dataBegin = PoGoCmp::PokemonByNumber.begin() + (range.first - 1);
            auto dataSize = (size_t)std::distance(dataBegin, dataBegin + (range.second - range.first + 1));
            std::vector<PoGoCmp::PokemonSpecie> data(dataBegin, dataBegin + dataSize);

            std::cout << data.size() << " matches (showing ";
            numResults = std::min(numResults, dataSize);
            std::cout << numResults << " results):\n";

            std::sort(data.begin(), data.end(), sortFunction);

            std::vector<PoGoCmp::PokemonSpecie> results;
            results.insert(results.begin(), data.begin(), data.begin() + numResults);

            for (const auto& pkm : results)
            {
                std::cout << PokemonToString(pkm, format, sortCriteria);
            }
        }
        //std::cout << "\n";

        ret = EXIT_SUCCESS;
    }

    if (ret != EXIT_SUCCESS)
    {
        std::cerr << "Error: no command given.\n";
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
