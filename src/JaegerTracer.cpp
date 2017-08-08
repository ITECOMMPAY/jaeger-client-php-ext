#include <iostream>
#include <algorithm>
#include "JaegerTracer.h"
#include "Helper.h"
#include "TextCarrier.h"
#include "JaegerSpan.h"

#include "thrift-gen/Agent.h"
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TCompactProtocol.h>

using namespace OpenTracing;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::protocol;

JaegerTracer::~JaegerTracer()
{
    delete _reporter;
    delete _sampler;
    delete _process;
    clearSpans();
    Php::out << "~JaegerTracer" << std::endl;
}

JaegerTracer::JaegerTracer(IReporter* reporter, ISampler* sampler) :
    _reporter{ reporter },
    _sampler{ sampler },
    _isSampled{ false }
{
    Php::out << "JaegerTracer::JaegerTracer " << this << std::endl;
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
        refType = SpanRefType::type::CHILD_OF;
    }
    else if (Php::array_key_exists("followsFrom", options))
    {
        parent = options["followsFrom"];
        refType = SpanRefType::type::FOLLOWS_FROM;
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
    Php::out << "JaegerTracer(" << span << ")::finishSpan " << std::endl;
    Php::out << "    _spans count:" << this->_spans.size() << std::endl;
    //Php::out << "    time: " << jaegerSpan->_endTime << std::endl;
    Php::out << "    spanid: " << dynamic_cast<JaegerSpan*>(span)->_context->_spanId << std::endl;
    Php::out << "    _logs.size: " << dynamic_cast<JaegerSpan*>(span)->_logs.size() << std::endl;
    Php::out << "    _tags.size: " << dynamic_cast<JaegerSpan*>(span)->_tags.size() << std::endl;
#endif
    /*
        if ($span instanceof SpanContext) {
            $span = $this->spans[(string)$span->spanId] ?? null;
        }
    */
    JaegerSpan* jaegerSpan = dynamic_cast<JaegerSpan*>(span);
    if (jaegerSpan != nullptr)
    {
        jaegerSpan->_endTime = !endTime.isNull() ? static_cast<int64_t>(endTime) : Helper::now();

#ifdef TRACER_DEBUG
        Php::out << "    span to remove: " << jaegerSpan->_context->_spanId << std::endl;
        Php::out << "    _activeSpans BEFORE(size: " << this->_activeSpans.size() << "): " << std::endl;
        for (auto& iter : this->_activeSpans)
        {
            Php::out << "        " << iter << std::endl;
        }
#endif

        if (!_activeSpans.empty())
        {
            auto findSpanId = std::find(_activeSpans.begin(), _activeSpans.end(), jaegerSpan->_context->_spanId);
            if (findSpanId != _activeSpans.end())
                _activeSpans.erase(findSpanId);
        }

#ifdef TRACER_DEBUG
        Php::out << "    _activeSpans AFTER(size: " << this->_activeSpans.size() << "): " << std::endl;
        for (auto& iter : this->_activeSpans)
        {
            Php::out << "        " << iter << std::endl;
        }
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

void JaegerTracer::flush()
{
    Php::out << "JaegerTracer::flush " << std::endl;

    if (!this->_isSampled)
        return;

    if (strcmp(this->_reporter->_name(), "FileReporter") == 0)
    {
        //$data = json_encode($this, JSON_PRETTY_PRINT);
    }
    else if (strcmp(this->_reporter->_name(), "UdpReporter") == 0)
    {
        Php::out << "*** " << std::endl;

        boost::shared_ptr<TMemoryBuffer> trans(new TMemoryBuffer());
        boost::shared_ptr<TCompactProtocol> proto(new TCompactProtocol(trans));
        boost::shared_ptr<AgentConcurrentClient> agent(new AgentConcurrentClient(nullptr, proto));


        const ::Batch* batch = Helper::jaegerizeTracer(this);
        agent->emitBatch(*batch);
        //uint8_t* buf;
        //uint32_t sz;
        //trans->getBuffer(&buf, &sz);
        std::string data = trans->getBufferAsString();

        Php::out << data << std::endl;



        /*
        $trans = new \Thrift\Transport\TMemoryBuffer();
        $proto = new \Thrift\Protocol\TCompactProtocol($trans);
        $agent = new \Eco\Tracer\Autogen\AgentClient(null, $proto);

        $batch = Helper::jaegerizeTracer($this);

        $agent->emitBatch($batch);
        $data = $trans->getBuffer();

        */
    }
    else
        return;

    this->_reporter->flush();//$data
    this->clearSpans();
}

void JaegerTracer::clearSpans()
{
    Php::out << "\tclearSpans()" << std::endl;
    //for (auto& iter : _spans)
    {
        //issue with PHP ref count, so it will be deleted after script finishes
        //will be handled by PHP __destruct of a JaegerSpan

        //delete iter.second; 
    }
    _spans.clear();
    _activeSpans.clear();
}

const char* JaegerTracer::_name() const
{
    return "JaegerTracer";
}
