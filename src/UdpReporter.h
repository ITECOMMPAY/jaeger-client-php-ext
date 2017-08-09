#ifndef UDPREPORTER_H
#define UDPREPORTER_H

#include <phpcpp.h>
#include "IReporter.h"

namespace OpenTracing
{
    class UdpReporter : public IReporter
    {
    private:
        Php::Value _options;
    public:
        ~UdpReporter();
        UdpReporter(const Php::Value& params = nullptr);

        //UdpReporter() = delete;
        //UdpReporter(const UdpReporter&) = delete;
        //UdpReporter(const UdpReporter&&) = delete;
        //UdpReporter& operator=(const UdpReporter&) = delete;
        //UdpReporter& operator=(const UdpReporter&&) = delete;
        void flush(const uint8_t* data, int len) const;
        const char* _name() const;
    };
}
#endif /* UDPREPORTER_H */

