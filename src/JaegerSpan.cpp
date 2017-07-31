#include "JaegerSpan.h"

JaegerSpan::JaegerSpan()
{
    Php::out << "JaegerSpan::JaegerSpan addr: " << this << std::endl;
}

JaegerSpan::~JaegerSpan()
{
    Php::out << "JaegerSpan::~JaegerSpan addr: " << this << std::endl;
}

void JaegerSpan::addTags(Php::Parameters & params)
{
    Php::out << "JaegerSpan::addTags addr: " << this << std::endl;
}

void JaegerSpan::addLogs(Php::Parameters & logs)
{
}

const char * JaegerSpan::_name() const
{
    return "JaegerSpan";
}
