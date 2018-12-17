/**
    @file ProgramOptions.h
    @brief Utility classes for handling program options. */
#pragma once

#include "../Lib/StringUtils.h"
#include "../Lib/Utf8.h"

#include <algorithm>

//template <typename ValueType>
struct ProgramOption
{
    ProgramOption() {}
    //! type is deduced automatically from the shortName or longName.
    ProgramOption(const std::string& sn, const std::string& ln, const std::wstring& h) :
        shortName(sn), longName(ln), help(Utf8::FromWString(h))
    {
        type = IsArg(shortName) || IsArg(longName) ? Arg : Cmd;
    }
    //! Commands have no prefix, arguments have either "-" or "--" prefix.
    //! @todo could support also /-prefix, also e.g. would be nice to ignore/warn about e.g. ---prefix
    static bool IsArg(const std::string &str)
    {
        return str.find_first_of("-") == 0 && !StringUtils::IsNumber(str);
    }

    enum Type : bool { Cmd, Arg };

    Type type;
    std::string shortName;
    std::string longName;
    Utf8::String help;

    //!std::string longHelp; // when help <command> is used?
    //bool multiple; // are multiple same options supported
    // ValueType value
};

std::ostream& operator<< (std::ostream& out, const ProgramOption& opt)
{
    out << opt.shortName << (opt.shortName.empty() || opt.longName.empty() ? "" : ",") << opt.longName << ": " << opt.help;
    return out;
}

struct ProgamOptionMap
{
    using StringVector = std::vector<std::string>;
    //! (shortName, longName)
    //using NamePair = std::pair <std::string, std::string>;

    ProgamOptionMap(const StringVector& args) : args(args) {}
    //! @param argv The program invokation argument argv[0] is ignored.
    ProgamOptionMap(int argc, char **argv/*, const std::vector<ProgramOption>& options*/)
        : ProgamOptionMap({ argv + 1, argv + argc })
    {
        //for (const auto& opt : options)
        //{
        //    opts[std::make_pair(opt.shortName, opt.longName)] = OptionValue(opt.shortName, opt.longName);
        //}
    }

    bool HasOption(const std::string& shortName, const std::string& longName) const
    {
        return std::count_if(args.begin(), args.end(),
            [&](const auto& arg) { return arg == shortName || arg == longName; }) > 0;
    }

    bool HasOption(const std::string& name) const { return HasOption(name, name); }

    std::string OptionValue(const std::string& shortName, const std::string& longName) const
    {
        auto optIt = std::find_if(args.begin(), args.end(), [&](const auto& arg) {
            return arg == shortName || arg == longName;
        });
        auto valueIt = optIt != args.end() ? optIt + 1 : args.end();
        return IsValue(valueIt) ? *valueIt : "";
    }

    std::string OptionValue(const std::string& name) const { return OptionValue(name, name); }

    StringVector OptionValues(const std::string& shortName, const std::string& longName) const
    {
        StringVector ret;
        auto it = args.begin();
        while (it != args.end())
        {
            auto optIt = std::find_if(it, args.end(), [&](const auto& arg) {
                return arg == shortName || arg == longName;
            });
            auto valueIt = optIt != args.end() ? optIt + 1 : args.end();
            if (IsValue(valueIt))
                ret.push_back(*valueIt);
            it = valueIt;
            if (it != args.end())
                ++it;
        }
        return ret;
    }

    StringVector OptionValues(const std::string& name) const { return OptionValues(name, name); }

    //! @param valueIt Iterator to program option which should be considered a value.
    bool IsValue(StringVector::const_iterator it) const
    {
        if (it == args.begin() || it == args.end()) return false;
        auto prevIt = (it - 1);
        return (prevIt == args.begin() || ProgramOption::IsArg(*prevIt)) && !ProgramOption::IsArg(*it);
    }
    //! The original arguments
    std::vector<std::string> args;
    //! Program options and their values
    //std::map<NamePair, std::string> opts;
};

