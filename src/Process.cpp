#include <iostream>
#include <time.h>
#include "Process.h"
#include "Helper.h"
#include "Tracer.h"

const std::string OpenTracing::Process::DEFAULT_SERVICE_NAME{ "unknown" };

int OpenTracing::Process::ip_timestamp = 0;
std::string OpenTracing::Process::ip;
std::string OpenTracing::Process::hostname;

OpenTracing::Process::Process(const std::string& serviceName) :
    _serviceName{ serviceName }
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("\tProcess " + ss.str() + " constructor", true);
    }

    time_t now = time(NULL);

    int ip_revalidate_freq = Php::ini_get("jaeger-client.ip_revalidate_freq");

    if (ip_revalidate_freq <= 0 || ip_timestamp + ip_revalidate_freq <= now || ip.empty()) {
        ip = Helper::getCurrentIp();
        hostname = Helper::getHostName();
        ip_timestamp = now;
    }
    _tags.push_back(new Tag(Tag::TAG_TYPE_HOST, hostname));
    _tags.push_back(new Tag(Tag::TAG_TYPE_IP, ip));
}

OpenTracing::Process::~Process()
{
    for (auto& iter : _tags)
        delete iter;
    _tags.clear();

    Tracer::file_logger.PrintLine("    ~Process", true);
}
