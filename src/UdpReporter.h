#ifndef UDPREPORTER_H
#define UDPREPORTER_H
#include <phpcpp.h>
#include <iostream>
#include "IReporter.h"

namespace OpenTracing
{
    class UdpReporter : public IReporter
    {
    public:
        ~UdpReporter()
        {
            Php::out << "~UdpReporter" << std::endl;
        }

        UdpReporter(const Php::Value& params)
        {
            Php::out << "UdpReporter::UdpReporter" << std::endl;
            //Php::Value defaults;
            //defaults["addr"] = "localhost";;
            //defaults["port"] = 6831;

            /*if (params.empty())
            {
                options = defaults;
            }
            else
            {
                options = params[0];
                options = Php::call("array_merge", defaults, this->options);
            }*/
        }
        UdpReporter() = delete;
        UdpReporter(const UdpReporter&) = delete;
        UdpReporter(const UdpReporter&&) = delete;
        UdpReporter& operator=(const UdpReporter&) = delete;
        UdpReporter& operator=(const UdpReporter&&) = delete;


        void flush() const
        {
            Php::out << "    UdpReporter::flush" << std::endl;
            /*$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
            if ($socket) {
                socket_sendto($socket, $data, strlen($data), 0, $this->options['addr'], $this->options['port']);
                socket_close($socket);
            }*/
        }

        const char* _name() const
        {
            return "UdpReporter";
        }
    };
}
#endif /* UDPREPORTER_H */

