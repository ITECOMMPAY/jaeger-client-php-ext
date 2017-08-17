#include <iostream>
#include <string>
#include "UdpReporter.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sstream>
using namespace OpenTracing;

UdpReporter::~UdpReporter()
{
#ifdef TRACER_DEBUG
    Php::out << "~UdpReporter" << std::endl;
#endif
}

UdpReporter::UdpReporter(const Php::Value& params)
{
#ifdef TRACER_DEBUG
    Php::out << "UdpReporter::UdpReporter" << std::endl;
#endif

    Php::Value defaults;
    defaults["addr"] = "localhost";
    defaults["port"] = 6831;

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
#ifdef TRACER_DEBUG
    Php::out << "    UdpReporter::flush" << std::endl;
#endif

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
        //sendto(_socket, (data + '\n').c_str(), data.length() + 1, 0, (sockaddr*)&_addr, sizeof(_addr));
        sendto(_socket, (data).c_str(), data.length(), 0, (sockaddr*)&_addr, sizeof(_addr));
#ifdef TRACER_DEBUG
        ssize_t num_send = {};
        if (num_send != -1)
            Php::out << "    num_send: " << num_send << std::endl;
        else
            Php::out << "    nothing sent, some error occures" << std::endl;
#endif
        close(_socket);
    }
}

const char* UdpReporter::_name() const
{
    return "UdpReporter";
}
