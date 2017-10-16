#ifndef LOG_H
#define LOG_H

#include <vector>
#include "Tag.h"

namespace OpenTracing
{
    enum class LogCount {
        ERROR = -1,
        NOTHING = 0,
        QUATER = 1,
        HALF = 2,
        THREEQUATERS = 3,
        WHOLE = 4,
    };

    class Log
    {
    public:
        int64_t _timestamp;
        std::vector<Tag*> _fields;

        Log(std::vector<Tag*>& logs, const Php::Value& timestamp = nullptr);
        Log(const Log&) = delete;
        ~Log();
    };
}

#endif /* LOG_H */

