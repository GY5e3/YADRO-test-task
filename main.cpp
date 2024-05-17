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

void StartGameSession(std::unordered_map<std::string, ClientSession> &mappingClientSession,
                      const std::string &currentClient,
                      const int &currentGameTable,
                      const Time &currentTime,
                      std::vector<GameTable> &gameTables,
                      int &freeGameTablesCount)
{
    mappingClientSession[currentClient].gameTableNumber = currentGameTable;
    mappingClientSession[currentClient].startSessionTime = currentTime;
    gameTables[currentGameTable].AddClient(currentClient);
    freeGameTablesCount--;
}

void EndGameSession(std::unordered_map<std::string, ClientSession> &mappingClientSession,
                    const std::string &currentClient,
                    Time &currentTime,
                    std::vector<GameTable> &gameTables,
                    int &freeGameTablesCount)
{
    int index = mappingClientSession[currentClient].gameTableNumber;
    gameTables[index].AddDeltaTimeAndProfit(
        currentTime - mappingClientSession[currentClient].startSessionTime);
    gameTables[index].RemoveClient();
    mappingClientSession[currentClient].gameTableNumber = GAME_TABLE_IS_UNDEFINED;
    freeGameTablesCount++;
}
// TODO: Для более удобного тестирования вынести основной функционал с парсингом в отдельный класс
//      в функцию передается по ссылке строка, в которую запишется весь лог с входящими и исходящими событиями
//      сама функция вернет код завершения програмы
int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Argument with file name is missing" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        std::cerr << "File could not be opened" << std::endl;
        return 1;
    }

    StoiDecorator stoi_decorator;
    ClientNameParser name_parcer;

    int logLinesCounter = 0;           // Переменная для хранения числа строк в логе
    int gameTablesCount;               // Число игровых столов
    Time workTimeBegin;                // Время начала работы компьютерного клуба
    Time workTimeEnd;                  // Время окончания работы компьютерного клуба
    int pricePerHour;                  // Цена за час использования игрового стола
    std::vector<GameTable> gameTables; // Здесь будем хранить информацию обо всех столах

    int freeGameTablesCount;
    std::unordered_map<std::string, ClientSession> mappingClientSession;
    std::queue<std::string> queueClients;

    std::string line; // Буферная переменная для хранения текущей строки в логе

    std::istringstream iss;
    std::string token;
    std::vector<std::string> tokens;
    try
    {
        // Чтение первой строки заголовка в логе
        std::getline(file, line);
        logLinesCounter++;
        gameTablesCount = stoi_decorator(line);
        freeGameTablesCount = gameTablesCount;


        // Чтение второй строки заголовка в логе
        std::getline(file, line);
        logLinesCounter++;
        iss.str(line);
        iss.clear();
        while (iss >> token)
            tokens.push_back(token);
        if (tokens.size() != 2)
            throw std::invalid_argument("Incorrect log line format");
        workTimeBegin.StringToTime(tokens[0]);
        workTimeEnd.StringToTime(tokens[1]);
        // Поскольку во второй строке должно указываться только время работы,
        // нужно исключить эти случаи, поскольку из-за отстутствия даты
        // не получится корректно обработать строки лога, между которыми "наступил новый день"
        if (workTimeEnd < workTimeBegin)
            throw std::invalid_argument("WorkTimeEnd can`t be less than workTimeBegin");


        // Чтение третьей строки заголовка в логе
        std::getline(file, line);
        logLinesCounter++;
        pricePerHour = stoi_decorator(line);
        gameTables.resize(gameTablesCount, GameTable(pricePerHour));


        std::cout << workTimeBegin.GetString() << std::endl;


        Time previousTime(workTimeBegin);

        Time currentTime;   
        int currentEvent;
        std::string currentClient;
        int currentGameTable;
        // Чтение остальных строк лога с событиями
        while (std::getline(file, line))
        {
            logLinesCounter++;
            std::cout << line << std::endl;

            tokens.clear();
            iss.str(line);
            iss.clear();

            while (iss >> token)
                tokens.push_back(token);

            if (MAX_LOG_LINE_ARGUMENTS < tokens.size() || tokens.size() < MIN_LOG_LINE_ARGUMENTS)
                throw std::invalid_argument("Incorrect log line format");

            currentTime.StringToTime(tokens[0]);
            currentEvent = stoi_decorator(tokens[1]);
            currentClient = name_parcer(tokens[2]);
            currentGameTable = GAME_TABLE_IS_UNDEFINED;

            if (logLinesCounter > LOG_HEADER_END && currentTime < previousTime)
                throw std::invalid_argument("Unsorted lines in the log");

            switch (currentEvent)
            {
            case IncomingEventID::ClientHasCome:
                if (currentTime < workTimeBegin || workTimeEnd < currentTime)
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError
                              << " NotOpenYet" << std::endl;
                    break;
                }
                if (mappingClientSession.find(currentClient) != mappingClientSession.end())
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError
                              << " YouShallNotPass" << std::endl;
                    break;
                }
                mappingClientSession[currentClient].gameTableNumber = GAME_TABLE_IS_UNDEFINED;
                break;
            case IncomingEventID::ClientTakeGameTable:
                if (tokens.size() != 4)
                    throw std::invalid_argument("Incorrect log line format");
                currentGameTable = stoi_decorator(tokens[3]) - 1;

                if (currentGameTable < 0 || currentGameTable >= gameTablesCount)
                    throw std::invalid_argument("Table " + std::to_string(currentGameTable + 1) +
                                                " does not exist");
                if (mappingClientSession.find(currentClient) == mappingClientSession.end())
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError
                              << " ClientUnknown" << std::endl;
                    break;
                }
                if (gameTables[currentGameTable].IsBusy())
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError
                              << " PlaceIsBusy" << std::endl;
                    break;
                }
                if (mappingClientSession[currentClient].gameTableNumber != GAME_TABLE_IS_UNDEFINED)
                {
                    EndGameSession(mappingClientSession, currentClient, currentTime, gameTables, freeGameTablesCount);
                }
                StartGameSession(mappingClientSession, currentClient, currentGameTable, currentTime, gameTables, freeGameTablesCount);
                break;
            case IncomingEventID::ClientIsWaiting:
                // В ТЗ это явно не прописано, но могу предположить, что если клиент не вошёл в клуб, то и ожидать он не может
                if (mappingClientSession.find(currentClient) == mappingClientSession.end())
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError
                              << " ClientUnknown" << std::endl;
                    break;
                }
                // Опять же в ТЗ не прописано, как должна вести себя программа,
                // если клиент уже занял стол, поэтому я добавил обработку этого условия сюда
                if (freeGameTablesCount > 0 || mappingClientSession[currentClient].gameTableNumber != GAME_TABLE_IS_UNDEFINED)
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError
                              << " ICanWaitNoLonger!" << std::endl;
                    break;
                }
                if (queueClients.size() > gameTablesCount)
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::ClientHasLeftForced
                              << " " + currentClient << std::endl;
                    mappingClientSession.erase(currentClient);
                    break;
                }
                queueClients.push(currentClient);
                break;
            case IncomingEventID::ClientHasLeft:
                if (mappingClientSession.find(currentClient) == mappingClientSession.end())
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError
                              << " ClientUnknown" << std::endl;
                    break;
                }
                currentGameTable = mappingClientSession[currentClient].gameTableNumber;

                EndGameSession(mappingClientSession, currentClient, currentTime, gameTables, freeGameTablesCount);

                mappingClientSession.erase(currentClient);

                if (queueClients.size() > 0)
                {
                    currentClient = queueClients.front();
                    queueClients.pop();

                    StartGameSession(mappingClientSession, currentClient, currentGameTable, currentTime, gameTables, freeGameTablesCount);

                    std::cout << currentTime.GetString() + " " << OutgoingEventID::ClientFromQueueTakeGameTable
                              << " " + currentClient + " " + std::to_string(currentGameTable + 1) << std::endl;
                }
                break;
            default:
                throw std::invalid_argument("The event does not exist");
                break;
            }
            previousTime = currentTime;
        }

        std::cout << workTimeEnd.GetString() << std::endl;

        // TODO: вывод всех клиентов, которые остались в клубе к моменту закрытия
        for (auto lastClient : mappingClientSession)
        {
            EndGameSession(mappingClientSession, lastClient.first, workTimeEnd, gameTables, freeGameTablesCount);
        }

        for (int i = 0; i < gameTables.size(); i++)
        {
            std::cout << i + 1 << " " << gameTables[i].GetString() << std::endl;
        }
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Line " + std::to_string(logLinesCounter) + ": " + e.what() << std::endl;
        if (logLinesCounter == 2)
            std::cerr << "It must look like: \"<HH:MM> <HH:MM>\"" << std::endl;

        return 1;
    }

    file.close();
    return 0;
}
