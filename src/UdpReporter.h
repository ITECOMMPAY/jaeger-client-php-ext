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

        void flush(const std::string& data) const;
        const char* _name() const;
    };
}
#endif /* UDPREPORTER_H */

