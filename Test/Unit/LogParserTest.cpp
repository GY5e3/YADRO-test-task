#include "LogParserTest.hpp"

TEST_F(LogParserTest, Парсинг_заголовка_лога)
{
	std::vector<std::string> negativeTestCases{"", "-1", "1a", "1 1", "1", "1\n12:0018:00", "1\n18:00 12:00", "1\n12:00 18:00",
											   "1\n12:00 18:00\n", "1\n12:00 18:00\n-1"};
	for (const auto testCase : negativeTestCases)
	{
		std::ofstream testLogFile("test_log_invalid.txt");
		testLogFile << testCase;
		testLogFile.close();

		std::ostringstream newCout;
		std::streambuf *oldCout = std::cout.rdbuf();
		std::cout.rdbuf(newCout.rdbuf());

		logParser = LogParser("test_log_invalid.txt");

		std::cout.rdbuf(oldCout);
		std::string result = newCout.str();

		EXPECT_THROW(ParseHeader(), std::invalid_argument);
	}
}
TEST_F(LogParserTest, Парсинг_события_в_теле_лога)
{
	std::vector<std::string> negativeTestCases{"\n",
											   "02:00 12 client1",
											   "32:00 12 client1",
											   "02:00 1 CLIENT",
											   "12:00 1 client1\n11:59 1 client2",
											   "12:00 ab client1",
											   "12:00 1 client1\n13:00 2 client1 -1",
											   "10:00 2 client1\n"};
	for (const auto testCase : negativeTestCases)
	{
		logParser = LogParser("test_log_invalid.txt");
		SetHeaderProperties();
		std::ofstream testLogFile("test_log_invalid.txt");
		testLogFile << testCase;
		testLogFile.close();

		std::ostringstream newCout;
		std::streambuf *oldCout = std::cout.rdbuf();
		std::cout.rdbuf(newCout.rdbuf());

		EXPECT_THROW(ParseBody(), std::invalid_argument);

		std::cout.rdbuf(oldCout);
		std::string result = newCout.str();
	}
}
TEST_F(LogParserTest, Обработка_события_из_тела_лога)
{
	std::vector<std::pair<std::string, std::string>> positiveTestCases{
		{"", ""},

		{"10:00 1 client1\n",
		 "10:00 1 client1\n"},

		{"08:00 1 client1",
		 "08:00 1 client1\n08:00 13 NotOpenYet\n"},

		{"10:00 1 client1\n11:00 1 client1\n",
		 "10:00 1 client1\n11:00 1 client1\n11:00 13 YouShallNotPass\n"},

		{"10:00 2 client1 2\n",
		 "10:00 2 client1 2\n10:00 13 ClientUnknown\n"},

		{"10:00 1 client1\n11:00 2 client1 2\n12:00 1 client2\n13:00 2 client2 2",
		 "10:00 1 client1\n11:00 2 client1 2\n12:00 1 client2\n13:00 2 client2 2\n13:00 13 PlaceIsBusy\n"},

		{"10:00 1 client1\n10:00 3 client1\n",
		 "10:00 1 client1\n10:00 3 client1\n10:00 13 ICanWaitNoLonger!\n"},

		{"10:00 1 client1\n11:00 2 client1 2\n12:00 3 client1\n",
		 "10:00 1 client1\n11:00 2 client1 2\n12:00 3 client1\n12:00 13 ICanWaitNoLonger!\n"},

		{"08:00 4 client1",
		 "08:00 4 client1\n08:00 13 ClientUnknown\n"}
	};

	for (const auto testCase : positiveTestCases)
	{
		std::ofstream testLogFile("unit_test.txt");
		testLogFile << testCase.first;
		testLogFile.close();

		logParser = LogParser("unit_test.txt");
		SetHeaderProperties();

		std::ostringstream newCout;
		std::streambuf *oldCout = std::cout.rdbuf();
		std::cout.rdbuf(newCout.rdbuf());
		ParseBody();
		std::cout.rdbuf(oldCout);
		std::string result = newCout.str();

		EXPECT_EQ(result, testCase.second);
	}
}