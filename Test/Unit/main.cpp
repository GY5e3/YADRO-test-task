#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

#include <streambuf>

#include "../../Core/LogParser.hpp"

TEST(LogParserTest, ParseLog_InvalidInput_MissingHeader)
{
    std::ofstream testLogFile("test_log_invalid.txt");
    testLogFile << "1abcd\n";
    testLogFile.close();
    std::string expectedOutput = "Line 1: stoi\n";

    LogParser parser("test_log_invalid.txt");

    std::ostringstream newCout;
    std::streambuf *oldCout = std::cout.rdbuf();
    std::cout.rdbuf(newCout.rdbuf());

    parser.Execute();

    std::cout.rdbuf(oldCout);
    std::cout << "I was here " +  newCout.str() << std::endl;
    EXPECT_EQ(newCout.str(), expectedOutput);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
