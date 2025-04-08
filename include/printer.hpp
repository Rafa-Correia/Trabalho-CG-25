#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <string>
#include <iostream>
#include <vector>

class printer
{
public:
        static void print_warning(const std::string &message)
        {
                std::cout << "\033[38;5;208mWarning:\033[0m " << message << std::endl;
        }

        static void print_exception(const std::string &message)
        {
                std::cout << "\033[31mException: \033[0m " << message << std::endl;
        }
};

#endif