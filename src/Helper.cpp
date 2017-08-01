#include "Helper.h"
#include <sys/time.h>

const int64_t Helper::now()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t microsec = ((unsigned long long)time.tv_sec * 1000 * 1000) + time.tv_usec;
    return microsec;
}

const std::string Helper::getHostName()
{
    char name[HOST_NAME_MAX];
    gethostname(name, HOST_NAME_MAX);
    return std::string{ name };
}

const std::string Helper::getCurrentIp()
{
    const char *cmd = "ifconfig | grep -Eo 'inet (addr:)?([0-9]*\\.){3}[0-9]*' | grep -Eo '([0-9]*\\.){3}[0-9]*' | grep -v '127.0.0.1'";
    char buf[BUFSIZ];
    FILE *ptr;
    if ((ptr = popen(cmd, "r")) != NULL)
    {
        while (fgets(buf, BUFSIZ, ptr) != NULL);
        pclose(ptr);
    }

    return std::string{ buf };
}


