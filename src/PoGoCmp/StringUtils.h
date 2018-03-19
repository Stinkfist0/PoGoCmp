/**
    @file StringUtils.h
    @brief String utilities. */
#pragma once

#include <string>
#include <cstring>
#include <locale>

namespace StringUtils
{

static inline void SnakeCaseToTitleCase(std::string &str)
{
    if (str.empty()) return;
    const auto& cLocale = std::locale::classic();
    size_t i = 0;
    str[i] = std::toupper(str[i], cLocale);
    for (++i; i < str.length(); ++i)
    {
        if (str[i] == '_') str[i] = ' ';
        else if (str[i - 1] == ' ') str[i] = std::toupper(str[i], cLocale);
        else str[i] = std::tolower(str[i], cLocale);
    }
}

static inline std::string SnakeCaseToTitleCase(const std::string &str)
{
    if (str.empty()) return str;
    auto copy = str;
    SnakeCaseToTitleCase(copy);
    return copy;
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
