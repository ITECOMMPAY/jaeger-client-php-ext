#include <iostream>
#include "PercentageSampler.h"
#include "Helper.h"
#include "Tracer.h"
using namespace OpenTracing;

PercentageSampler::~PercentageSampler()
{
    {
        std::ostringstream ss;
        ss << "~PercentageSampler " << this;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
}

PercentageSampler::PercentageSampler(const Php::Value& params) :
    _value{ 50 }
{

    if (!params.isNull())
        _value = params["percents"];

    {
        std::ostringstream ss;
        ss <<
            "PercentageSampler::PercentageSampler " <<
            "_value " << _value;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
}

bool PercentageSampler::isSampled()
{
    Tracer::file_logger.PrintLine("    PercentageSampler::isSampled", true);

    return Helper::genPercentage() < this->_value;
}
