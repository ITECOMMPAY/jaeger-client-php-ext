#include <iostream>
#include "ISampler.h"
using namespace OpenTracing;

ISampler::~ISampler()
{
#ifdef TRACER_DEBUG
    Php::out << "    ~ISampler addr: " << this << std::endl;
#endif
}
