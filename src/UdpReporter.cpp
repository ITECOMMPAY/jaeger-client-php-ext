#include <iostream>
#include "UdpReporter.h"
using namespace OpenTracing;

UdpReporter::~UdpReporter()
{
    Php::out << "~UdpReporter" << std::endl;
}

UdpReporter::UdpReporter(const Php::Value& params)
{
    Php::out << "UdpReporter::UdpReporter" << std::endl;
    /*
        $defaults = [
            'addr' => 'localhost',
            'port' => 6831
        ];

        $this->options = array_merge($defaults, $options);
    */

    //Php::Value defaults;
    //defaults["addr"] = "localhost";;
    //defaults["port"] = 6831;

    //if (params.empty())
    //{
    //    options = defaults;
    //}
    //else
    //{
    //    options = params[0];
    //    options = Php::call("array_merge", defaults, this->options);
    //}
}

void UdpReporter::flush() const
{
    Php::out << "    UdpReporter::flush" << std::endl;

    /*
        $socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
        if ($socket) {
            socket_sendto($socket, $data, strlen($data), 0, $this->options['addr'], $this->options['port']);
            socket_close($socket);
        }
    */
}

const char* UdpReporter::_name() const
{
    return "UdpReporter";
}
