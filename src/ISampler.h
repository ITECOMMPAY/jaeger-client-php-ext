#ifndef ISAMPLER_H
#define ISAMPLER_H

#include <phpcpp.h>

namespace OpenTracing
{
    class ISampler
    {
    public:
        virtual ~ISampler() = 0;
        virtual bool isSampled() = 0;
    };
}
#endif /* ISAMPLER_H */

