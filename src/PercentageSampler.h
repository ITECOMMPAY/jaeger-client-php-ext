#ifndef PERCENTAGESAMPLER_H
#define PERCENTAGESAMPLER_H

#include <phpcpp.h>
#include <iostream>
#include "ISampler.h"

class PercentageSampler : public ISampler
{
private:
    int _value;
public:
    ~PercentageSampler()
    {
        Php::out << "PercentageSampler::~PercentageSampler" << std::endl;
    }

    PercentageSampler(const Php::Value &params = nullptr);

    bool isSampled();
};

#endif /* PERCENTAGESAMPLER_H */
