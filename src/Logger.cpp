#include "Logger.h"
#include <time.h>
#include <sys/stat.h>

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
        Php::out << dir << std::endl;
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


OpenTracing::Logger::Logger(const std::string& reportPath) :
    _reportPath{ reportPath },
    _reportName{ "tracer-cpp.log" }
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
    PrintStart();
}

OpenTracing::Logger::~Logger()
{
    PrintLine("~Logger");
    PrintEnd();
}

void OpenTracing::Logger::Open()
{
    _logFile.open(_reportPath + "/" + _reportName, std::ofstream::out | std::ofstream::app);
    if (_logFile.bad())
    {
        Php::out << "Couldn't open log file" << std::endl;
    }
}

void OpenTracing::Logger::Close()
{
    _logFile.close();
}

void OpenTracing::Logger::PrintStart()
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

void OpenTracing::Logger::PrintEnd()
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

void OpenTracing::Logger::PrintLine(const std::string& line, bool printTime)
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
                _logFile << std::string(strDate) << '\t';
            }
            _logFile << line << std::endl;
        }
        catch (...)
        {
        }
    }

    Close();

    Php::out << line << std::endl;
}

//void OpenTracing::Logger::Print(const std::string& line)
//{
//    Open();
//
//    //if (!_logFile.bad())
//    //{
//    //    try
//    //    {
//            _logFile << line;
//    //    }
//    //    catch (...)
//    //    {
//    //    }
//    //}
//
//    Close();
//
//    Php::out << line;
//}
