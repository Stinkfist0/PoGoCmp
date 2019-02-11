/**
    @file StringUtils.h
    @brief String utilities. */
#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <locale>
#include <sstream>
#include <algorithm>

namespace StringUtils
{

enum SplitOptions : bool { RemoveEmptyEntries, KeepEmptyEntries };

static inline std::vector<std::string> Split(const std::string &s, char delim, SplitOptions opts)
{
    std::vector<std::string> entries;
    std::istringstream stream{ s };
    for (std::string entry; std::getline(stream, entry, delim);)
    {
        if (opts == SplitOptions::KeepEmptyEntries || !entry.empty())
            entries.push_back(entry);
    }
   return entries;
}

static inline std::string Join(const std::vector<std::string> &strings, const std::string& delim)
{
    std::string ret;
    //! @todo
    //! numStrings = accumulate(strings.size)
    //! ret.reserve(numStrings + numStrings * delim.size())?
    const auto size = strings.size();
    for (size_t i = 0; i < size; ++i)
    {
        ret += strings[i];
        if (i < size - 1)
            ret  += delim;
    }
    return ret;
}

//! Converts "snake_case" or "SCREAMING_NAKE_CASE" to "Title Case", C locale.
static inline void SnakeCaseToTitleCase(std::string &str)
{
    if (str.empty()) return;
    const auto& cLocale = std::locale::classic();
    size_t i = 0;
    str[i] = std::toupper(str[i], cLocale);
    const auto size = str.size();
    for (++i; i < size; ++i)
    {
        if (str[i] == '_') str[i] = ' ';
        else if (str[i - 1] == ' ' || str[i - 1] == '-') str[i] = std::toupper(str[i], cLocale);
        else str[i] = std::tolower(str[i], cLocale);
    }
}
[[nodiscard]] static inline std::string SnakeCaseToTitleCaseCopy(std::string str)
{
    SnakeCaseToTitleCase(str);
    return str;
}

//! All non-alphanumeric characters are converted to underscores, C locale.
static inline void ToScreamingSnakeCase(std::string& str)
{
    const auto& cLocale = std::locale::classic();
    std::transform(
        str.begin(), str.end(), str.begin(),
        [&cLocale](auto c)
        {
            if (!std::isalnum(c, cLocale)) return '_';
            else return std::toupper(c, cLocale);
        }
    );
}
[[nodiscard]] static inline std::string ToScreamingSnakeCaseCopy(std::string str)
{
    ToScreamingSnakeCase(str);
    return str;
}

static inline bool IsNumber(const std::string& str)
{
    char* p;
    (void)std::strtod(str.c_str(), &p);
    return *p == 0;
}

// https://stackoverflow.com/a/18899027
namespace detail {

template<typename>
struct string_size_impl;

template<size_t N>
struct string_size_impl<const char[N]> {
    static constexpr size_t size(const char(&)[N]) { return N - 1; }
};

template<size_t N>
struct string_size_impl<char[N]> {
    static size_t size(char(&s)[N]) { return N ? std::strlen(s) : 0; }
};

template<>
struct string_size_impl<const char*> {
    static size_t size(const char* s) { return s ? std::strlen(s) : 0; }
};

template<>
struct string_size_impl<char*> {
    static size_t size(char* s) { return s ? std::strlen(s) : 0; }
};

template<>
struct string_size_impl<std::string> {
    static size_t size(const std::string& s) { return s.size(); }
};

template<typename String> size_t string_size(String&& s) {
    using noref_t = typename std::remove_reference<String>::type;
    using string_t = typename std::conditional<std::is_array<noref_t>::value,
        noref_t,
        typename std::remove_cv<noref_t>::type
    >::type;
    return string_size_impl<string_t>::size(s);
}

template<typename...>
struct concatenate_impl;

template<typename String>
struct concatenate_impl<String> {
    static size_t size(String&& s) { return string_size(s); }
    static void concatenate(std::string& result, String&& s) { result += s; }
};

template<typename String, typename... Rest>
struct concatenate_impl<String, Rest...> {
    static size_t size(String&& s, Rest&&... rest) {
        return string_size(s)
            + concatenate_impl<Rest...>::size(std::forward<Rest>(rest)...);
    }
    static void concatenate(std::string& result, String&& s, Rest&&... rest) {
        result += s;
        concatenate_impl<Rest...>::concatenate(result, std::forward<Rest>(rest)...);
    }
};

} // namespace detail

template<typename... Strings>
std::string Concatenate(Strings&&... strings) {
    std::string result;
    result.reserve(detail::concatenate_impl<Strings...>::size(std::forward<Strings>(strings)...));
    detail::concatenate_impl<Strings...>::concatenate(result, std::forward<Strings>(strings)...);
    return result;
}

template<typename... Strings>
std::string Concat(Strings&&... strings) { return Concatenate(strings...); }

// https://stackoverflow.com/a/25829233
inline std::string& LTrim(std::string& s, const char* t = " \t\n\r\f\v") { s.erase(0, s.find_first_not_of(t)); return s; }
inline std::string& RTrim(std::string& s, const char* t = " \t\n\r\f\v") { s.erase(s.find_last_not_of(t) + 1); return s; }
inline std::string& Trim(std::string& s, const char* t = " \t\n\r\f\v") { return LTrim(RTrim(s, t), t); }
inline std::string LTrimCopy(std::string s, const char* t = " \t\n\r\f\v") { return LTrim(s, t); }
inline std::string RTrimCopy(std::string s, const char* t = " \t\n\r\f\v") { return RTrim(s, t); }
inline std::string TrimCopy(std::string s, const char* t = " \t\n\r\f\v") { return Trim(s, t); }

} // namespace StringUtils
