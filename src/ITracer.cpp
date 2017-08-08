#include <iostream>
#include "ITracer.h"
using namespace OpenTracing;

ITracer::~ITracer()
{
    Php::out << "    ~ITracer addr: " << this << std::endl;
}