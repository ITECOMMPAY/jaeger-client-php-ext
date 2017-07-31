#include "NoopTracer.h"

NoopTracer::~NoopTracer()
{
    Php::out << "NoopTracer::~NoopTracer addr: " << this << std::endl;
}

NoopTracer::NoopTracer()
{
    Php::out << "NoopTracer constructor" << std::endl;
}

void NoopTracer::init(const std::string & serviceName) const
{
}

ISpan * NoopTracer::startSpan(const std::string & operationName, const Php::Value & options) const
{
    if (options.isNull())
    {
        Php::out << "options is null" << std::endl;

    }
    else
    {
        Php::out << "options is NOT null" << std::endl;

    }
    Php::out << "NoopTracer::startSpan, operationName = " << operationName << std::endl;
    return new NoopSpan();
}

ISpan * NoopTracer::getCurrentSpan() const
{
    return new NoopSpan();
}

void NoopTracer::finishSpan() const
{
}

void NoopTracer::inject() const
{
}

void NoopTracer::extract() const
{
}

void NoopTracer::flush() const
{
}

const char * NoopTracer::_name() const
{
    return "NoopTracer";
}
