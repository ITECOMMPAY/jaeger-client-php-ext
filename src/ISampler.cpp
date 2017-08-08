#include <iostream>
#include "ISampler.h"
using namespace OpenTracing;

ISampler::~ISampler()
{
    Php::out << "    ~ISampler addr: " << this << std::endl;
}
