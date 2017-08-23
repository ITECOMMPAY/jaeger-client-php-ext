#ifndef PRINTER_H
#define PRINTER_H

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

        time_t startTime;
        time_t stopTime;
        int durationTime;

        bool _flag;

    public:
        Printer(const std::string& reportPath, bool printFooters);

        ~Printer();

        void Open();
        void Close();

        void PrintStart();
        void PrintEnd();
        void PrintLine(const std::string& line, bool printTime = true);
    };
}

#endif /* PRINTER_H */
