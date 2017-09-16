#include <iostream>
#include "ISpan.h"
#include "Tracer.h"
using namespace OpenTracing;

ISpan::~ISpan()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("    ~ISpan addr: " + ss.str());
    }
}
