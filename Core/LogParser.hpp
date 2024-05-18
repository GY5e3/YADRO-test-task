#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <algorithm>

#include "GameTable.hpp"
#include "Time.hpp"
#include "StoiDecorator.hpp"
#include "ClientNameParser.hpp"
#include "FastQueue.hpp"

//#include "../Test/Unit/LogParserTest.hpp"



const int GAME_TABLE_IS_UNDEFINED = -1;
const int LOG_HEADER_END = 4;
const int MIN_LOG_LINE_ARGUMENTS = 3;
const int MAX_LOG_LINE_ARGUMENTS = 4;

struct ClientSession
{
public:
    int gameTableNumber;
    Time startSessionTime;

    ClientSession() : gameTableNumber(GAME_TABLE_IS_UNDEFINED) {}

    ClientSession(int gameTableNumber, Time startSessionTime) : gameTableNumber(gameTableNumber),
                                                                startSessionTime(startSessionTime) {}
};

enum IncomingEventID
{
    ClientHasCome = 1,
    ClientTakeGameTable = 2,
    ClientIsWaiting = 3,
    ClientHasLeft = 4,
};
enum OutgoingEventID
{
    ClientHasLeftForced = 11,
    ClientFromQueueTakeGameTable = 12,
    EventError = 13
};

class LogParser
{
public:
    friend class LogParserTest;

    LogParser();
    LogParser(const std::string &filename);

    int Execute();

private:
    void startGameSession(const std::string &currentClient, int currentGameTable, Time &currentTime);
    void endGameSession(const std::string &currentClient, Time &currentTime);
    void parseHeader();
    void parseBody();
    void parseEvent(const std::vector<std::string> &tokens, Time &previousTime);
    void handleClientHasCome(const std::string &currentClient, Time &currentTime);
    void handleClientTakeGameTable(const std::vector<std::string> &tokens, const std::string &currentClient, Time &currentTime);
    void handleClientIsWaiting(const std::string &currentClient, Time &currentTime);
    void handleClientHasLeft(const std::string &currentClient, Time &currentTime);

    std::ifstream p_file;

    int p_logLinesCounter; // Счётчик строк в логе
    Time p_workTimeBegin;  // Время открытия компьютерного клуба
    Time p_workTimeEnd;    // Время закрытия компьютерного клуба

    std::vector<GameTable> p_gameTables; //Перечень всех игровых столов
    int p_freeGameTablesCount;           //Количество незанятых игровых столов
    std::unordered_map<std::string, ClientSession> p_clientInfo; //Отображение для клиента и данных о его игровой сессии
    FastQueue p_queueClients; // Очередь клиентов, ожидающих освобождения любого игрового стола

    StoiDecorator stoi_decorator; 
    ClientNameParser name_parser;
};