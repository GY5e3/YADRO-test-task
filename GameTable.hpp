#pragma once

#include <iostream>

#include "Time.hpp"

class GameTable
{
public:
    int pricePerHour;

    GameTable(int);
    std::string GetString();
    void AddDeltaTimeAndProfit(Time);
    void AddClient(const std::string &);
    void RemoveClient();
    bool IsBusy();

private:
    bool p_isBusy;
    int p_profit;
    Time p_busyTime;
    std::string p_client;
};
