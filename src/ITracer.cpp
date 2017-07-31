#include "ITracer.h"

ITracer::~ITracer()
{
    Php::out<<"    ITracer::~ITracer addr: " << this << std::endl;
}