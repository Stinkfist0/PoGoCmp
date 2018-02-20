#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../PoGoCmp/PoGoCmp.h"
#include "../ThirdParty/nlohmann/json/json.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <regex>
#include <iomanip>
#include <set>
#include <ctime>

struct PokemonSpecieStr
{
    uint16_t number;
    uint16_t baseAtk;
    uint16_t baseDef;
    uint16_t baseSta;
    std::string name;
    std::string type;
    std::string type2;
};

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Output path missing. Usage: PoGoCmpDbGen <inputJsonPath> <outputhPath>\n";
        return EXIT_FAILURE;
    }

    std::string inputPath{ argv[1] };
    std::ifstream input{ inputPath };
    if (!input.is_open())
    {
        std::cerr << "Failed to open " << inputPath << "\n";
        return EXIT_FAILURE;
    }

    std::string outputPath{ argv[2] };
    std::ofstream output{ outputPath };
    if (!output.is_open())
    {
        std::cerr << "Failed to open " << outputPath << "\n";
        return EXIT_FAILURE;
    }

    std::map<uint16_t, PokemonSpecieStr> pokemonTable;
    std::set<std::string> pokemonTypes;

    using namespace nlohmann;
    try
    {
        json jsonInput;
        input >> jsonInput;

        std::string timestampMs = jsonInput["timestampMs"];
        auto timestampS = (time_t)std::stoll(timestampMs.c_str()) / 1000;
        auto tm = std::localtime(&timestampS);
        std::cout << "Input file's timestamp " << std::put_time(tm, "%c") << "\n";

        const std::regex idPattern{"V(\\d{4})_POKEMON_(\\w+)"}; // e.g. "V0122_POKEMON_MR_MIME"
        const std::regex typePattern{"POKEMON_TYPE_(\\w+)"}; // e.g. "POKEMON_TYPE_PSYCHIC"

        for (auto& itemTemplate : jsonInput["itemTemplates"])
        {
            std::string templateId = itemTemplate["templateId"];
            std::smatch matches;
            if (std::regex_match(templateId, matches, idPattern))
            {
                // Defer writing the actual Pokemon data after the enums are written.
                const auto& settings = itemTemplate["pokemonSettings"];
                PokemonSpecieStr pkm;
                pkm.number = (uint16_t)std::stoi(matches[1]);
                pkm.name = matches[2];
                pkm.baseAtk = settings["stats"]["baseAttack"];
                pkm.baseDef = settings["stats"]["baseDefense"];
                pkm.baseSta = settings["stats"]["baseStamina"];
                // Primary type
                std::string type = settings["type"];
                std::regex_match(type, matches, typePattern);
                pokemonTypes.insert(matches[1]);
                pkm.type = matches[1];
                // Secondary type, if applicable
                auto it = settings.find("type2");
                if (it != settings.end())
                {
                    std::string type2 = *it;
                    std::regex_match(type2, matches, typePattern);
                    pokemonTypes.insert(matches[1]);
                    pkm.type2 = matches[1];
                }

                pokemonTable[pkm.number] = pkm;
            }
        }
    }
    catch (const detail::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    output <<
R"(/**
    @file FILENAME
    @brief C++ API for PoGoCmp. For C one can use PoGoCmp.h. */
#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <map>
#ifndef _WIN32
#include <strings.h> // strcasecmp()
#endif

namespace PoGoCmp {

)";

    const std::string indent{"    "};
    const std::string typeNone{ "NONE" };
    output << "enum class PokemonType : uint8_t {\n";
    output << indent << typeNone << ",\n";
    for (auto type : pokemonTypes)
    {
        output << indent << type << ",\n";
    }
    output << "};\n\n";

    // TODO EnumToString and StringToEnum functions

    output <<
R"(struct PokemonSpecie
{
    /// Pokedex number.
    uint16_t number;
    /// Base attack.
    uint16_t baseAtk;
    /// Base defence.
    uint16_t baseDef;
    /// Base stamina (a.k.a. HP).
    uint16_t baseSta;
    /// Pokemon's specie name, uppercase.
    /// Punctuation and other "special" characters are replaced with underscores.
    std::string name;
    /// Primary type.
    PokemonType type;
    /// Secondary type, if applicable.
    PokemonType type2;
};

)";

    auto writePokemon = [&](const PokemonSpecieStr& pkm)
    {
        output << "{ " << pkm.number << ", " << pkm.baseAtk << ", " << pkm.baseDef
            << ", " << pkm.baseSta << ", " << std::quoted(pkm.name) << ", PokemonType::" << pkm.type
            << ", PokemonType::" << (pkm.type2.empty() ? typeNone : pkm.type2) << " }";
    };

    //auto writePokemonMap = [&](const auto& key, const PokemonSpecieStr& pkm)
    //{
    //    output << indent << "{ " << key << ", " << writePokemon(pkm) << " },\n";
    //};

    //output << "static const std::map<uint16_t, PokemonSpecie> PokemonByNumber {\n";
    output << "/// Pokedex number - 1 can be used as the index to the array.\n";
    // NOTE double-brace syntax needed for the array's initializer list ctor
    output << "static const std::array<PokemonSpecie, " << pokemonTable.size() << "> PokemonByNumber {{\n";
    for (const auto& it : pokemonTable)
    {
        output << indent;
        writePokemon(it.second);
        output << ",\n";
        //writePokemonMap(it.second.number, it.second);
    }
    output << "}};\n";

    output <<
R"(
struct StringLessThanI
{
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
#ifdef _WIN32
        return _stricmp(lhs.c_str(), rhs.c_str()) < 0;
#else
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
#endif
    }
};

)";

    output << "static const std::map<std::string, PokemonSpecie, StringLessThanI> PokemonByName {\n";
    for (const auto& it : pokemonTable)
    {
        //writePokemon(std::quoted(it.second.name), it.second);
        output << indent << "{ " << std::quoted(it.second.name) << ", PokemonByNumber[" << it.first - 1 << "] },\n" ;
    }
    output << "};\n\n";

    output << "}\n";
}
