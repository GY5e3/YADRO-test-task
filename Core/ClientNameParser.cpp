#include "ClientNameParser.hpp"

std::string ClientNameParser::operator()(const std::string &str)
{
    for (auto c : str)
    {
        if ('a' <= c && c <= 'z' || '0' <= c && c <= '9' || c == '_' || c == '-')
            continue;
        else
            throw std::invalid_argument("Invalid client name");
    }
    return str;
}