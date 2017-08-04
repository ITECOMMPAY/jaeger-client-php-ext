#include "JaegerTracer.h"
#include "Helper.h"
#include "TextCarrier.h"

#include <algorithm>


JaegerTracer::~JaegerTracer()
{
    delete _reporter;
    delete _sampler;
    delete _process;
    for (auto& iter : _spans)
        delete iter.second;
    _spans.clear();
    _activeSpans.clear();

    Php::out << "~JaegerTracer" << std::endl;
}

JaegerTracer::JaegerTracer(IReporter * reporter, ISampler * sampler) :
    _reporter{ reporter },
    _sampler{ sampler },
    _isSampled{ false }
{
    Php::out << "JaegerTracer::JaegerTracer" << std::endl;
}

void JaegerTracer::init(const std::string& serviceName)
{
    if (!_spans.empty())
    {
        flush();
    }
    this->_process = new Process(serviceName);
}

ISpan * JaegerTracer::startSpan(const std::string& operationName, const Php::Value& options)
{
    Php::Value refType = nullptr;
    Php::Value parent = nullptr;
    SpanContext* context = nullptr;

    // check reference
    if (Php::array_key_exists("childOf", options))
    {
        parent = options["childOf"];
        //refType = \Eco\Tracer\Autogen\SpanRefType::CHILD_OF;
        refType = 0;    //todo
    }
    else if (Php::array_key_exists("followsFrom", options))
    {
        parent = options["followsFrom"];
        //refType = \Eco\Tracer\Autogen\SpanRefType::FOLLOWS_FROM;
        refType = 1;    //todo
    }

    if (!parent.isNull())
    {
        SpanContext* paramContext = nullptr;
        if (parent.instanceOf("SpanContext"))
        {
            Php::out << "-received SpanContext" << std::endl;
            paramContext = (SpanContext*)parent.implementation();
        }
        else if (parent.instanceOf("ISpan"))
        {
            // allow span to be passed as reference, not just SpanContext
            Php::out << "-received Span" << std::endl;
            ISpan* span = (ISpan*)parent.implementation();
            paramContext = dynamic_cast<JaegerSpan*>(span)->_context;
        }
        else
            throw Php::Exception("JaegerTracer::startSpan - no SpanContext nor ISpan passed");
        
        context = new SpanContext(
            paramContext->_traceId,
            Helper::generateId(),
            paramContext->_spanId,
            paramContext->_flags,
            refType
        );
    }
    else
    {
        //int64_t traceId = Helper::generateId();
        int flags = 0;
        if (this->_sampler->isSampled())
        {
            flags |= JaegerSpan::SAMPLED_FLAG;
        }

        context = new SpanContext(
            //traceId,
            //traceId,
            Helper::generateId(),
            Helper::generateId(),
            0,
            flags,
            refType
        );
    }

    JaegerSpan* span = new JaegerSpan(context, operationName);

    this->_spans[span->_context->_spanId] = span;
    this->_activeSpans.push_back(span->_context->_spanId);
    if (!this->_isSampled && span->isSampled())
    {
        this->_isSampled = true;
    }
    return span;
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
    //    $span = $this->spans[(string)$span->spanId] ?? null;
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
        Php::out << "spanid: " << jaegerSpan->_context->_spanId << std::endl;
        Php::out << "_activeSpans size: " << _activeSpans.size() << std::endl;
#endif
    }
}

void JaegerTracer::inject(const Php::Value& context, const std::string& format, std::string& carrier)
{
    SpanContext* paramContext = nullptr;

    if (context.instanceOf("SpanContext"))
    {
        paramContext = (SpanContext*)context.implementation();
    }
    else if (context.instanceOf("ISpan"))
    {
        ISpan* span = (ISpan*)context.implementation();
        paramContext = dynamic_cast<JaegerSpan*>(span)->_context;
    }
    else
        throw Php::Exception("JaegerTracer::startSpan - no SpanContext nor ISpan passed");
    

    // TODO do we need TextMap format? headers format?
    TextCarrier::inject(paramContext, carrier);
}

SpanContext* JaegerTracer::extract(const std::string& format, const std::string& carier) const
{
    // TODO do we need TextMap format? headers format?
    return TextCarrier::extract(carier);
}

void JaegerTracer::flush() const
{
}

const char * JaegerTracer::_name() const
{
    return "JaegerTracer";
}
