//#include "../src/PoGoCmp/PoGoCmp.h"
#include "../src/PoGoCmp/PoGoCmpDb.h"

#include <cstdlib>
#include <iostream>

#define Assert(a, b) \
if ((a) != (b)) \
{ std::cerr << "'" << a << "' did not map into '" << b << "'\n"; return EXIT_FAILURE; }

int main(/*int argc, char **argv*/)
{
    Assert(PoGoCmp::PokemonNameToId("Nidoran Female"), "NIDORAN_FEMALE");
    Assert(PoGoCmp::PokemonNameToId("Nidoran Male"), "NIDORAN_MALE");
    Assert(PoGoCmp::PokemonNameToId("Farfetch'd"), "FARFETCHD");
    Assert(PoGoCmp::PokemonNameToId("Mr. Mime"), "MR_MIME");
    Assert(PoGoCmp::PokemonNameToId("Ho-Oh"), "HO_OH");

    Assert(PoGoCmp::PokemonIdToName("NIDORAN_FEMALE"), "Nidoran Female");
    Assert(PoGoCmp::PokemonIdToName("NIDORAN_MALE"), "Nidoran Male");
    Assert(PoGoCmp::PokemonIdToName("FARFETCHD"), "Farfetch'd");
    Assert(PoGoCmp::PokemonIdToName("MR_MIME"), "Mr. Mime");
    Assert(PoGoCmp::PokemonIdToName("HO_OH"), "Ho-Oh");

    return EXIT_SUCCESS;
}
