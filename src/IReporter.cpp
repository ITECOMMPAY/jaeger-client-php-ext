#include <iostream>
#include "IReporter.h"
using namespace OpenTracing;

IReporter::~IReporter()
{
#ifdef TRACER_DEBUG
    Php::out << "    ~IReporter addr: " << this << std::endl;
#endif
}

