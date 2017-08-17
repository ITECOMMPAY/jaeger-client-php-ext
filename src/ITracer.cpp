#include <iostream>
#include "ITracer.h"
#include "Tracer.h"
using namespace OpenTracing;

ITracer::~ITracer()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("    ~ITracer addr: " + ss.str());
    }
#ifdef TRACER_DEBUG
    Php::out << "    ~ITracer addr: " << this << std::endl;
#endif
}