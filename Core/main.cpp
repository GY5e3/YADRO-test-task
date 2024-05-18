#include <iostream>

#include "LogParser.hpp"

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        std::cout << "Argument with file name is missing" << std::endl;
        return 1;
    }
    LogParser parser(argv[1]);
    return parser.Execute();
}
