#include "NoopTracer.h"

NoopTracer::~NoopTracer()
{
#ifdef TRACER_DEBUG
    Php::out << "NoopTracer::~NoopTracer addr: " << this << std::endl;
#endif
}

NoopTracer::NoopTracer()
{
#ifdef TRACER_DEBUG
    Php::out << "NoopTracer::NoopTracer" << std::endl;
#endif
}

void NoopTracer::init(const std::string& serviceName)
{
#ifdef TRACER_DEBUG
    Php::out << "NoopTracer::init" << std::endl;
#endif
}

ISpan * NoopTracer::startSpan(const std::string& operationName, const Php::Value& options)
{
#ifdef TRACER_DEBUG
    if (options.isNull())
    {
        Php::out << "options is null" << std::endl;
    }
    else
    {
        Php::out << "options is NOT null" << std::endl;

    }
    Php::out << "NoopTracer::startSpan, operationName = " << operationName << std::endl;
#endif
    return new NoopSpan();
}

ISpan * NoopTracer::getCurrentSpan()
{
    return new NoopSpan();
}

void NoopTracer::finishSpan(ISpan* span, const Php::Value& endTime)
{
    Php::out << "NoopTracer::finishSpan " << span->_name() << std::endl;
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
