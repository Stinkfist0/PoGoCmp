#include "PoGoCmp.h"
#include "PoGoCmpDb.h"

const char* PoGoCmpVersionString() { return "0.0.1"; }

size_t NumPokemonInDb() { return PoGoCmp::PokemonByNumber.size(); }
