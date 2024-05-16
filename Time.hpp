#pragma once

#include <iostream>

#include "SafeStoiDecorator.hpp"

class Time
{
public:
    Time();
    Time(int hours, int minutes);
    std::string GetString();
    bool operator<(const Time& rightOperand);
    Time operator+(const Time& rightOperand);
    Time operator-(const Time& rightOperand);
    void StringToTime(std::string);

    int GetHours();
    int GetMinutes();
private:
    int p_hours;
    int p_minutes;
};

