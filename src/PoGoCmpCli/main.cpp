#include "../PoGoCmp/PoGoCmp.h"
#include "../PoGoCmp/PoGoCmpDb.h"
#include "../PoGoCmp/StringUtils.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <regex>

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

const std::string defaultFormat{ "%nu %na ATK %a DEF %d STA %s TYPE %Tt" };

const std::vector<ProgramOption> programsOptions {
    { "-h", "--help", "Print help." },
    { "-v", "--version", "Print version information."},
    { "list", "", "List the first Pokemon in order by certain criteria: "
        "'all' (default), 'gen<X>' (1/2/3)"/*, '<X>-<Y>' (Pokedex range e.g. '16-32)'"*/},
    { "-r", "--results", "Show only first N entries of the results, e.g. '-r 5' (negative number means 'show all')." },
    //{ "-a", "--ascending", "Sort possible print in ascending order."
    { "-d", "--descending", "Sort the results in descending order (ascending by default)." },
    { "-f", "--format",
        "Specify format for the output,'" + defaultFormat + "' by default: "
        "%nu (number), %na (name), %a (attack), %d (defense), %s (stamina), %T (primary type), %t (secondary type) "
        "%Tt (both types, 2nd type only if applicable)"
    }
};

struct ProgamOptionMap
{
    using ConstIterator = std::vector<std::string>::const_iterator;

    /// The program invokation argument argv[0] is ignored.
    ProgamOptionMap(int argc, char **argv) : args(argv + 1, argv + argc) {}

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
    bool IsValue(ConstIterator valueIt) const
    {
        return valueIt != args.end() && !ProgramOption::IsArg(*valueIt);
    }

    std::vector<std::string> args;
};

std::string PokemonToString(const PoGoCmp::PokemonSpecie& pkm, std::string fmt)
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

void LogErrorAndExit(const std::string& msg)
{
    std::cerr << msg << "\n";
    std::exit(EXIT_FAILURE);
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

    if (opts.HasOption("list"))
    {
        std::string listValue = opts.OptionValue("list");
        std::pair<size_t, size_t> range; // Pokedex number, number - 1 for the index.
        if (listValue.find("all") == 0 || listValue.empty())
        {
            range.first = 1;
            range.second = PoGoCmp::PokemonByNumber.size();
        }
        else if (listValue.find("gen1") == 0) /// @todo Generic genX
        {
            /// @todo generate values for these in PoGoCmpDb.h
            range.first = 1;
            range.second = 151;
        }
        else if (listValue.find("gen2") == 0)
        {
            range.first = 152;
            range.second = 251;
        }
        else if (listValue.find("gen3") == 0)
        {
            range.first = 252;
            range.second = 386;
        }
        else
        {
            LogErrorAndExit(StringUtils::Concatenate("Invalid value for list ", listValue));
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

        size_t numResults = PoGoCmp::PokemonByNumber.size();
        if (opts.HasOption("-r", "--results"))
        {
            try
            {
                numResults = std::min((size_t)std::stoul(opts.OptionValue("-r", "--results")), numResults);
            }
            catch(const std::exception& e)
            {
                LogErrorAndExit(StringUtils::Concatenate("Failed to parse -r/--results value: ", e.what()));
            }
        }

        auto begin = PoGoCmp::PokemonByNumber.begin() + (range.first - 1);
        auto matches = (size_t)std::distance(begin, PoGoCmp::PokemonByNumber.begin() + range.second);

        std::cout << matches << " matches (showing ";
        numResults = std::min(numResults, matches);
        std::cout << numResults << " results):\n";

        std::vector<PoGoCmp::PokemonSpecie> results;
        results.insert(results.begin(), begin, begin + numResults);

        if (opts.HasOption("-d", "--descending"))
        {
            std::reverse(results.begin(), results.end());
        }

        for(const auto& pkm : results)
        {
            std::cout << PokemonToString(pkm, format) << "\n";
        }
    }

    for (auto it = opts.args.begin(); it != opts.args.end(); ++it)
    {
        if (std::count(knownArgs.begin(), knownArgs.end(), *it) == 0 && !opts.IsValue(it))
        {
            std::cerr << "Unknown parameter '" << *it << "'\n";
        }
    }
}
