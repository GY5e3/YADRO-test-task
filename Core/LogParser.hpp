#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <queue>

#include "GameTable.hpp"
#include "Time.hpp"
#include "StoiDecorator.hpp"
#include "ClientNameParser.hpp"

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


class LogParser {
public:
    LogParser(const std::string &filename);
    int Execute();

private:
    void startGameSession(const std::string &currentClient, int currentGameTable, Time &currentTime);
    void endGameSession(const std::string &currentClient, Time &currentTime);
    void parseHeader();
    void parseEvent(const std::vector<std::string> &tokens);
    void handleClientHasCome(const std::string &currentClient, Time &currentTime);
    void handleClientTakeGameTable(const std::vector<std::string> &tokens, const std::string &currentClient, Time &currentTime);
    void handleClientIsWaiting(const std::string &currentClient, Time &currentTime);
    void handleClientHasLeft(const std::string &currentClient, Time &currentTime);
    
    std::ifstream p_file;
    int p_logLinesCounter;
    int p_gameTablesCount;
    Time p_workTimeBegin;
    Time p_workTimeEnd;
    std::vector<GameTable> p_gameTables;
    int p_freeGameTablesCount;
    std::unordered_map<std::string, ClientSession> p_clientInfo;
    std::queue<std::string> p_queueClients;
    
    StoiDecorator stoi_decorator;
    ClientNameParser name_parser;
};