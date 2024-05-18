#include "FastQueue.hpp"

void FastQueue::push(std::string value)
{
    auto it = p_list.insert(p_list.end(), value);
    p_map[value] = it;
}

void FastQueue::remove(std::string value)
{
    auto it = p_map.find(value);
    if (p_map.find(value) != p_map.end())
    {
        p_list.erase(it->second);
        p_map.erase(it);
    }
}

void FastQueue::pop()
{
    if (!p_list.empty())
    {
        auto it = p_list.begin();
        p_map.erase(*it);
        p_list.erase(it);
    }
}

std::string FastQueue::front() const
{
    if (!p_list.empty())
    {
        return p_list.front();
    }
    throw std::runtime_error("Queue is empty");
}

bool FastQueue::empty() const
{
    return p_list.empty();
}

int FastQueue::size() const
{
    return p_map.size();
}