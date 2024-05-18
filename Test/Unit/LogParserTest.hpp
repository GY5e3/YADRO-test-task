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
    void ParseHeader()
    {
        logParser.parseHeader();
    }

protected:
    LogParser logParser;
};