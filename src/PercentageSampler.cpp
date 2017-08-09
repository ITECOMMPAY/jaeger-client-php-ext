#include <iostream>
#include "PercentageSampler.h"
#include "Helper.h"
using namespace OpenTracing;

PercentageSampler::~PercentageSampler()
{
#ifdef TRACER_DEBUG
    Php::out << "~PercentageSampler " << this << std::endl;
#endif    
}

PercentageSampler::PercentageSampler(const Php::Value& params) :
    _value{ 50 }
{
#ifdef TRACER_DEBUG
    Php::out << "PercentageSampler::PercentageSampler" << std::endl;
#endif    
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
#ifdef TRACER_DEBUG
    Php::out << "    PercentageSampler::isSampled" << std::endl;
#endif    
    return Helper::genPercentage() < this->_value;
}
