#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include "UdpReporter.h"
#include "Tracer.h"
using namespace OpenTracing;

UdpReporter::~UdpReporter()
{
    Tracer::file_logger.PrintLine("~UdpReporter", true);
}

UdpReporter::UdpReporter(const Php::Value& params)
{
    Tracer::file_logger.PrintLine("UdpReporter::UdpReporter", true);

    Php::Value defaults;
    defaults["addr"] = "localhost";
    defaults["port"] = 6831; //TCompact
    //defaults["port"] = 6832; //TBinary

    if (!params.isNull())
    {
        Php::Value par = params;
        _options = Php::call("array_merge", defaults, par);
    }
    else
    {
        _options = defaults;
    }
}

void UdpReporter::flush(const std::string& data) const
{
    Tracer::file_logger.PrintLine("    UdpReporter::flush", true);

    sockaddr_in _addr;
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(static_cast<uint16_t>(std::stoul(this->_options["port"].stringValue())));

    if (this->_options["addr"].stringValue() == "localhost")
        _addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* Inet 127.0.0.1.  */
    else
        _addr.sin_addr.s_addr = inet_addr(this->_options["addr"].stringValue().c_str());

    int _socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (_socket != -1)
    {
        ssize_t num_send = sendto(_socket, data.c_str(), data.length(), 0, (sockaddr*)&_addr, sizeof(_addr));

        if (num_send != -1)
        {
            Tracer::file_logger.PrintLine("UdpReporter num_send: " + std::to_string(num_send));
        }
        else
            Tracer::file_logger.PrintLine("UdpReporter num_send: nothing sent");
        close(_socket);
    }
}

const char* UdpReporter::_name() const
{
    return "UdpReporter";
}
