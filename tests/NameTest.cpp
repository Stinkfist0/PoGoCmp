//#include "../src/PoGoCmp/PoGoCmp.h"
#include "../src/PoGoCmp/PoGoCmpDb.h"

#include <cstdlib>
#include <iostream>

/// @todo Unicode output
#define Assert(a, b) \
if ((a) != (b)) \
{ std::cerr << "'" << a << "' did not map into '" << b << "'\n"; return EXIT_FAILURE; }

int main(/*int argc, char **argv*/)
{
    using namespace PoGoCmp;
    Assert(PokemonNameToId(u8"nidoran♀"), "NIDORAN_FEMALE");
    Assert(PokemonNameToId("Nidoran Female"), "NIDORAN_FEMALE");
    Assert(PokemonNameToId(u8"nidoran♂"), "NIDORAN_MALE");
    Assert(PokemonNameToId("Nidoran Male"), "NIDORAN_MALE");
    Assert(PokemonNameToId("Farfetch'd"), "FARFETCHD");
    Assert(PokemonNameToId("Mr. Mime"), "MR_MIME");
    Assert(PokemonNameToId("mr mime"), "MR_MIME");
    Assert(PokemonNameToId("Ho-Oh"), "HO_OH");
    Assert(PokemonNameToId("ho oh"), "HO_OH");

    Assert(PokemonIdToName("nidoran_female"), u8"Nidoran♀");
    Assert(PokemonIdToName("NIDORAN_MALE"), u8"Nidoran♂");
    Assert(PokemonIdToName("FARFETCHD"), "Farfetch'd");
    Assert(PokemonIdToName("MR_MIME"), "Mr. Mime");
    Assert(PokemonIdToName("HO_OH"), "Ho-Oh");

    return EXIT_SUCCESS;
}