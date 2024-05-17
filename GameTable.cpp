#include "GameTable.hpp"

GameTable::GameTable(int pricePerHour) : p_profit(0),
                                         p_isBusy(false),
                                         p_client(""),
                                         pricePerHour(pricePerHour) {}

std::string GameTable::GetString()
{
    return std::to_string(p_profit) + " " + p_busyTime.GetString();
}

void GameTable::AddDeltaTimeAndProfit(Time deltaTime)
{
    p_busyTime = p_busyTime + deltaTime;
    p_profit += (deltaTime.GetHours() + (deltaTime.GetMinutes() > 0)) * pricePerHour;
}

void GameTable::AddClient(const std::string &client)
{
    p_client = client;
    p_isBusy = true;
}

void GameTable::RemoveClient()
{
    p_client = "";
    p_isBusy = false;
}

bool GameTable::IsBusy()
{
    return p_isBusy;
}