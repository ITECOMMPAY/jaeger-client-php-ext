#include "NoopSpan.h"

NoopSpan::NoopSpan()
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::NoopSpan addr: " << this << std::endl;
#endif
}

NoopSpan::~NoopSpan()
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::~NoopSpan addr: " << this << std::endl;
#endif
}

void NoopSpan::addTags(Php::Parameters & params)
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::addTags addr: " << this << std::endl;
#endif
}

void NoopSpan::addLogs(Php::Parameters & logs)
{
}

const char * NoopSpan::_name() const
{
    return "NoopSpan";
}
