#include <iostream>
#include "PercentageSampler.h"
#include "Helper.h"
using namespace OpenTracing;

PercentageSampler::~PercentageSampler()
{
    Php::out << "~PercentageSampler " << this << std::endl;
}

PercentageSampler::PercentageSampler(const Php::Value& params) :
    _value{ 50 }
{
    Php::out << "PercentageSampler::PercentageSampler" << std::endl;
    if (!params.isNull())
    {
        _value = params["percents"];
#ifdef TRACER_DEBUG
        Php::out << "PercentageSampler::_value " << _value << std::endl;
#endif
    }
}

bool PercentageSampler::isSampled()
{
    Php::out << "    PercentageSampler::isSampled" << std::endl;
    return Helper::genPercentage() < this->_value;
}
