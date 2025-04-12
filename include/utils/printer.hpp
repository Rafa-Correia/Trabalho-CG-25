#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <string>
#include <iostream>
#include <vector>

namespace printer
{
        inline void print_warning(const std::string &message, const std::string &caller = std::string(""))
        {
                if (std::string("").compare(caller) != 0)
                {
                        std::cout << "\033[38;5;208mWarning (" << caller << "):\033[0m " << message << std::endl;
                }
                else
                {
                        std::cout << "\033[38;5;208mWarning:\033[0m " << message << std::endl;
                }
        }

        inline void print_exception(const std::string &message, const std::string &caller = std::string(""))
        {
                if (std::string("").compare(caller) != 0)
                {
                        std::cout << "\033[31mException (" << caller << "): \033[0m " << message << std::endl;
                }
                else
                {
                        std::cout << "\033[31mException: \033[0m " << message << std::endl;
                }
        }

        inline void print_info(const std::string &message, const std::string &caller = std::string(""))
        {
                if (std::string("").compare(caller) != 0)
                {
                        std::cout << "\033[96mInfo (" << caller << "): \033[0m " << message << std::endl;
                }
                else
                {
                        std::cout << "\033[96mInfo: \033[0m " << message << std::endl;
                }
        }

        inline void print_init()
        {
                std::cout << "\nPress P to print the whole shebang (tutorial / keys).\n\n"
                          << std::endl;
        }

        inline void print_tutorial(const unsigned char *vendor, const unsigned char *renderer, const unsigned char *version)
        {
                // noted that using endl (flushing) every time this function is called is VERY (!) inneficient. We should be "\n"ing
                // and flushing only at the end. This is only called like once, so it's probably fine.
                std::cout << "\n>------------------------------------------------------------------------------------------<\n"
                          << std::endl;

                std::cout << "\n\n> ! - - - - - General Info - - - - - ! <\n"
                          << std::endl;
                std::cout << "Vendor: " << vendor << std::endl;
                std::cout << "Renderer: " << renderer << std::endl;
                std::cout << "Version: " << version << std::endl;

                std::cout << "\n\n> ! - - - - - Debug options - - - - - ! <\n"
                          << std::endl;

                std::cout << "Press 1 to toggle axis rendering." << std::endl;
                std::cout << "Press 2 to toggle between wire and solid rendering mode." << std::endl;
                std::cout << "Press 3 to toggle dynamic translation path rendering." << std::endl;
                std::cout << "Press 4 to toggle animation on or off." << std::endl;

                std::cout << "\nPress 5 to toggle bounding sphere rendering." << std::endl;
                std::cout << "Press 6 to toggle view frustum rendering." << std::endl;
                std::cout << "Press 7 to toggle view frustum culling." << std::endl;
                std::cout << "Press 8 to toggle frustum update on free camera mode." << std::endl;

                std::cout << "\n\n> ! - - - - - Keyboard / mouse controls - - - - - ! <\n"
                          << std::endl;

                std::cout << ">- - - Fixed camera - - -<\n"
                          << std::endl;
                std::cout << "W/A/S/D to rotate camera around target." << std::endl;
                std::cout << "Press Z to zoom into target." << std::endl;
                std::cout << "Press X to zoom out of target." << std::endl;
                std::cout << "Press C to change camera target." << std::endl;
                std::cout << "Press R to reset camera to first target." << std::endl;

                std::cout << "\n>- - - Free camera - - -<\n"
                          << std::endl;
                std::cout << "Use mouse to look around / change camera direction." << std::endl;
                std::cout << "W/A/S/D to move camera relative to it's direction." << std::endl;

                std::cout << "\n>- - - General / Common - - -<\n"
                          << std::endl;
                std::cout << "Press F to switch between fixed camera and free camera mode." << std::endl;
                std::cout << "Press P to print this info again." << std::endl;
                std::cout << "Press ESC to exit." << std::endl;

                std::cout << "\n>------------------------------------------------------------------------------------------<\n\n\n\n"
                          << std::endl;
        }
};

#endif