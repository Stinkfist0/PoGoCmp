//#include "../src/PoGoCmp/PoGoCmp.h"
#include "../src/Lib/PoGoDb.h"

#include <cstdlib>
#include <string>

int main(/*int argc, char **argv*/)
{
    const auto byName = PoGoCmp::PokemonByIdName("ivysaur");
    if (byName.number == 0) return EXIT_FAILURE;

    auto pokedexNumber = 2;
    const auto byNumberIt = PoGoCmp::PokemonByNumber.find(byName.number);

    return byNumberIt != PoGoCmp::PokemonByNumber.end() && byNumberIt->second.number == pokedexNumber
        ? EXIT_SUCCESS
        : EXIT_FAILURE;

    //return std::string(PoGoCmpVersionString()).empty() ? EXIT_FAILURE : EXIT_SUCCESS;
}
