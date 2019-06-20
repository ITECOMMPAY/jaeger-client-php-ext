#ifndef PRINTER_H
#define PRINTER_H

// #define EXTENDED_DEBUG

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <phpcpp.h>

namespace OpenTracing
{
    class Printer
    {
    private:
        std::string _reportPath;
        std::string _reportName;
        std::ofstream _logFile;
        bool _printFooters;
        time_t _startTime;
        time_t _stopTime;

        void Open();
        void Close();
        void PrintStart();
        void PrintEnd();

    public:
        Printer(const std::string& reportPath, bool printFooters);
        ~Printer();

        bool _enabled;
        void PrintLine(const std::string& line, bool skip = false, bool printTime = true);
    };
}

#endif /* PRINTER_H */
