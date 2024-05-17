#pragma once

#include <iostream>

#include "StoiDecorator.hpp"

class Time
{
public:
    Time();
    Time(int hours, int minutes);
    
    bool operator<(const Time& rightOperand);
    Time operator+(const Time& rightOperand);
    Time operator-(const Time& rightOperand);
    void StringToTime(std::string);

    std::string GetString();
    int GetHours();
    int GetMinutes();
private:
    int p_hours;
    int p_minutes;
};

