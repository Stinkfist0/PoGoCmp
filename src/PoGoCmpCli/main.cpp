#include "../PoGoCmp/PoGoCmp.h"
#include "../PoGoCmp/PoGoCmpDb.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

struct ProgramOption
{
    /// Commands have no prefix, arguments have either "-" or "--" prefix.
    enum Type : bool { Cmd, Arg };
    Type type;
    std::string shortName;
    std::string longName;
    std::string help;
};

std::ostream& operator<< (std::ostream& out, const ProgramOption& opt)
{
    /// @todo indentation
    out << opt.shortName << (opt.longName.empty() ? "" : ",") << opt.longName << ": " << opt.help;
    return out;
}


const std::vector<ProgramOption> programsOptions {
    { ProgramOption::Arg, "-h", "--help", "Print help." },
    { ProgramOption::Arg, "-v", "--version", "Print version information."},
    { ProgramOption::Cmd, "list", "", "List all Pokemon."},
    //{ ProgramOption::Arg, "-a", "--ascending", "Sort possible print in ascending order." },
    { ProgramOption::Arg, "-d", "--descending", "Sort possible print in descending order (ascending by default)." }
};

std::string PokemonToString(const PoGoCmp::PokemonSpecie& pkm)
{
    std::stringstream ss;
    ss << pkm.number << " " << pkm.name << " atk " << pkm.baseAtk << " def "
        << pkm.baseDef << " sta " << pkm.baseSta;
    return ss.str();
}

int main(int argc, char **argv)
{
    std::vector<std::string> args(argv + 1, argv + argc); // ignore the program invokation argument argv[0]
    if (args.size() < 1)
    {
        std::cerr << "Invalid usage.\n";
        std::cerr << "Supported commands:\n  ";
        for (const auto& opt : programsOptions)
            if (opt.type == ProgramOption::Cmd) std::cerr << opt << "\n";
        std::cerr << "Supported arguments:\n  ";
        for (const auto& opt : programsOptions)
            if (opt.type == ProgramOption::Arg) std::cerr << opt << "\n";
        return EXIT_FAILURE;
    }

    //bool ascending = std::count_if(args.begin(), args.end(),
    //    [](const auto& arg) { return arg == "-a" || arg == "--ascending"; }) > 0;
    bool descending = std::count_if(args.begin(), args.end(),
        [](const auto& arg) { return arg == "-d" || arg == "--descending"; }) > 0;

    std::vector<std::string> knownArgs;
    for (const auto& opt : programsOptions)
    {
        knownArgs.push_back(opt.shortName);
        knownArgs.push_back(opt.longName);
    }

    for (const auto& arg : args)
    {
        if (arg == "-v" || arg == "--version")
        {
            std::cout << "PoGoCmp version " << PoGoCmpVersionString() << "\n";
        }
        else if (arg == "list")
        {
            auto allPokemon = PoGoCmp::PokemonByNumber;
            if (descending)
            {
                std::reverse(allPokemon.begin(), allPokemon.end());
            }
            for(const auto& pkm : allPokemon)
            {
                std::cout << PokemonToString(pkm) << "\n";
            }
        }
        else if (std::count(knownArgs.begin(), knownArgs.end(), arg) < 1)
        {
            std::cerr << "Unknown parameter '" << arg << "'\n";
        }
    }
}
