#pragma once

#include <assert.h>
#include <iostream>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG(message) std::cout << message << std::endl
#define LOG_WARNING(message) std::cerr << message << std::endl
#define ERROR(message) {std::cerr << message << std::endl; throw message;}
#define LOG_ERROR(message) std::cerr << "(" << __FILENAME__ << ":" << __LINE__ << ") " << message << std::endl

#define LOG_DEBUG(message) std::cout << "(" << __FILENAME__ << ":" << __LINE__ << ") " << message << std::endl

#define TERMINATE(message) std::cerr << "SYSTEM Error:" << std::endl << message << std::endl << "Press any key to continue..."; std::cin.get(); exit(-1)
#define WAIT() std::cout << "Press any key to continue... "; std::cin.get()