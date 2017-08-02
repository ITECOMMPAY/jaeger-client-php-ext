#include "JaegerTracer.h"
#include "Helper.h"

#include <algorithm>


void JaegerTracer::init(const std::string & serviceName)
{
    if (!_spans.empty())
    {
        flush();
    }
    this->_process = new Process(serviceName);
}

ISpan * JaegerTracer::startSpan(const std::string & operationName, const Php::Value & options)
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

ISpan * JaegerTracer::getCurrentSpan()
{
    if (!_activeSpans.empty())
    {
        return _spans[_activeSpans.back()];
    }
    return nullptr;
}

void JaegerTracer::finishSpan(ISpan* span, const Php::Value& endTime)
{
#ifdef TRACER_DEBUG
    Php::out << "JaegerTracer::finishSpan " << span->_name() << std::endl;
#endif
    //if ($span instanceof SpanContext) {
    //    $span = $this->spans[(string)$span->spanId] ? ? null;
    //}
    JaegerSpan* jaegerSpan = dynamic_cast<JaegerSpan*>(span);
    if (jaegerSpan != nullptr)
    {
        jaegerSpan->_endTime = !endTime.isNull() ? static_cast<int64_t>(endTime) : Helper::now();

        if (!_activeSpans.empty())
        {
            auto findSpanId = std::find(_activeSpans.begin(), _activeSpans.end(), jaegerSpan->_context->_spanId);
            if (findSpanId != _activeSpans.end())
                _activeSpans.erase(findSpanId);
        }


#ifdef TRACER_DEBUG
        Php::out << "time: " << jaegerSpan->_endTime << std::endl;
        //Php::out << "spanid: " << jaegerSpan->_context->_spanId << std::endl;
#endif
    }
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
