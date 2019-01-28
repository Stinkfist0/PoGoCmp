//#include "../src/PoGoCmp/PoGoCmp.h"
#include "../src/Lib/PoGoDb.h"

#include <cstdlib>
#include <iostream>
#include <locale>

//! @todo Unicode output
#define AssertNameToId(a, b) \
if (PokemonNameToId((a)) != (b)) \
{ std::cerr << "Name '" << a << "' did not map into ID '" << b << "'\n"; return EXIT_FAILURE; }

#define AssertIdToName(a, b) \
if (PokemonIdToName((a)) != (b)) \
{ std::cerr << "ID '" << a << "' did not map into name '" << b << "'\n"; return EXIT_FAILURE; }

int main(/*int argc, char **argv*/)
{
    using namespace PoGoCmp;
    // Would be nice to use e.g. u8"Nidoran♀" here but unfortunately not possible currently
    // on MSVC without changing to source file encoding to UTF-16 LE BOM.
    auto nidoranFemale = PoGoCmp::NidoranFemaleName;
    nidoranFemale[0] = std::tolower(nidoranFemale[0], {});
    AssertNameToId(nidoranFemale, "NIDORAN_FEMALE");
    AssertNameToId("Nidoran Female", "NIDORAN_FEMALE");
    AssertNameToId(PoGoCmp::NidoranMaleName, "NIDORAN_MALE");
    AssertNameToId("Nidoran Male", "NIDORAN_MALE");
    AssertNameToId("Farfetch'd", "FARFETCHD");
    AssertNameToId("Mr. Mime", "MR_MIME");
    AssertNameToId("mr mime", "MR_MIME");
    AssertNameToId("Ho-Oh", "HO_OH");
    AssertNameToId("ho oh", "HO_OH");
    AssertNameToId("Unown !", "UNOWN_EXCLAMATION_POINT");
    AssertNameToId("Unown ?", "UNOWN_QUESTION_MARK");
    AssertNameToId("spinda 1", "SPINDA_00");
    AssertNameToId("SPINDA 2", "SPINDA_01");
    AssertNameToId("Spinda 3", "SPINDA_02");
    AssertNameToId("Spinda 4", "SPINDA_03");
    AssertNameToId("Spinda 5", "SPINDA_04");
    AssertNameToId("Spinda 6", "SPINDA_05");
    AssertNameToId("Spinda 7", "SPINDA_06");
    AssertNameToId("Spinda 8", "SPINDA_07");
    AssertNameToId("porygonz", "PORYGON_Z");
    AssertNameToId("porygon z", "PORYGON_Z");
    AssertNameToId("mime jr", "MIME_JR");
    AssertNameToId("mime jr.", "MIME_JR");

    AssertIdToName("nidoran_female", PoGoCmp::NidoranFemaleName);
    AssertIdToName("NIDORAN_MALE", PoGoCmp::NidoranMaleName);
    AssertIdToName("FARFETCHD", "Farfetch'd");
    AssertIdToName("MR_MIME", "Mr. Mime");
    AssertIdToName("HO_OH", "Ho-Oh");
    AssertIdToName("UNOWN_EXCLAMATION_POINT", "Unown !");
    AssertIdToName("UNOWN_QUESTION_MARK", "Unown ?");
    AssertIdToName("SPINDA_00", "Spinda 1");
    AssertIdToName("SPINDA_01", "Spinda 2");
    AssertIdToName("SPINDA_02", "Spinda 3");
    AssertIdToName("SPINDA_03", "Spinda 4");
    AssertIdToName("SPINDA_04", "Spinda 5");
    AssertIdToName("SPINDA_05", "Spinda 6");
    AssertIdToName("SPINDA_06", "Spinda 7");
    AssertIdToName("SPINDA_07", "Spinda 8");
    AssertIdToName("MIME_JR", "Mime Jr.");
    AssertIdToName("PORYGON_Z", "Porygon Z");

    if (PoGoCmp::PokemonByIdName("SPINDA_08").number != 0)
    {
        std::cerr << "SPINDA_08 exists but has no name-ID mapping\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
