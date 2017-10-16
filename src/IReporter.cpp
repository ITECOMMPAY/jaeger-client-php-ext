#include <iostream>
#include "IReporter.h"
#include "Tracer.h"
using namespace OpenTracing;

IReporter::~IReporter()
{
    {
        std::ostringstream ss;
        ss << "    ~IReporter addr: " << this;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
}

