#include <iostream>
#include "ITracer.h"
using namespace OpenTracing;

ITracer::~ITracer()
{
#ifdef TRACER_DEBUG
    Php::out << "    ~ITracer addr: " << this << std::endl;
#endif
}