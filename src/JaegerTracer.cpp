#include "JaegerTracer.h"

void JaegerTracer::init(const std::string & serviceName) const
{
    if (!_spans.empty())
    {
        flush();
    }

    //this->print();
}

ISpan * JaegerTracer::startSpan(const std::string & operationName, const Php::Value & options) const
{
    if (options.isNull())
    {
        Php::out << "options is null" << std::endl;

    }
    else
    {
        Php::out << "options is NOT null" << std::endl;

    }

    Php::out << "JaegerTracer::startSpan, operationName = " << operationName << std::endl;
    return new JaegerSpan();
}

ISpan * JaegerTracer::getCurrentSpan() const
{
    //$spanKey = end($this->activeSpans);
    //if ($spanKey) {
    //    return $this->spans[$spanKey];
    //}
    //return false;

    return nullptr;
}

void JaegerTracer::finishSpan() const
{
}

void JaegerTracer::inject() const
{
}

void JaegerTracer::extract() const
{
}

void JaegerTracer::flush() const
{
}

const char * JaegerTracer::_name() const
{
    return "JaegerTracer";
}
