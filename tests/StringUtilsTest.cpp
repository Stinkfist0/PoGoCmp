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
    AssertTrue(strSplit.size() == 3, "Split(SplitOptions::RemoveEmptyEntries) not working.");
    strSplit = Split(str, ",", SplitOptions::KeepEmptyEntries);
    AssertTrue(strSplit.size() == 4, "Split(SplitOptions::KeepEmptyEntries) not working.");
    auto strJoined = Join(strSplit, ",");
    AssertTrue(str == strJoined, "String not the same after split and join.");

    std::string caseStr{"snake_case_123_!"};
    auto copy = SnakeCaseToTitleCaseCopy(caseStr);
    AssertTrue(copy == "Snake Case 123 !", "SnakeCaseToTitleCaseCopy not working.");
    SnakeCaseToTitleCase(caseStr);
    AssertTrue(caseStr == copy, "SnakeCaseToTitleCaseCopy and SnakeCaseToTitleCase yield different results.");

    AssertTrue(IsNumber(strSplit[2]), strSplit[2] + " should be a number.");
    const auto pi = 3.14159265358979323846f;
    const auto piFromStr = LexicalCast<float>(strSplit[2]);
    AssertTrue(MathUtils::Equals(pi, pi), strSplit[2] + " is not equal to pi");

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
