//#include "../src/PoGoCmp/PoGoCmp.h"
#include "../src/PoGoCmp/PoGoCmpDb.h"

#include <cstdlib>
#include <string>

int main(/*int argc, char **argv*/)
{
    auto byName = PoGoCmp::PokemonByName.find("ivysaur");
    if (byName == PoGoCmp::PokemonByName.end()) return EXIT_FAILURE;

    auto pokedexNumber = 2;
    const auto& byNumber = PoGoCmp::PokemonByNumber[(*byName).second.number - 1];

    return byNumber.number == pokedexNumber? EXIT_SUCCESS : EXIT_FAILURE;

    //return std::string(PoGoCmpVersionString()).empty() ? EXIT_FAILURE : EXIT_SUCCESS;
}
