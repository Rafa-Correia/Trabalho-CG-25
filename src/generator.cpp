#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
    if (argc < 5 || argc > 7) {
        std::cout << "Wrong number of arguments." << std::endl;
        return 1;
    }

    std::string model_type(argv[1]);
    if(model_type.compare("sphere") == 0) {

    } 
    else if (model_type.compare("box") == 0) {

    } 
    else if(model_type.compare("cone") == 0) {

    }
    else if(model_type.compare("plane") == 0) {
        
    }
    else {
        std::cout << "Invalid shape!" << std::endl;
        return 1;
    }

    return 0;
}
