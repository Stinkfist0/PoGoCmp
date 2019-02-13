#include "../src/Lib/StringUtils.h"
#include "../src/Lib/MathUtils.h"

#include <iostream>

#define AssertTrue(exp, msg) \
if (!(exp)) { std::cerr << (msg) << '\n'; return EXIT_FAILURE; }

#define AssertFalse(exp, msg) \
if ((exp)) { std::cerr << (msg) << '\n'; return EXIT_FAILURE; }

int main()
{
    using namespace StringUtils;

    const std::string str{"yksi, two, 3.141592635359,"}; // empty entry at the end
    auto strSplit = Split(str, ",", SplitOptions::RemoveEmptyEntries);
    AssertTrue(strSplit.size() == 3, "Split(RemoveEmptyEntries) not working.");
    strSplit = Split(str, ",", SplitOptions::KeepEmptyEntries);
    AssertTrue(strSplit.size() == 4, "Split(KeepEmptyEntries) not working.");
    auto strJoined = Join(strSplit, ",");
    AssertTrue(str == strJoined, "String not the same after split and join.");
    AssertTrue(
        Split("", ",", SplitOptions::RemoveEmptyEntries).empty(),
        "Split(RemoveEmptyEntries) for an empty string yields a non-empty array"
    );
    AssertTrue(
        Split("", ",", SplitOptions::KeepEmptyEntries).empty(),
        "Split(KeepEmptyEntries) for an empty string yields a non-empty array"
    );
    AssertTrue(
        Split(str, "", SplitOptions::RemoveEmptyEntries).front() == str,
        "Split(RemoveEmptyEntries) with an empty delimeter does not result a vector with the original string"
    );
    AssertTrue(
        Split(str, "", SplitOptions::KeepEmptyEntries).front() == str,
        "Split(KeepEmptyEntries) with an empty delimeter does not result a vector with the original string"
    );

    std::string caseStr{"snake_case_123_!"};
    auto copy = SnakeCaseToTitleCaseCopy(caseStr);
    AssertTrue(copy == "Snake Case 123 !", "SnakeCaseToTitleCaseCopy not working.");
    SnakeCaseToTitleCase(caseStr);
    AssertTrue(caseStr == copy, "SnakeCaseToTitleCaseCopy and SnakeCaseToTitleCase yield different results.");

    AssertTrue(IsNumber(strSplit[2]), strSplit[2] + " should be a number.");
    const auto pi = 3.14159265358979323846f;
    const auto piFromStr = LexicalCast<float>(strSplit[2]);
    AssertTrue(MathUtils::Equals(pi, pi), strSplit[2] + " is not equal to pi");
    try
    {
        LexicalCast<float>("asdf");
        std::cerr << "string-float LexicalCast doesn't throw on invalid input\n";
        return EXIT_FAILURE;
    }
    catch (... /*const std::runtime_error&*/) {}

    try
    {
        LexicalCast<std::string>(pi);
    }
    catch (... /*const std::runtime_error&*/)
    {
        std::cerr << "float-string LexicalCast throws on valid input\n";
        return EXIT_FAILURE;
    }


    AssertTrue(Concat("Hello", ",", " ", "World", "!") == "Hello, World!", "Concat not working.");

    const std::string whitespaceStr{"\t    white    space      \t\t"};
    const std::string lTrimmed{"white    space      \t\t"};
    const std::string rTrimmed{"\t    white    space"};
    const std::string trimmed{"white    space"};
    AssertTrue(LTrimCopy(whitespaceStr) == lTrimmed, "LTrimCopy not working.");
    AssertTrue(RTrimCopy(whitespaceStr) == rTrimmed, "RTrimCopy not working.");
    AssertTrue(TrimCopy(whitespaceStr) == trimmed, "TrimCopy not working.");

    copy = whitespaceStr;
    AssertTrue(LTrim(copy) == lTrimmed, "LTrim not working.");
    AssertTrue(RTrim(copy) == trimmed, "RTrim not working.");
    auto copy2 = whitespaceStr;
    AssertTrue(Trim(copy2) == copy, "Trim not working.");

    return EXIT_SUCCESS;
}
