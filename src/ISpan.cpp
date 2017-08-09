#include <iostream>
#include "ISpan.h"
using namespace OpenTracing;

ISpan::~ISpan()
{
#ifdef TRACER_DEBUG
    Php::out << "    ~ISpan addr: " << this << std::endl;
#endif
}
