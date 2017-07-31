#include "PercentageSampler.h"

PercentageSampler::~PercentageSampler()
{
    Php::out << "~PercentageSampler" << std::endl;
}

PercentageSampler::PercentageSampler(const Php::Value & params) :
    _value{ 50 }
{
    srand(time(NULL));

    Php::out << "PercentageSampler::PercentageSampler" << std::endl;
    if (!params.isNull())
    {
        _value = params["percents"];
#ifdef TRACER_DEBUG
        Php::out << "_value " << _value << std::endl;
#endif
    }
}

bool PercentageSampler::isSampled()
{
    Php::out << "PercentageSampler::isSampled" << std::endl;
    return rand() % 100 < this->_value;
}
