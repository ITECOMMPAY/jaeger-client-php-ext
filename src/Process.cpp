#include <iostream>
#include "Process.h"
#include "Helper.h"
#include "Tracer.h"

const std::string OpenTracing::Process::DEFAULT_SERVICE_NAME{ "unknown" };

OpenTracing::Process::Process(const std::string& serviceName) :
    _serviceName{ serviceName }
{
    {
        std::ostringstream ss;
        ss << "    Process::Process addr: " << this;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
    _tags.push_back(new Tag(Tag::TAG_TYPE_HOST, Helper::getHostName()));
    _tags.push_back(new Tag(Tag::TAG_TYPE_IP, Helper::getCurrentIp()));
}

OpenTracing::Process::~Process()
{
    for (auto& iter : _tags)
        delete iter;
    _tags.clear();

    Tracer::file_logger.PrintLine("    ~Process", true);
}
