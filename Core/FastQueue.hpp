#include <iostream>
#include <unordered_map>
#include <list>
/**
 * Эта структура данных нужна для того, чтобы за константное время удалять из очереди клиента, 
 * который решил уйти до того, как занял игровой стол
*/
class FastQueue
{
public:
    void push(std::string value);

    void remove(std::string value);

    void pop();

    std::string front() const;

    bool empty() const;

    int size() const;

private:
    std::list<std::string> p_list;                                                    
    std::unordered_map<std::string, std::list<std::string>::iterator> p_map; 
};