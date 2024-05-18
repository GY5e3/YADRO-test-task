#include "LogParser.hpp"

LogParser::LogParser() : p_file(""), p_logLinesCounter(0) {}

LogParser::LogParser(const std::string &filename) : p_file(filename), p_logLinesCounter(0)
{
    if (!p_file.is_open())
    {
        throw std::runtime_error("File could not be opened");
    }
}

int LogParser::Execute()
{
    try
    {
        parseHeader();

        std::cout << p_workTimeBegin.GetString() << std::endl;

        parseBody();

        std::vector<std::string> lastClients;
        for (const auto &lastClient : p_clientInfo)
        {
            lastClients.push_back(lastClient.first);
            if(p_clientInfo[lastClient.first].gameTableNumber != GAME_TABLE_IS_UNDEFINED)
                endGameSession(lastClient.first, p_workTimeEnd);
        }
        while (p_queueClients.size() > 0)
        {
            lastClients.push_back(p_queueClients.front());
            p_queueClients.pop();
        }
        std::sort(lastClients.begin(), lastClients.end());

        for (const auto i : lastClients)
        {
            std::cout << p_workTimeEnd.GetString() + " " << OutgoingEventID::ClientHasLeftForced << " " + i << std::endl;
        }
        std::cout << p_workTimeEnd.GetString() << std::endl;
        for (size_t i = 0; i < p_gameTables.size(); i++)
        {
            std::cout << i + 1 << " " << p_gameTables[i].GetString() << std::endl;
        }
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << "Line " + std::to_string(p_logLinesCounter) + ": " + e.what() << std::endl;
        return 1;
    }

    return 0;
}

void LogParser::startGameSession(const std::string &currentClient, int currentGameTable, Time &currentTime)
{
    p_clientInfo[currentClient].gameTableNumber = currentGameTable;
    p_clientInfo[currentClient].startSessionTime = currentTime;
    p_gameTables[currentGameTable].AddClient(currentClient);
    p_freeGameTablesCount--;
}

void LogParser::endGameSession(const std::string &currentClient, Time &currentTime)
{
    int index = p_clientInfo[currentClient].gameTableNumber;
    p_gameTables[index].AddDeltaTimeAndProfit(currentTime - p_clientInfo[currentClient].startSessionTime);
    p_gameTables[index].RemoveClient();
    p_clientInfo[currentClient].gameTableNumber = GAME_TABLE_IS_UNDEFINED;
    p_freeGameTablesCount++;
}

void LogParser::parseHeader()
{
    std::string line;

    std::getline(p_file, line);
    p_logLinesCounter++;

    int gameTablesCount = stoi_decorator(line);
    p_freeGameTablesCount = gameTablesCount;

    std::getline(p_file, line);
    p_logLinesCounter++;
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (iss >> token)
    {
        tokens.push_back(token);
    }
    if (tokens.size() != 2)
    {
        throw std::invalid_argument("Incorrect log line format");
    }
    p_workTimeBegin.StringToTime(tokens[0]);
    p_workTimeEnd.StringToTime(tokens[1]);
    if (p_workTimeEnd < p_workTimeBegin)
    {
        throw std::invalid_argument("WorkTimeEnd can't be less than workTimeBegin");
    }

    std::getline(p_file, line);
    p_logLinesCounter++;
    int pricePerHour = stoi_decorator(line);
    p_gameTables.resize(gameTablesCount, GameTable(pricePerHour));
}

void LogParser::parseBody()
{
    std::string line;
    std::vector<std::string> tokens;
    std::istringstream iss;
    std::string token;

    Time currentTime = p_workTimeBegin;
    while (std::getline(p_file, line))
    {
        p_logLinesCounter++;
        std::cout << line << std::endl;
        tokens.clear();
        iss.str(line);
        iss.clear();

        while (iss >> token)
        {
            tokens.push_back(token);
        }
        parseEvent(tokens, currentTime);
    }
}

