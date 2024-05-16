#pragma once

#include <iostream>

class SafeStoiDecorator
{
public:
    int operator()(const std::string &str);
};
