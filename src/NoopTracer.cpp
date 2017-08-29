#include <iostream>
#include "NoopTracer.h"
#include "NoopSpan.h"
#include "Tracer.h"
using namespace OpenTracing;

NoopTracer::~NoopTracer()
{
#ifdef TRACER_DEBUG
    Php::out << "NoopTracer::~NoopTracer addr: " << this << std::endl;
#endif
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("\t~NoopTracer " + ss.str() + " destructor");
    }
}

NoopTracer::NoopTracer()
{
#ifdef TRACER_DEBUG
    Php::out << "NoopTracer::NoopTracer" << std::endl;
#endif
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("\tNoopTracer " + ss.str() + " constructor");
    }
}

void NoopTracer::init(const std::string& serviceName)
{
#ifdef TRACER_DEBUG
    Php::out << "NoopTracer::init" << std::endl;
#endif
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " init, serviceName = " + serviceName);
    }
}

ISpan* NoopTracer::startSpan(const std::string& operationName, const Php::Value& options)
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
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " startSpan, operationName = " + operationName);
    }
    return new NoopSpan();
}

ISpan* NoopTracer::getCurrentSpan()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " getCurrentSpan");
    }

    return nullptr;//new NoopSpan();
}

void NoopTracer::finishSpan(ISpan* span, const Php::Value& endTime)
{
#ifdef TRACER_DEBUG
    Php::out << "NoopTracer::finishSpan " << span->_name() << std::endl;
#endif
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " finishSpan");
    }
}

void NoopTracer::inject(const Php::Value& context, Php::Value& carrier)
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " inject, carrier = ");// +carrier);
    }
}

SpanContext* NoopTracer::extract(const Php::Value& carrier) const
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " inject, carrier = ");// +carrier);
    }
    return nullptr;
}

void NoopTracer::flush()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " flush");
    }
}

const char* NoopTracer::_name() const
{
    return "NoopTracer";
}
