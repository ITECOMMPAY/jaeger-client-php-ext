#include <iostream>
#include "IReporter.h"
using namespace OpenTracing;

IReporter::~IReporter()
{
    Php::out << "    ~IReporter addr: " << this << std::endl;
}

