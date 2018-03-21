/**
    @file   ProcessUtils.cpp
    @brief  Process/IO utilities. */

#include "Utf8.h"

#ifdef WIN32
#include <cwchar>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include "MinimalWin32.h"
#undef NOUSER   // All USER defines and routines
#undef NONLS    // All NLS defines and routines
#undef NOMSG    // typedef MSG and associated routines
#include <windows.h> // WideCharToMultiByte & MultiByteToWideChar
#include <shellapi.h> // CommandLineToArgvW
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <strings.h> // strcasecmp()
#include <locale>
#include <codecvt>
#endif

namespace Utf8
{

String FromUtf16(const std::wstring& wstr)
{
#ifdef WIN32
    std::string utf8;
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (requiredSize > 0)
    {
        std::vector<char> buffer(requiredSize);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer.data(), requiredSize, nullptr, nullptr);
        utf8.assign(buffer.begin(), buffer.end() - 1);
    }
    return utf8;
#else
    // Could use this also on Windows, but quick tests gave the impression that this is significantly slower
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
#endif
}

std::wstring ToUtf16(const String& str)
{
#ifdef WIN32
    std::wstring utf16;
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (requiredSize > 0)
    {
        std::vector<wchar_t> buffer(requiredSize);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer.data(), requiredSize);
        utf16.assign(buffer.begin(), buffer.end() - 1);
    }
    return utf16;
#else
    // Could use this also on Windows, but quick tests gave the impression that this is significantly slower
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
#endif
}

int EnableUnicodeStdIo()
{
#ifdef WIN32
    int res = -1;
    for (auto stream : { stdout, stderr, stdin })
    {
        res = _setmode(_fileno(stream), _O_U16TEXT);
        if (res == -1) return res;
    }
    return res;
#else
    /// @todo Should do the trick?  Test!
    std::locale::global(std::locale(""));
    return 0;
#endif
}

void Print(const String& str, OutputStream stream)
{
    auto out = stream == OutputStream::Err ? stderr : stdout;
#ifdef WIN32
    const auto wstr = ToUtf16(str);
    // Note: could use GetConsoleMode(), it should give more reliable results, especially for stdin.
    if (_isatty(_fileno(out)) != 0) // no output redirection
    {
        HANDLE handle = GetStdHandle(stream == OutputStream::Err ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
        if (handle == INVALID_HANDLE_VALUE)
            return;
        DWORD charsWritten;
        WriteConsole(handle, wstr.c_str(), wstr.size(), &charsWritten, nullptr);
    }
    else // Output redirected
    {
        std::fwprintf(out, L"%s", wstr.c_str()); // Doesn't work perfectly (limited to UCS-2?).
    }
#else
    std::fprintf(out, "%s", str.c_str());
#endif
}

void PrintLine(const String& str, OutputStream stream)
{
    Print(str + "\n", stream);
}

std::vector<String> ParseArguments(int argc, char** argv, bool skipFirst)
{
#ifdef WIN32
    std::vector<String> args;
    (void)argv;
    PWSTR *argvw = CommandLineToArgvW(GetCommandLine(), &argc);
    for (int i = skipFirst ? 1 : 0; i < argc; ++i)
    {
        args.push_back(FromUtf16(std::wstring{ argvw[i] }));
    }
    LocalFree(argvw);
    return args;
#else
    return { argv + (skipFirst ? 1 : 0), argv + argc };
#endif
}

int CompareI(ByteArray str1, ByteArray str2)
{
#ifdef _WIN32
    const auto wstr1 = ToUtf16(str1);
    const auto wstr2 = ToUtf16(str2);
    return StrCmpIW(wstr1.c_str(), wstr2.c_str());
#else
    /// @todo strcasecmp() probably doesn't support UTF-8?
    return strcasecmp(str1, str2);
#endif
}

}
