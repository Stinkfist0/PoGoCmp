#include "../src/PoGoCmp/PoGoCmp.h"

#include <cstdlib>

int main(/*int argc, char **argv*/)
{
    return PoGoCmpVersion() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