void LogParser::parseEvent(const std::vector<std::string> &tokens, Time &previousTime)
{
    if (tokens.size() < MIN_LOG_LINE_ARGUMENTS || tokens.size() > MAX_LOG_LINE_ARGUMENTS)
        throw std::invalid_argument("Incorrect log line format");

    Time currentTime;
    int currentEvent;
    std::string currentClient;
    int currentGameTable = GAME_TABLE_IS_UNDEFINED;

    currentTime.StringToTime(tokens[0]);
    currentEvent = stoi_decorator(tokens[1]);
    currentClient = name_parser(tokens[2]);

    if (p_logLinesCounter > LOG_HEADER_END && currentTime < previousTime)
        throw std::invalid_argument("Unsorted lines in the log");

    switch (currentEvent)
    {
    case IncomingEventID::ClientHasCome:
        handleClientHasCome(currentClient, currentTime);
        break;
    case IncomingEventID::ClientTakeGameTable:
        handleClientTakeGameTable(tokens, currentClient, currentTime);
        break;
    case IncomingEventID::ClientIsWaiting:
        handleClientIsWaiting(currentClient, currentTime);
        break;
    case IncomingEventID::ClientHasLeft:
        handleClientHasLeft(currentClient, currentTime);
        break;
    default:
        throw std::invalid_argument("The event does not exist");
    }
    previousTime = currentTime;
}

void LogParser::handleClientHasCome(const std::string &currentClient, Time &currentTime)
{
    if (currentTime < p_workTimeBegin || p_workTimeEnd < currentTime)
    {
        std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError << " NotOpenYet" << std::endl;
        return;
    }
    if (p_clientInfo.find(currentClient) != p_clientInfo.end())
    {
        std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError << " YouShallNotPass" << std::endl;
        return;
    }
    p_clientInfo[currentClient].gameTableNumber = GAME_TABLE_IS_UNDEFINED;
}

void LogParser::handleClientTakeGameTable(const std::vector<std::string> &tokens, const std::string &currentClient, Time &currentTime)
{
    if (tokens.size() != 4)
    {
        throw std::invalid_argument("Incorrect log line format");
    }
    int currentGameTable = stoi_decorator(tokens[3]) - 1;

    if (currentGameTable < 0 || currentGameTable >= p_gameTables.size())
    {
        throw std::invalid_argument("Table " + std::to_string(currentGameTable + 1) + " does not exist");
    }
    if (p_clientInfo.find(currentClient) == p_clientInfo.end())
    {
        std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError << " ClientUnknown" << std::endl;
        return;
    }
    if (p_gameTables[currentGameTable].IsBusy())
    {
        std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError << " PlaceIsBusy" << std::endl;
        return;
    }
    if (p_clientInfo[currentClient].gameTableNumber != GAME_TABLE_IS_UNDEFINED)
    {
        endGameSession(currentClient, currentTime);
    }
    startGameSession(currentClient, currentGameTable, currentTime);
}

void LogParser::handleClientIsWaiting(const std::string &currentClient, Time &currentTime)
{
    if (p_clientInfo.find(currentClient) == p_clientInfo.end())
    {
        std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError << " ClientUnknown" << std::endl;
        return;
    }
    if (p_freeGameTablesCount > 0 || p_clientInfo[currentClient].gameTableNumber != GAME_TABLE_IS_UNDEFINED)
    {
        std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError << " ICanWaitNoLonger!" << std::endl;
        return;
    }
    if (p_queueClients.size() > p_gameTables.size())
    {
        std::cout << currentTime.GetString() + " " << OutgoingEventID::ClientHasLeftForced << " " + currentClient << std::endl;
        p_clientInfo.erase(currentClient);
        return;
    }
    p_queueClients.push(currentClient);
}

void LogParser::handleClientHasLeft(const std::string &currentClient, Time &currentTime)
{
    if (p_clientInfo.find(currentClient) == p_clientInfo.end())
    {
        std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError << " ClientUnknown" << std::endl;
        return;
    }

    int currentGameTable = p_clientInfo[currentClient].gameTableNumber;

    if (currentGameTable != GAME_TABLE_IS_UNDEFINED)
    {
        endGameSession(currentClient, currentTime);
        p_clientInfo.erase(currentClient);

        if (!p_queueClients.empty())
        {
            std::string nextClient = p_queueClients.front();
            p_queueClients.pop();

            startGameSession(nextClient, currentGameTable, currentTime);
            std::cout << currentTime.GetString() + " " << OutgoingEventID::ClientFromQueueTakeGameTable << " " + nextClient + " " + std::to_string(currentGameTable + 1) << std::endl;
        }
    }
}