#ifndef LOG_H
#define LOG_H
#include "Tag.h"

class Log
{
    int64_t _timestamp;
    std::vector<Tag*> _fields;

public:
    static const int LOGS_THRESHOLD = 50;
    Log(const std::vector<Tag*>& logs, const Php::Value& timestamp = nullptr);
    Log(const Log&) = delete;
    ~Log();

private:

};

#endif /* LOG_H */

