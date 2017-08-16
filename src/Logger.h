#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <phpcpp.h>

namespace OpenTracing
{
    class Logger
    {
    private:
        std::string _reportPath;
        std::string _reportName;

        std::ofstream _logFile;

        time_t startTime;
        time_t stopTime;
        int durationTime;
    public:
        Logger(const std::string& reportPath);

        ~Logger();

        void Open();
        void Close();

        void PrintStart();
        void PrintEnd();
        void PrintLine(const std::string& line, bool printTime = true);
    };
}

#endif /* LOGGER_H */
