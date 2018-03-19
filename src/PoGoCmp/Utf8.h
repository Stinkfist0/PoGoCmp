/**
    @file Utf8.h
    @brief UTF-8 Process/IO/String utilities. */
#pragma once

#include "PoGoCmpApi.h"

#include <string>
#include <vector>

namespace Utf8
{
/// Used to denote that the string can contain UTF-8 encoded data.
using String = std::string;
/// Used to denote that the char array can contain UTF-8 encoded data.
using ByteArray = const char*;

POGOCMP_API String FromUtf16(const std::wstring& wstr);

POGOCMP_API std::wstring ToUtf16(const String& str);

/// On Windows argc/argv is ignored and CommandLineToArgvW/GetCommandLine used instead.
POGOCMP_API std::vector<String> GetCommandLineUtf8(int argc, char** argv);

/// Prints Unicode string to stdout (error == false) or stderr (error == true).
POGOCMP_API void Print(const String& str, bool error = false);

/// Prints Unicode string and appends a new line.
POGOCMP_API void PrintLine(const String& str, bool error = false);

/// Case-insensitive UTF-8 string comparison.
POGOCMP_API int CompareI(ByteArray str1, ByteArray str2);

/// Enables Unicode output to std::wcout, std::wcerr, and std::wclog on.
/// @returns On Windows the previous mode, -1 if failed to change mode.
POGOCMP_API int EnableUnicodeStreamOutput();
}
