#define TMemory
//#define TBinary

#include <iostream>
#include <algorithm>
#include "JaegerTracer.h"
#include "Helper.h"
#include "TextCarrier.h"
#include "JaegerSpan.h"
#include "Tracer.h"

#include "thrift-gen/Agent.h"
#include "thrift-lib/transport/TBufferTransports.h"
#include "thrift-lib/transport/TSocket.h"
#include "thrift-lib/protocol/TCompactProtocol.h"
#include "thrift-lib/protocol/TBinaryProtocol.h"

using namespace OpenTracing;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::protocol;

JaegerTracer::~JaegerTracer()
{
    flush();
    delete _reporter;
    delete _sampler;
    delete _process;
    clearSpans();

    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("\t~JaegerTracer " + ss.str() + " destructor");
    }
#ifdef TRACER_DEBUG
    Php::out << "~JaegerTracer" << std::endl;
#endif
}

JaegerTracer::JaegerTracer(IReporter* reporter, ISampler* sampler) :
    _reporter{ reporter },
    _sampler{ sampler },
    _process{ nullptr },
    _isSampled{ false }
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("\tJaegerTracer " + ss.str() + " constructor");
    }
#ifdef TRACER_DEBUG
    Php::out << "JaegerTracer::JaegerTracer " << this << std::endl;
#endif
}

void JaegerTracer::init(const std::string& serviceName)
{
    if (!_spans.empty())
    {
        flush();
    }
    this->_process = new Process(serviceName);
}

ISpan* JaegerTracer::startSpan(const std::string& operationName, const Php::Value& options)
{
    Php::Value refType = nullptr;
    Php::Value parent = nullptr;
    SpanContext* context = nullptr;

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
#ifdef TRACER_DEBUG
            Php::out << "-received SpanContext" << std::endl;
#endif
            paramContext = (SpanContext*)parent.implementation();
        }
        else if (parent.instanceOf("ISpan"))
        {
            // allow span to be passed as reference, not just SpanContext
#ifdef TRACER_DEBUG
            Php::out << "-received Span" << std::endl;
#endif
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
        int flags = 0;
        if (this->_sampler->isSampled())
        {
            flags |= JaegerSpan::SAMPLED_FLAG;
        }
        // root traceID and spanID should match
        int64_t _id = Helper::generateId();
        context = new SpanContext(
            _id,
            _id,
            0,
            flags,
            refType
        );
    }

    JaegerSpan* span = new JaegerSpan(context, operationName);
    this->_spans_ref.push_back(Php::Object(span->_name(), span));

    this->_spans[span->_context->_spanId] = span;
    this->_activeSpans.push_back(span->_context->_spanId);
    if (!this->_isSampled && span->isSampled())
    {
        this->_isSampled = true;
    }
    return span;
}

ISpan* JaegerTracer::getCurrentSpan()
{
    std::ostringstream ss;
    {
        ss << "JaegerTracer " << this << " getCurrentSpan";
    }

    ISpan* span = nullptr;

    if (!_activeSpans.empty())
    {
        span = _spans[_activeSpans.back()];
    }

    //{
    //    ss <<
    //        " returned: " << span <<
    //        "\n\t\t\t\t\t\t\t\t\t\t\t\t\t_spans.size(): " << _spans.size() <<
    //        "\n\t\t\t\t\t\t\t\t\t\t\t\t\t_activeSpans.size(): " << _activeSpans.size();
    //}
    //Tracer::file_logger.PrintLine(ss.str());
    return span;
}

