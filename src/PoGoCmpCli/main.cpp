#include "../PoGoCmp/PoGoCmp.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr <<
            "Invalid usage. Supported parameters:\n"
            "-v, --version\n";
        return EXIT_FAILURE;
    }

    std::string arg1{ argv[1] };
    if (arg1 == "-v" || arg1 == "--version")
    {
        std::cout << "PoGoCmp version " << PoGoCmpVersionString() << "\n";
    }
    else
    {
        std::cerr << "Unknown parameter '" << arg1 << "'\n";
    }
}
