#ifndef IREPORTER_H
#define IREPORTER_H

#include <phpcpp.h>

namespace OpenTracing
{
    class IReporter
    {
    public:
        virtual ~IReporter() = 0;
        virtual void flush(const uint8_t* data, int len) const = 0;
        virtual const char* _name() const = 0;
    };
}
#endif /* IREPORTER_H */

