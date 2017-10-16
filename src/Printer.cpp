#include "Printer.h"
#include <time.h>
#include <sys/stat.h>
#include <mutex>

std::mutex mtx;

int mkpath(std::string s, mode_t mode)
{
    size_t pre = 0, pos;
    std::string dir;
    int mdret{ -1 };

    if (s[s.size() - 1] != '/')
    {
        // force trailing / so we can handle everything in loop
        s += '/';
    }

    while ((pos = s.find_first_of('/', pre)) != std::string::npos)
    {
        dir = s.substr(0, pos++);
        pre = pos;
        if (dir.size() == 0)
            continue; // if leading / first time is 0 length
        if ((mdret = mkdir(dir.c_str(), mode)) && errno != EEXIST)
        {
            return mdret;
        }
    }
    return mdret;
}

OpenTracing::Printer::Printer(const std::string& reportPath, bool printFooters) :
    _reportPath{ reportPath },
    _reportName{ "tracer-cpp.log" },
    _printFooters{ printFooters },
    _enabled{ false }
{
    try
    {
        //if (mkpath(reportPath, 0644) == -1)
        {
            //throw
        }

        //std::stringstream ss;
        //ss << "mkdir -p " << reportPath;
        //if (system(ss.str().c_str()) == -1)
        //{
        //    //throw
        //}

    }
    catch (...)
    {
        Php::out << "Can't create directory " << reportPath << std::endl;
    }

    if (_printFooters)
    {
        PrintStart();
    }

    {
        std::ostringstream ss;
        ss << this;
        PrintLine("Printer " + ss.str() + " constructor", true);
    }
}

OpenTracing::Printer::~Printer()
{
    {
        std::ostringstream ss;
        ss << this;
        PrintLine("~Printer " + ss.str() + " destructor", true);
    }

    if (_printFooters)
    {
        PrintEnd();
    }
}

void OpenTracing::Printer::Open()
{
    try
    {
        _logFile.open(_reportPath + "/" + _reportName, std::ofstream::out | std::ofstream::app);
        if (_logFile.bad())
        {
            Php::out << "Can't open log file" << std::endl;
        }
    }
    catch (...)
    {
    }
}

void OpenTracing::Printer::Close()
{
    try
    {
        _logFile.close();
    }
    catch (...)
    {
    }
}

void OpenTracing::Printer::PrintStart()
{
    struct tm lTimeinfo;
    char strDate[120];
    std::ostringstream line;

    time(&_startTime);
    localtime_r(&_startTime, &lTimeinfo);
    strftime(strDate, 51, "%Y-%m-%d %H:%M:%S", &lTimeinfo);

    line <<
        "********************************************************************************\n" <<
        "Log Started: " << std::string(strDate) << " (" << this << ")\n" <<
        "********************************************************************************";

    PrintLine(line.str(), false);
}

void OpenTracing::Printer::PrintEnd()
{
    struct tm lTimeinfo;
    char strDate[120];
    std::ostringstream line;

    time(&_stopTime);
    localtime_r(&_stopTime, &lTimeinfo);
    strftime(strDate, 51, "%Y-%m-%d %H:%M:%S", &lTimeinfo);

    line <<
        "********************************************************************************\n" <<
        "Log Finished: " << std::string(strDate) << " (" << this << ")\n" <<
        "********************************************************************************\n";

    PrintLine(line.str(), false);
}

void OpenTracing::Printer::PrintLine(const std::string& line, bool skip, bool printTime)
{
    if (_enabled)
    {
        mtx.lock();
        Open();

        if (!_logFile.bad())
        {
            try
            {
                if (!skip)
                {
                    if (printTime)
                    {
                        struct tm lTimeinfo;
                        char strDate[120];

                        time(&_startTime);
                        localtime_r(&_startTime, &lTimeinfo);
                        strftime(strDate, 51, "%Y-%m-%d %H:%M:%S", &lTimeinfo);
                        _logFile <<
                            std::string(strDate) <<
                            " " <<
                            this <<
                            "\t\t";
                    }
                    _logFile << line << std::endl;
                }
            }
            catch (...)
            {
            }
        }
        else
        {
            throw Php::Exception("Problem with printer");
        }

        Close();
        mtx.unlock();
    }
}

