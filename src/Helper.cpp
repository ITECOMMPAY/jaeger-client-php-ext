#include <random>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include "Helper.h"
#include "NoopTracer.h"
#include "JaegerTracer.h"
#include "JaegerSpan.h"
#include "Logger.h"
extern OpenTracing::Logger* file_logger;

std::random_device rd;
std::default_random_engine re{ rd() };
std::uniform_int_distribution<int> dist{ 0, 99 };
std::uniform_int_distribution<unsigned int> dist_32bit{ 0x00000000, 0xFFFFFFFF };
std::uniform_int_distribution<uint64_t> dist_64bit{ 0x0000000000000000, 0xFFFFFFFFFFFFFFFF };

const int64_t OpenTracing::Helper::now()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t microsec = ((uint64_t)time.tv_sec * 1000 * 1000) + time.tv_usec;
    return microsec;
}

const uint64_t OpenTracing::Helper::generateId()
{
    return dist_64bit(re);
}

const int OpenTracing::Helper::genPercentage()
{
    return dist(re);
}

const std::string OpenTracing::Helper::getCurrentIp()
{
    //requires ifconfig to be installed
    const char *cmd = R"(ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1')";
    char buf[BUFSIZ];
    memset(buf, '\0', BUFSIZ);
    FILE *ptr;
    if ((ptr = popen(cmd, "r")) != NULL)
    {
        while (fgets(buf, BUFSIZ, ptr) != NULL);
        pclose(ptr);
    }

    return std::string{ buf };
}

const std::string OpenTracing::Helper::getHostName()
{
    char name[HOST_NAME_MAX];
    gethostname(name, HOST_NAME_MAX);
    return std::string{ name };
}

const ::Batch* OpenTracing::Helper::jaegerizeTracer(const OpenTracing::ITracer* tracer)
{
    ::Batch* batch = new ::Batch();

    if (strcmp(tracer->_name(), "NoopTracer") == 0)
    {
    }
    else if (strcmp(tracer->_name(), "JaegerTracer") == 0)
    {
        std::vector<::Span> jaegerSpans;
        file_logger->PrintLine("batch jaegerizeTracer");
        const JaegerTracer* jaegerTracer = dynamic_cast<const JaegerTracer*>(tracer);
        for (auto& iter : jaegerTracer->_spans)
        {
            if (dynamic_cast<JaegerSpan*>(iter.second) != nullptr)
            {
                file_logger->PrintLine("batch jaegerSpans");

                if (dynamic_cast<JaegerSpan*>(iter.second)->isSampled())
                {
                    jaegerSpans.push_back(jaegerizeSpan(iter.second));
                }
            }
            else
            {
                file_logger->PrintLine("batch jaegerizeTracer - no spans");
            }
        }

        batch->__set_process(jaegerizeProcess(jaegerTracer->_process));
        batch->__set_spans(jaegerSpans);
    }

    return batch;
}

::Span OpenTracing::Helper::jaegerizeSpan(const OpenTracing::ISpan* span)
{
    ::Span jaegerSpan;

    const JaegerSpan* _span = dynamic_cast<const JaegerSpan*>(span);

    if (_span->_tags.size() > 0)
    {
        std::vector<::Tag> tags;
        for (auto& iter : _span->_tags)
        {
            tags.push_back(jaegerizeTag(iter));
        }
        jaegerSpan.__set_tags(tags);
    }

    if (_span->_logs.size() > 0)
    {
        std::vector<::Log> logs;
        for (auto iter = _span->_logs.rbegin(); iter != _span->_logs.rend(); iter++)
        {
            if (logs.size() < Log::LOGS_THRESHOLD)
            {
                logs.push_back(jaegerizeLog(*iter));
            }
        }

        // the first one log will usually contain useful data, let's include it explicitly, if omitted
        if (logs.size() >= Log::LOGS_THRESHOLD && _span->_logs.size() > 0)
        {
            logs.push_back(jaegerizeLog(*_span->_logs.begin()));

            std::vector<Tag*> tags{ new Tag("info", "<---Too many logs, some were omitted in order to fit udp package size--->") };
            OpenTracing::Log* log = new OpenTracing::Log(tags, logs.back().timestamp);

            logs.push_back(jaegerizeLog(log));
            delete log;
            for (auto& iter : tags)
                delete iter;
        }

        jaegerSpan.__set_logs(logs);
    }

    jaegerSpan.__set_traceIdLow(_span->_context->_traceId);
    jaegerSpan.__set_traceIdHigh(0);
    jaegerSpan.__set_spanId(_span->_context->_spanId);
    jaegerSpan.__set_parentSpanId(_span->_context->_parentId);
    jaegerSpan.__set_operationName(_span->_operationName);
    jaegerSpan.__set_flags(_span->_context->_flags);
    jaegerSpan.__set_startTime(_span->_startTime);
    jaegerSpan.__set_duration(_span->_endTime != 0 ? _span->_endTime - _span->_startTime : Helper::now() - _span->_startTime);

    if (!_span->_context->_refType.isNull())
    {
        std::vector<SpanRef> references;

        SpanRef spanRef;
        spanRef.__set_refType(static_cast<SpanRefType::type>(_span->_context->_refType.numericValue()));
        spanRef.__set_traceIdLow(_span->_context->_traceId);
        spanRef.__set_traceIdHigh(0);
        spanRef.__set_spanId(_span->_context->_parentId);
        references.push_back(spanRef);

        jaegerSpan.__set_references(references);
    }

    return jaegerSpan;
}

::Process OpenTracing::Helper::jaegerizeProcess(const OpenTracing::Process* process)
{
    ::Process jaegerProcess;
    std::vector<::Tag> tags;

    if (process != nullptr)
    {
        for (auto& iter : process->_tags)
        {
            tags.push_back(jaegerizeTag(iter));
        }

        jaegerProcess.__set_tags(tags);
        jaegerProcess.__set_serviceName(process->_serviceName);
    }
    return jaegerProcess;
}

::Tag OpenTracing::Helper::jaegerizeTag(const OpenTracing::Tag* tag)
{
    ::Tag jaegerTag;
    jaegerTag.__set_key(tag->_key);
    jaegerTag.__set_vType(TagType::type::STRING);
    jaegerTag.__set_vStr(tag->_value);


    /*todo - because Tag is <string,string>
        switch (true) {
            case is_numeric($tag->value):
                $vType = Autogen\TagType::DOUBLE;
                $vKey = 'vDouble';
                $vVal = (float)$tag->value;
                break;

            case is_bool($tag->value):
                $vType = Autogen\TagType::BOOL;
                $vKey = 'vBool';
                $vVal = (bool)$tag->value;
                break;

            default:
                $vType = Autogen\TagType::STRING;
                $vKey = 'vStr';
                $vVal = (string)$tag->value;
                break;
        }
    */

    return jaegerTag;
}

::Log OpenTracing::Helper::jaegerizeLog(const OpenTracing::Log* log)
{
    ::Log jaegerLog;
    std::vector<::Tag> fields;

    for (auto& iter : log->_fields)
    {
        fields.push_back(jaegerizeTag(iter));
    }

    jaegerLog.__set_timestamp(log->_timestamp);
    jaegerLog.__set_fields(fields);
    return jaegerLog;
}