void JaegerTracer::finishSpan(ISpan* span, const Php::Value& endTime)
{
#ifdef TRACER_DEBUG
    Php::out << "JaegerTracer(" << span << ")::finishSpan " << std::endl;
    Php::out << "    _spans count:" << this->_spans.size() << std::endl;
    Php::out << "    time: " << dynamic_cast<JaegerSpan*>(span)->_endTime << std::endl;
    Php::out << "    spanid: " << dynamic_cast<JaegerSpan*>(span)->_context->_spanId << std::endl;
    Php::out << "    _logs.size: " << dynamic_cast<JaegerSpan*>(span)->_logs.size() << std::endl;
    Php::out << "    _tags.size: " << dynamic_cast<JaegerSpan*>(span)->_tags.size() << std::endl;
#endif
    /*
        if ($span instanceof SpanContext) {
            $span = $this->spans[(string)$span->spanId] ?? null;
        }
    */

    {
        std::ostringstream ss;
        ss <<
            "JaegerTracer " << this << " finishSpan " << span;
        Tracer::file_logger.PrintLine(ss.str());
    }

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
#ifdef TRACER_DEBUG
    Php::out << "JaegerTracer::flush " << std::endl;
#endif
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("JaegerTracer " + ss.str() + " flush");
    }

    if (!this->_isSampled)
        return;

    std::vector<std::string> data;
    if (strcmp(this->_reporter->_name(), "FileReporter") == 0)
    {
        /*
            $data = json_encode($this, JSON_PRETTY_PRINT);
        */
    }
    else if (strcmp(this->_reporter->_name(), "UdpReporter") == 0)
    {
#ifdef TRACER_DEBUG
        Php::out << "*** " << std::endl;
#endif

#ifdef TMemory
        /*TMemoryBuffer implementation*/
        std::shared_ptr<TMemoryBuffer> trans(new TMemoryBuffer());
        std::shared_ptr<TCompactProtocol> proto(new TCompactProtocol(trans));
        //std::shared_ptr<TBinaryProtocol> proto(new TBinaryProtocol(trans));
        std::shared_ptr<AgentClient> agent(new AgentClient(nullptr, proto));

        const int MAX_PACKET_SIZE = 65000;
        const int EMIT_BATCH_OVERHEAD = 33;
        const int MAXSPANBYTES = MAX_PACKET_SIZE - EMIT_BATCH_OVERHEAD;

        try
        {
            for (auto& iter : dynamic_cast<const JaegerTracer*>(this)->_spans)
            {
                std::string batchData{};
                LogCount incLogs{ LogCount::WHOLE };
                do
                {
                    ::Batch* batch = Helper::jaegerizeTracer(this, iter.second, incLogs);
                    agent->emitBatch(*batch);
                    batchData = trans->getBufferAsString();

                    trans->resetBuffer();
                    delete batch;
                    incLogs = static_cast<LogCount>(static_cast<int>(incLogs) - 1);
                    Tracer::file_logger.PrintLine("    ---batchData.length() " + std::to_string(batchData.length()));
                    if (incLogs == LogCount::ERROR)
                        break;
                } while (batchData.length() > MAXSPANBYTES);

                data.push_back(batchData);
            }
        }
        catch (...)
        {
            throw Php::Exception(" emitBatch exception");
        }
#endif

#ifdef TBinary
        /*TSocket implementation*/
        std::shared_ptr<TSocket> sock(new TSocket("127.0.0.1", 6832));
        //std::shared_ptr<TBufferedTransport> trans(new TBufferedTransport(sock));
        std::shared_ptr<TFramedTransport> trans(new TFramedTransport(sock));
        //std::shared_ptr<TCompactProtocol> proto(new TCompactProtocol(trans));
        std::shared_ptr<TBinaryProtocol> proto(new TBinaryProtocol(trans));
        //std::shared_ptr<TLEBinaryProtocol> proto(new TLEBinaryProtocol(trans));
        std::shared_ptr<AgentClient> agent(new AgentClient(nullptr, proto));
        trans->open();

        const ::Batch* batch = Helper::jaegerizeTracer(this);
        agent->emitBatch(*batch);

        trans->flush();
        trans->close();
        delete batch;
#endif
    }
    else
        return;

    for (auto& iter : data)
        this->_reporter->flush(iter);
    data.clear();
    this->clearSpans();
    Tracer::file_logger.PrintLine("\tflush end");
}

void JaegerTracer::clearSpans()
{
#ifdef TRACER_DEBUG
    Php::out << "\tclearSpans()" << std::endl;
#endif
    Tracer::file_logger.PrintLine("\tclearSpans start");

    // issue with PHP ref count, so it will be deleted after script finishes, or when worker terminates...
    // but here is a workaround
    //for (uint32_t iter = 0; iter < _spans_ref.size(); ++iter)
    //{
    //    _spans_ref[iter].~Value();
    //}
    for (auto& iter : _spans)
    {
        //delete iter.second; // this is handled by _spans_ref.~Value()
        iter.second = nullptr;
    }

    _spans_ref.clear();
    _spans.clear();
    _activeSpans.clear();
    Tracer::file_logger.PrintLine("\tclearSpans end");

}

const char* JaegerTracer::_name() const
{
    return "JaegerTracer";
}
