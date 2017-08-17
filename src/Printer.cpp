#include "Printer.h"
#include <time.h>
#include <sys/stat.h>
#include <mutex>

std::mutex mtx;

//int mkpath(std::string s, mode_t mode)
//{
//    size_t pre = 0, pos;
//    std::string dir;
//    int mdret{ -1 };
//
//    if (s[s.size() - 1] != '/')
//    {
//        // force trailing / so we can handle everything in loop
//        s += '/';
//    }
//
//    while ((pos = s.find_first_of('/', pre)) != std::string::npos)
//    {
//        dir = s.substr(0, pos++);
//        //Php::out << dir << std::endl;
//        pre = pos;
//        if (dir.size() == 0)
//            continue; // if leading / first time is 0 length
//        if ((mdret = mkdir(dir.c_str(), mode)) && errno != EEXIST)
//        {
//            return mdret;
//        }
//    }
//    return mdret;
//}

OpenTracing::Printer::Printer(const std::string& reportPath, bool printFooters) :
    _reportPath{ reportPath },
    _reportName{ "tracer-cpp.log" },
    _flag{ printFooters }
{
    try
    {
        //if (mkpath(reportPath, 0755) == -1)
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

    if (_flag)
    {
        PrintStart();
    }
    {
        //std::ostringstream ss;
        //ss << this;
        //PrintLine("Printer constructor: " + ss.str());
    }
}

OpenTracing::Printer::~Printer()
{
    {
        //std::ostringstream ss;
        //ss << this;
        //PrintLine("~Printer destructor: " + ss.str());
    }
    if (_flag)
    {
        PrintEnd();
    }
}

void OpenTracing::Printer::Open()
{
    try
    {
        mtx.lock();

        _logFile.open(_reportPath + "/" + _reportName, std::ofstream::out | std::ofstream::app);
        if (_logFile.bad())
        {
            Php::out << "Couldn't open log file" << std::endl;
        }

        mtx.unlock();
    }
    catch (...)
    {
    }
}

void OpenTracing::Printer::Close()
{
    try
    {
        mtx.lock();

        _logFile.close();

        mtx.unlock();
    }
    catch (...)
    {
    }
}

void OpenTracing::Printer::PrintStart()
{
    if (!_logFile.bad())
    {
        struct tm lTimeinfo;
        char strDate[120];
        std::ostringstream line;

        time(&startTime);
        localtime_r(&startTime, &lTimeinfo);
        strftime(strDate, 51, "%Y-%m-%d %H:%M:%S", &lTimeinfo);

        line <<
            "********************************************************************************\n" <<
            "Log Started: " << std::string(strDate) << " (" << this << ")\n" <<
            "********************************************************************************";

        PrintLine(line.str(), false);
    }
}

void OpenTracing::Printer::PrintEnd()
{
    if (!_logFile.bad())
    {
        struct tm lTimeinfo;
        char strDate[120];
        std::ostringstream line;

        time(&stopTime);
        localtime_r(&stopTime, &lTimeinfo);
        strftime(strDate, 51, "%Y-%m-%d %H:%M:%S", &lTimeinfo);

        line <<
            "********************************************************************************\n" <<
            "Log Finished: " << std::string(strDate) << " (" << this << ")\n" <<
            "********************************************************************************\n";

        PrintLine(line.str(), false);
    }
}

void OpenTracing::Printer::PrintLine(const std::string& line, bool printTime)
{
    Open();

    if (!_logFile.bad())
    {
        try
        {
            if (printTime)
            {
                struct tm lTimeinfo;
                char strDate[120];

                time(&startTime);
                localtime_r(&startTime, &lTimeinfo);
                strftime(strDate, 51, "%Y-%m-%d %H:%M:%S", &lTimeinfo);
                _logFile <<
                    std::string(strDate) <<
                    //" " << 
                    //this << 
                    "\t\t";
            }
            _logFile << line << std::endl;
        }
        catch (...)
        {
        }
    }

    Close();
}

