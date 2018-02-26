#include "../PoGoCmp/PoGoCmp.h"
#include "../PoGoCmp/PoGoCmpDb.h"

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
    static bool IsArg(const std::string &str)
    {
        /// @todo could support also /-prefix, also e.g. would be nice to ignore/warn about e.g. ---prefix
        return str.find_first_of("-") == 0;
    }

    enum Type : bool { Cmd, Arg };

    Type type;
    std::string shortName;
    std::string longName;
    std::string help;
};

std::ostream& operator<< (std::ostream& out, const ProgramOption& opt)
{
    out << opt.shortName << (opt.longName.empty() ? "" : ",") << opt.longName << ": " << opt.help;
    return out;
}

const std::string defaultFormat{ "%nu %na atk %a def %d sta %s type %T type2 %t" };

const std::vector<ProgramOption> programsOptions {
    { "-h", "--help", "Print help." },
    { "-v", "--version", "Print version information."},
    { "list", "", "List all Pokemon."},
    //{ "-a", "--ascending", "Sort possible print in ascending order." },
    { "-d", "--descending", "Sort possible print in descending order (ascending by default)." },
    { "-f", "--format",
        "Specify format for the output,'" + defaultFormat + "' by default: "
        "%nu (number), %na (name), %a (attack), %d (defense), %s (stamina), %T (primary type), %t (secondary type)"
    }
};

struct ProgamOptionMap
{
    using ConstIterator = std::vector<std::string>::const_iterator;

    ProgamOptionMap(int argc, char **argv) :
        args(argv + 1, argv + argc) // ignore the program invokation argument argv[0]
    {
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
        auto valueIt = optIt + 1;
        return optIt != args.end() && IsValue(valueIt) ? *valueIt : "";
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
    fmt = std::regex_replace(fmt, std::regex("%nu"), std::to_string(pkm.number));
    fmt = std::regex_replace(fmt, std::regex("%na"), pkm.name);
    fmt = std::regex_replace(fmt, std::regex("%a"), std::to_string(pkm.baseAtk));
    fmt = std::regex_replace(fmt, std::regex("%d"), std::to_string(pkm.baseDef));
    fmt = std::regex_replace(fmt, std::regex("%s"), std::to_string(pkm.baseSta));
    fmt = std::regex_replace(fmt, std::regex("%T"), PoGoCmp::PokemonTypeToString(pkm.type));
    fmt = std::regex_replace(fmt, std::regex("%t"), PoGoCmp::PokemonTypeToString(pkm.type2));
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

    if (opts.HasOption("list"))
    {
        std::string format = defaultFormat;
        if (opts.HasOption("-f", "--format"))
        {
            format = opts.OptionValue("-f", "--format");
            if (format.empty())
            {
                std::cerr << "Value missing for -f/--format\n";
                return EXIT_FAILURE;
            }
        }

        auto allPokemon = PoGoCmp::PokemonByNumber;
        if (opts.HasOption("-d", "--descending"))
        {
            std::reverse(allPokemon.begin(), allPokemon.end());
        }
        for(const auto& pkm : allPokemon)
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
