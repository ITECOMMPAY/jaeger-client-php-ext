#include <iostream>
#include "NoopTracer.h"
#include "NoopSpan.h"
#include "Tracer.h"
using namespace OpenTracing;

NoopTracer::~NoopTracer()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("\t~NoopTracer " + ss.str() + " destructor");
    }
}

NoopTracer::NoopTracer()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("\tNoopTracer " + ss.str() + " constructor");
    }
}

void NoopTracer::init(const std::string& serviceName)
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " init, serviceName = " + serviceName);
    }
}

ISpan* NoopTracer::startSpan(const std::string& operationName, const Php::Value& options)
{
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
    return new NoopSpan();
}

int64_t NoopTracer::getCurrentTraceId()
{
    return int64_t();
}

int64_t NoopTracer::getCurrentSpanId(ISpan* span)
{
    return int64_t();
}

int64_t NoopTracer::getCurrentParentId(ISpan* span)
{
    return int64_t();
}

void NoopTracer::finishSpan(ISpan* span, const Php::Value& endTime)
{
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
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " inject, carrier = ");
    }
}

SpanContext* NoopTracer::extract(const Php::Value& carrier) const
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopTracer " + ss.str() + " inject, carrier = ");
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
