#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>

#include "GameTable.hpp"
#include "Time.hpp"
#include "SafeStoiDecorator.hpp"
#include "ClientNameParser.hpp"

const int LOG_HEADER_END = 4;
const int MIN_LOG_LINE_LENGTH = 3;

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
// TODO: Для более удобного тестирования вынести основной функционал с парсингом в отдельный класс
//      в функцию передается по ссылке строка, в которую запишется весь лог с входящими и исходящими событиями
//      сама функция вернет код завершения програмы
// TODO: возможно, стоит загнать весь код в большой try-catch блок
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

    SafeStoiDecorator safe_stoi;
    ClientNameParser name_parcer;

    int logLinesCounter = 0;           // Переменная для хранения числа строк в логе
    int gameTablesCount;               // Число игровых столов
    Time workTimeBegin;                // Время начала работы компьютерного клуба
    Time workTimeEnd;                  // Время окончания работы компьютерного клуба
    int pricePerHour;                  // Цена за час использования игрового стола
    std::vector<GameTable> gameTables; // Здесь будем хранить информацию обо всех столах

    int freeGameTablesCount;
    std::unordered_map<std::string, std::pair<int, Time>> clientGameTableMapping;

    std::string line; // Буферная переменная для хранения текущей строки в логе
    try
    {
        // Чтение первой строки в логе
        std::getline(file, line);
        logLinesCounter++;
        gameTablesCount = safe_stoi(line);
        gameTables.resize(gameTablesCount);
        freeGameTablesCount = gameTablesCount;

        // TODO: здесь надо обязательно распрасить по пробелам, иначе может пройти ошибочный тест типа: 00:01000:02
        // Чтение второй строки в логе
        std::getline(file, line);
        logLinesCounter++;
        workTimeBegin.StringToTime(line.substr(0, 5));
        workTimeEnd.StringToTime(line.substr(6, 5));
        // Поскольку во второй строке должно указываться только время работы,
        // нужно исключить эти случаи, поскольку из-за отстутствия даты
        // не получится корректно обработать строки лога, между которыми "наступил новый день"
        if (workTimeEnd < workTimeBegin)
            throw std::invalid_argument("WorkTimeEnd can`t be less than workTimeBegin");

        // Чтение третьей строки в логе
        std::getline(file, line);
        logLinesCounter++;
        pricePerHour = safe_stoi(line);

        std::cout << workTimeBegin.GetString() << std::endl;

        // Чтение остальных строк лога с событиями
        Time previousTime(workTimeBegin);
        while (std::getline(file, line))
        {
            logLinesCounter++;
            std::cout << line << std::endl;
            Time currentTime;

            std::vector<std::string> tokens;
            std::istringstream iss(line);
            std::string token;
            while (iss >> token)
                tokens.push_back(token);

            // TODO: парсинг всей строки

            currentTime.StringToTime(tokens[0]);
            int currentEvent = safe_stoi(tokens[1]);
            std::string currentClient = tokens[2];
            int currentGameTable = -1;

            // TODO: парсинг корректного имени клиента
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
                if (clientGameTableMapping.find(currentClient) != clientGameTableMapping.end())
                {
                    std::cout << currentTime.GetString() + " " << OutgoingEventID::EventError
                              << " YouShallNotPass" << std::endl;
                    break;
                }
                clientGameTableMapping[currentClient].first = -1;
                break;
            case IncomingEventID::ClientTakeGameTable:
                currentGameTable = safe_stoi(tokens[3]) - 1;
                if (currentGameTable < 0 || currentGameTable >= gameTablesCount)
                    throw std::invalid_argument("Table " + std::to_string(currentGameTable + 1) +
                                                " does not exist");
                if (clientGameTableMapping.find(currentClient) == clientGameTableMapping.end())
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
                
                if(clientGameTableMapping[currentClient].first != -1) {
                    int index = clientGameTableMapping[currentClient].first;
                    gameTables[index].AddDeltaTimeAndProfit(currentTime - clientGameTableMapping[currentClient].second);
                    gameTables[index].RemoveClient();

                }
                clientGameTableMapping[currentClient].first = currentGameTable;
                clientGameTableMapping[currentClient].second = currentTime;
                gameTables[currentGameTable].AddClient(currentClient);
                break;
            case IncomingEventID::ClientIsWaiting:

                //TODO: если клиент уже сидит за столом то он не может ожидать
                /* code */
                break;
            case IncomingEventID::ClientHasLeft:
                /* code */
                break;
            default:
                break;
            }
            previousTime = currentTime;
        }
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Line " + std::to_string(logLinesCounter) + ": " + e.what() << std::endl;
        if (logLinesCounter == 2)
            std::cerr << "\nIt must look like: \"<HH:MM> <HH:MM>\"" << std::endl;
        return 1;
    }

    // TODO: всё парсить по пробелам не то пизда на тестах
    // TODO: Логика для обработки строк лога
    // TODO: Как обрабатывать случаи, когда предыдущий клиент пришел после закрытия, а текущий - в рабочие часы?
    //       будем считать это ошибкой сортировки и будем заканчивать работу программы

    file.close();
    return 0;
}
