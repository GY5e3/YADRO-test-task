#pragma once

#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <streambuf>

#include "../../Core/LogParser.hpp"

class LogParserTest : public testing::Test
{
public:
protected:
    LogParser logParser;
    void SetHeaderProperties() {
        logParser.p_logLinesCounter = 3;
        logParser.p_workTimeBegin.StringToTime("09:00");
        logParser.p_workTimeEnd.StringToTime("19:00");
        logParser.p_freeGameTablesCount = 3;
        logParser.p_gameTables.resize(logParser.p_freeGameTablesCount, GameTable(10));
    };
    void ParseBody()
    {
        logParser.parseBody();
    }
    void ParseHeader()
    {
        logParser.parseHeader();
    }
};