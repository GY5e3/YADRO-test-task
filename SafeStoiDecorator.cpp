#include "SafeStoiDecorator.hpp"

int SafeStoiDecorator::operator()(const std::string &str)
{
    int skipInsignificantZeros = 0;
    while (str[skipInsignificantZeros] == '0' && skipInsignificantZeros < str.length() - 1)
        skipInsignificantZeros++;
    int result = std::stoi(str.substr(skipInsignificantZeros));
    // Проверка сделана для того, чтобы исключать строки вида: *цифры**другие_символы*(342321helloworld);
    if (std::to_string(result).length() < str.substr(skipInsignificantZeros).length())
        throw std::invalid_argument("stoi");
    return result;
}