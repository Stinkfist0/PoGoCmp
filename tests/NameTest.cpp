//#include "../src/PoGoCmp/PoGoCmp.h"
#include "../src/Lib/PoGoDb.h"

#include <cstdlib>
#include <iostream>
#include <locale>

//! @todo Unicode output
#define Assert(a, b) \
if ((a) != (b)) \
{ std::cerr << "'" << a << "' did not map into '" << b << "'\n"; return EXIT_FAILURE; }
    #include <codecvt>

int main(/*int argc, char **argv*/)
{
    using namespace PoGoCmp;
    // Would be nice to use e.g. u8"Nidoran♀" here but unfortunately not possible currently
    // on MSVC without changing to source file encoding to UTF-16 LE BOM.
    auto nidoranFemale = PoGoCmp::NidoranFemaleName;
    nidoranFemale[0] = std::tolower(nidoranFemale[0], {});
    Assert(PokemonNameToId(nidoranFemale), "NIDORAN_FEMALE");
    Assert(PokemonNameToId("Nidoran Female"), "NIDORAN_FEMALE");
    Assert(PokemonNameToId(PoGoCmp::NidoranMaleName), "NIDORAN_MALE");
    Assert(PokemonNameToId("Nidoran Male"), "NIDORAN_MALE");
    Assert(PokemonNameToId("Farfetch'd"), "FARFETCHD");
    Assert(PokemonNameToId("Mr. Mime"), "MR_MIME");
    Assert(PokemonNameToId("mr mime"), "MR_MIME");
    Assert(PokemonNameToId("Ho-Oh"), "HO_OH");
    Assert(PokemonNameToId("ho oh"), "HO_OH");
    Assert(PokemonNameToId("porygonz"), "PORYGON_Z");
    Assert(PokemonNameToId("porygon z"), "PORYGON_Z");

    Assert(PokemonIdToName("nidoran_female"), PoGoCmp::NidoranFemaleName);
    Assert(PokemonIdToName("NIDORAN_MALE"), PoGoCmp::NidoranMaleName);
    Assert(PokemonIdToName("FARFETCHD"), "Farfetch'd");
    Assert(PokemonIdToName("MR_MIME"), "Mr. Mime");
    Assert(PokemonIdToName("HO_OH"), "Ho-Oh");
    Assert(PokemonIdToName("PORYGON_Z"), "Porygon Z");

    return EXIT_SUCCESS;
}
