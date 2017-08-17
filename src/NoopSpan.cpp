#include <iostream>
#include "NoopSpan.h"
using namespace OpenTracing;

NoopSpan::NoopSpan()
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::NoopSpan addr: " << this << std::endl;
#endif
}

NoopSpan::~NoopSpan()
{
#ifdef TRACER_DEBUG
    Php::out << "~NoopSpan addr: " << this << std::endl;
#endif
}

void NoopSpan::addTags(Php::Parameters& tags)
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::addTags addr: " << this << std::endl;
#endif
}

void NoopSpan::addLogs(Php::Parameters& logs)
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::addLogs addr: " << this << std::endl;
#endif
}

const char* NoopSpan::_name() const
{
    return "NoopSpan";
}
