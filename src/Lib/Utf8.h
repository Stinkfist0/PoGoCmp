/**
    @file Utf8.h
    @brief UTF-8 Process/IO/String utilities. */
#pragma once

#include "PoGoCmpApi.h"

#include <string>
#include <vector>
#include <fstream>

namespace Utf8
{

//! Used to denote that the string can contain UTF-8 encoded data.
using String = std::string;
//! Used to denote that the char array can contain UTF-8 encoded data.
using ByteArray = const char*;

enum class OutputStream : bool { Out, Err };

//! wstring assumed to contain UTF-16 on Windows and UTF-32 on other platforms.
POGOCMP_API String FromWString(const std::wstring& wstr);
//! wstring assumed to contain UTF-16 on Windows and UTF-32 on other platforms.
POGOCMP_API std::wstring ToWString(const String& str);

//! Parses argc/argv into a UTF-8 encoded string vector.
//! On Windows argc/argv is ignored and CommandLineToArgvW/GetCommandLine used instead.
//! On other platforms arguments are assumed to be already UTF-8 encoded.
//! @param skipFirst Should the first argument be skipped. This is the program invokation argument
//! and typically uninteresting.
POGOCMP_API std::vector<String> ParseArguments(int argc, char** argv, bool skipFirst = true);

//! Prints Unicode string to output stream.
//! On Window, in case the output is redirected, EnableUnicodeStdIo() may need to be called
//! in order to Unicode output to appear correctly.
POGOCMP_API void Print(const String& str, OutputStream stream = OutputStream::Out);

//! Prints Unicode string and appends a new line.
POGOCMP_API void PrintLine(const String& str, OutputStream stream = OutputStream::Out);

//! Case-insensitive UTF-8 string comparison.
POGOCMP_API int CompareI(ByteArray str1, ByteArray str2);

//! Enables Unicode I/O on stdout, stderr, and stdin, mostly needed for Windows.
//! @returns On Windows the previous mode, -1 if failed to change mode.
POGOCMP_API int EnableUnicodeStdIo();

#ifdef WIN32
// Utilize MSVC's fstream wchar ctors (non-standard).
// Idea from http://www.nubaria.com/en/blog/?p=289
class ifstream : public std::ifstream
{
public:
    ifstream() : std::ifstream() {}

    explicit ifstream(ByteArray filename, std::ios_base::openmode mode = std::ios_base::in) :
        std::ifstream(ToWString(filename), mode)
    {}

    explicit ifstream(const String& filename, std::ios_base::openmode mode = std::ios_base::in) :
        std::ifstream(ToWString(filename), mode)
    {}

    void open(ByteArray filename, std::ios_base::openmode mode = std::ios_base::in)
    {
        std::ifstream::open(ToWString(filename), mode);
    }

    void open(const String& filename, std::ios_base::openmode mode = std::ios_base::in)
    {
        std::ifstream::open(ToWString(filename), mode);
    }
};

class ofstream : public std::ofstream
{
public:
    ofstream() : std::ofstream() {}

    explicit ofstream(ByteArray filename, std::ios_base::openmode mode = std::ios_base::out) :
        std::ofstream(ToWString(filename), mode)
    {}

    explicit ofstream(const String& filename, std::ios_base::openmode mode = std::ios_base::out) :
        std::ofstream(ToWString(filename), mode)
    {}

    void open(ByteArray filename, std::ios_base::openmode mode = std::ios_base::out)
    {
        std::ofstream::open(ToWString(filename), mode);
    }

    void open(const String& filename, std::ios_base::openmode mode = std::ios_base::out)
    {
        std::ofstream::open(ToWString(filename), mode);
    }
};
#else
using std::ifstream;
using std::ofstream;
#endif
}
