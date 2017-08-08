#ifndef UDPREPORTER_H
#define UDPREPORTER_H

#include <phpcpp.h>
#include "IReporter.h"

namespace OpenTracing
{
    class UdpReporter : public IReporter
    {
    public:
        ~UdpReporter();
        UdpReporter(const Php::Value& params);
        //UdpReporter() = delete;
        //UdpReporter(const UdpReporter&) = delete;
        //UdpReporter(const UdpReporter&&) = delete;
        //UdpReporter& operator=(const UdpReporter&) = delete;
        //UdpReporter& operator=(const UdpReporter&&) = delete;
        void flush() const;
        const char* _name() const;
    };
}
#endif /* UDPREPORTER_H */

