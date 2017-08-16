#include <iostream>
#include "ITracer.h"
#include "Logger.h"
using namespace OpenTracing;

extern Logger* file_logger;

ITracer::~ITracer()
{

    if (file_logger != nullptr)
    {
        std::ostringstream ss;
        ss << this;
        file_logger->PrintLine("    ~ITracer addr: " + ss.str());
    }
#ifdef TRACER_DEBUG
    Php::out << "    ~ITracer addr: " << this << std::endl;
#endif
}