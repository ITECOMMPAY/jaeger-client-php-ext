#include <iostream>
#include "ISpan.h"
using namespace OpenTracing;

ISpan::~ISpan()
{
    Php::out << "    ~ISpan addr: " << this << std::endl;
}
