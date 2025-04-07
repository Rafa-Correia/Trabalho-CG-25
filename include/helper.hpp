#ifndef HELPER_HPP
#define HELPER_HPP

#ifdef _WIN32
#include <windows.h>
#endif

#include <string>
#include <iostream>

class helper
{
public:
    static void print_warning(const std::string &message)
    {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
        std::cout << "Warning:";

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << " " << message << std::endl;
#else
        std::cout << "\033[38;5;208mWarning:\033[0m " << message << std::endl;
#endif
    }

    static void print_exception(const std::string &message)
    {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        std::cout << "Exception: ";

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << " " << message << std::endl;
#else
        std::cout << "\033[31mException: \033[0m " << message << std::endl;
#endif
    }
};

#endif