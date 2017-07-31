#ifndef HELPER_H
#define HELPER_H
#include <unistd.h>
#include <limits.h>
#include "ISpan.h"

class Helper
{
public:
    static const std::string getHostName();
    static const std::string getCurrentIp();
};

#endif /* HELPER_H */
