#ifndef PERCENTAGESAMPLER_H
#define PERCENTAGESAMPLER_H

#include <phpcpp.h>
#include "ISampler.h"

namespace OpenTracing
{
    class PercentageSampler : public ISampler
    {
    private:
        int _value;
    public:
        ~PercentageSampler();
        PercentageSampler(const Php::Value& params = nullptr);
        PercentageSampler(const PercentageSampler&) = delete;
        bool isSampled();
    };
}

#endif /* PERCENTAGESAMPLER_H */
