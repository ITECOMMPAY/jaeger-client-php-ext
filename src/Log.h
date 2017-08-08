#ifndef LOG_H
#define LOG_H
#include "Tag.h"

namespace OpenTracing
{
    class Log
    {
    public:
        int64_t _timestamp;
        std::vector<Tag*> _fields;
        static const int LOGS_THRESHOLD = 50;

        Log(std::vector<Tag*>& logs, const Php::Value& timestamp = nullptr);
        Log(const Log&) = delete;
        ~Log();
    };
}

#endif /* LOG_H */

