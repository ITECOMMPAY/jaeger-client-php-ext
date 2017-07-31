#include "ITracer.h"

ITracer::~ITracer()
{
    Php::out << "    ~ITracer addr: " << this << std::endl;
}