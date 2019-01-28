//#include "../src/PoGoCmp/PoGoCmp.h"
#include "../src/Lib/PoGoDb.h"

#include <cstdlib>
#include <iostream>
#include <locale>

//! @todo Unicode output, AssertNameToId(name, id), AssertIdToName(id, name)
#define Assert(a, b) \
if ((a) != (b)) \
{ std::cerr << "'" << a << "' did not map into '" << b << "'\n"; return EXIT_FAILURE; }

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
    Assert(PokemonNameToId("Unown !"), "UNOWN_EXCLAMATION_POINT");
    Assert(PokemonNameToId("Unown ?"), "UNOWN_QUESTION_MARK");
    Assert(PokemonNameToId("spinda 1"), "SPINDA_00");
    Assert(PokemonNameToId("SPINDA 2"), "SPINDA_01");
    Assert(PokemonNameToId("Spinda 3"), "SPINDA_02");
    Assert(PokemonNameToId("Spinda 4"), "SPINDA_03");
    Assert(PokemonNameToId("Spinda 5"), "SPINDA_04");
    Assert(PokemonNameToId("Spinda 6"), "SPINDA_05");
    Assert(PokemonNameToId("Spinda 7"), "SPINDA_06");
    Assert(PokemonNameToId("Spinda 8"), "SPINDA_07");
    Assert(PokemonNameToId("porygonz"), "PORYGON_Z");
    Assert(PokemonNameToId("porygon z"), "PORYGON_Z");
    Assert(PokemonNameToId("mime jr"), "MIME_JR");
    Assert(PokemonNameToId("mime jr."), "MIME_JR");

    Assert(PokemonIdToName("nidoran_female"), PoGoCmp::NidoranFemaleName);
    Assert(PokemonIdToName("NIDORAN_MALE"), PoGoCmp::NidoranMaleName);
    Assert(PokemonIdToName("FARFETCHD"), "Farfetch'd");
    Assert(PokemonIdToName("MR_MIME"), "Mr. Mime");
    Assert(PokemonIdToName("HO_OH"), "Ho-Oh");
    Assert(PokemonIdToName("UNOWN_EXCLAMATION_POINT"), "Unown !");
    Assert(PokemonIdToName("UNOWN_QUESTION_MARK"), "Unown ?");
    Assert(PokemonIdToName("SPINDA_00"), "Spinda 1");
    Assert(PokemonIdToName("SPINDA_01"), "Spinda 2");
    Assert(PokemonIdToName("SPINDA_02"), "Spinda 3");
    Assert(PokemonIdToName("SPINDA_03"), "Spinda 4");
    Assert(PokemonIdToName("SPINDA_04"), "Spinda 5");
    Assert(PokemonIdToName("SPINDA_05"), "Spinda 6");
    Assert(PokemonIdToName("SPINDA_06"), "Spinda 7");
    Assert(PokemonIdToName("SPINDA_07"), "Spinda 8");
    Assert(PokemonIdToName("MIME_JR"), "Mime Jr.");
    Assert(PokemonIdToName("PORYGON_Z"), "Porygon Z");

    if (PoGoCmp::PokemonByIdName("SPINDA_08").number != 0)
    {
        std::cerr << "SPINDA_08 exists but has no name-ID mapping\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
