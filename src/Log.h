#ifndef LOG_H
#define LOG_H

#include <vector>
#include "Tag.h"

namespace OpenTracing
{
    enum class LogCount {
        ERROR = -1,
        ONE = 0,
        QUATER = 1,
        HALF = 2,
        THREEQUATERS = 3,
        WHOLE = 4,
        UPPERBOUND = 5,
        PARTIAL = 6
    };

    class Log
    {
    public:
        int64_t _timestamp;
        std::vector<Tag*> _fields;

        Log(std::vector<Tag*>& logs, const Php::Value& timestamp = nullptr);
        Log(const Log&) = delete;
        ~Log();
        
        operator std::string() const;

        static std::string toString(LogCount log)
        {
            switch (log)
            {
            case LogCount::PARTIAL:
                return "PARTIAL";
                break;
            case LogCount::UPPERBOUND:
                return "UPPERBOUND";
                break;
            case LogCount::WHOLE:
                return "WHOLE";
                break;
            case LogCount::THREEQUATERS:
                return "THREEQUATERS";
                break;
            case LogCount::HALF:
                return "HALF";
                break;
            case LogCount::QUATER:
                return "QUATER";
                break;
            case LogCount::ONE:
                return "ONE";
                break;
            case LogCount::ERROR:
                return "ERROR";
                break;
            default:
                return "UNDEFINDE";
                break;
            }
        }
    };
}

#endif /* LOG_H */

