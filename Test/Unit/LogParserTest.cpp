#include "LogParserTest.hpp"

TEST_F(LogParserTest, Парсинг_заголовка_лога)
{
	std::vector<std::pair<std::string, std::string>> testCases{{"1\n10:00 17:00\n30", ""},
															   {"1a", "Line 1: stoi\n"},
															   {"1 1", "Line 1: stoi\n"},
															   {"1", "Line 2: Incorrect log line format\n"},
															   {"1\n12:0018:00", "Line 2: Incorrect log line format\n"},
															   {"1\n18:00 12:00", "Line 2: WorkTimeEnd can't be less than workTimeBegin\n"},
															   {"1\n12:00 18:00", "Line 3: stoi\n"}};

	for (const auto pair : testCases)
	{
		std::ofstream testLogFile("test_log_invalid.txt");
		testLogFile << pair.first;
		testLogFile.close();

		std::ostringstream newCout;
		std::streambuf *oldCout = std::cout.rdbuf();
		std::cout.rdbuf(newCout.rdbuf());

		logParser = LogParser("test_log_invalid.txt");
		ParseHeader();

		std::cout.rdbuf(oldCout);
		std::string expected = pair.second;
		std::string result = newCout.str();
		EXPECT_EQ(expected, result);
	}
}