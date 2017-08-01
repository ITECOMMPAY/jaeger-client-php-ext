#include "JaegerTracer.h"
#include "Helper.h"

void JaegerTracer::init(const std::string & serviceName)
{
    if (!_spans.empty())
    {
        flush();
    }
    this->_process = new Process(serviceName);
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

void JaegerTracer::finishSpan(ISpan* span, const Php::Value& endTime) const
{
    Php::out << "JaegerTracer::finishSpan " << span->_name() << std::endl;
    //if ($span instanceof SpanContext) {
    //    $span = $this->spans[(string)$span->spanId] ? ? null;
    //}
    //if ($span instanceof Span) {
    //    $span->endTime = $endTime ? ? Helper::now();
    //    if (($key = array_search((string)$span->context->spanId, $this->activeSpans)) != = false) {
    //        unset($this->activeSpans[$key]);
    //    }
    //}
    JaegerSpan* jaegerSpan = dynamic_cast<JaegerSpan*>(span);
    //jaegerSpan->_endTime = !endTime.isNull() ? static_cast<int64_t>(endTime) : Helper::now();
    
    //Php::out << "time: " << jaegerSpan->_endTime << std::endl;
  
    //jaegerSpan->_context->_spanId;
    //this->_activeSpans


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
