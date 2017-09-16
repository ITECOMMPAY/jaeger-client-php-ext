#include <iostream>
#include "ISampler.h"
#include "Tracer.h"
using namespace OpenTracing;

ISampler::~ISampler()
{
    {
        std::ostringstream ss;
        ss << "    ~ISampler addr: " << this;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
}
