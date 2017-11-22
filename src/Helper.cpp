#include <random>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include "Helper.h"
#include "NoopTracer.h"
#include "JaegerTracer.h"
#include "JaegerSpan.h"
#include "Tracer.h"

std::random_device rd;
std::uniform_int_distribution<int> dist{ 0, 99 };
std::uniform_int_distribution<unsigned int> dist_32bit{ 0x00000000, 0xFFFFFFFF };
std::uniform_int_distribution<int64_t> dist_64bit{ 0x0000000000000000, INT64_MAX };

const int64_t OpenTracing::Helper::now()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t microsec = ((uint64_t)time.tv_sec * 1000 * 1000) + time.tv_usec;
    return microsec;
}

const int64_t OpenTracing::Helper::generateId()
{
    static std::default_random_engine re{ rd() };
    return dist_64bit(re);
}

const int OpenTracing::Helper::genPercentage()
{
    static std::default_random_engine re{ rd() };
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

    return std::string{ buf,strlen(buf) - 1 };
}

const std::string OpenTracing::Helper::getHostName()
{
    char name[HOST_NAME_MAX];
    gethostname(name, HOST_NAME_MAX);
    return std::string{ name };
}

::Batch* OpenTracing::Helper::jaegerizeTracer(
    const OpenTracing::ITracer* tracer,
    const OpenTracing::ISpan* span,
    LogCount logLimit,
    JaegerizeVersion ver,
    size_t indStart,
    size_t indCount,
    size_t part,
    bool badLog
)
{
    ::Batch* batch = new ::Batch();

    if (strcmp(tracer->_name(), "NoopTracer") == 0)
    {
    }
    else if (strcmp(tracer->_name(), "JaegerTracer") == 0)
    {
        std::vector<::Span> jaegerSpans;

        {
            std::ostringstream ss;
            ss << "       jaegerize - Tracer " << tracer;
            Tracer::file_logger.PrintLine(ss.str(), true);
        }

        const JaegerTracer* jaegerTracer = dynamic_cast<const JaegerTracer*>(tracer);

        //all logs should exist
        //this could be in different spans with the same id
        if (ver == JaegerizeVersion::V2)
        {
            if (span != nullptr)
            {
                if (dynamic_cast<const JaegerSpan*>(span)->_context != nullptr)
                {
                    if (dynamic_cast<const JaegerSpan*>(span)->isDebug() || dynamic_cast<const JaegerSpan*>(span)->isSampled())
                    {
                        jaegerSpans.push_back(jaegerizeSpan(span, logLimit, ver, indStart, indCount, part, badLog));
                    }
                }
            }
        }
        else if (ver == JaegerizeVersion::V1)
        {
            //each Batch contains only one span
            if (span != nullptr)
            {
                if (dynamic_cast<const JaegerSpan*>(span)->_context != nullptr)
                {
                    if (dynamic_cast<const JaegerSpan*>(span)->isDebug() || dynamic_cast<const JaegerSpan*>(span)->isSampled())
                    {
                        jaegerSpans.push_back(jaegerizeSpan(span, logLimit));
                    }
                }
            }
            else
            {
                //all spans in one Batch
                for (auto& iter : jaegerTracer->_spans)
                {
                    {
                        std::ostringstream ss;
                        ss <<
                            "\t\tjaegerize - Tracer->_spans KEY: " << iter.first << "\n" <<
                            "\t\t\t\t\t\t\t\t\t\t\t\tjaegerize - Tracer->_spans VALUE: " << iter.second << "\n" <<
                            "\t\t\t\t\t\t\t\t\t\t\t\tjaegerize - Tracer->_spans->_context: " << dynamic_cast<JaegerSpan*>(iter.second)->_context;
                        Tracer::file_logger.PrintLine(ss.str());
                    }

                    if (iter.second != nullptr)
                    {
                        if (dynamic_cast<JaegerSpan*>(iter.second)->_context != nullptr)
                        {
                            if (dynamic_cast<const JaegerSpan*>(iter.second)->isDebug() || dynamic_cast<const JaegerSpan*>(iter.second)->isSampled())
                            {
                                jaegerSpans.push_back(jaegerizeSpan(iter.second));
                            }
                        }
                    }
                    else
                    {
                        Tracer::file_logger.PrintLine("\tjaegerizeTracer 0xBAD");
                    }
                }
            }
        }
        batch->__set_process(jaegerizeProcess(jaegerTracer->_process));
        batch->__set_spans(jaegerSpans);
    }

    return batch;
}

::Span OpenTracing::Helper::jaegerizeSpan(
    const OpenTracing::ISpan* span,
    LogCount logLimit,
    JaegerizeVersion ver,
    size_t indStart,
    size_t indCount,
    size_t part,
    bool badLog
)
{
    ::Span jaegerSpan;

    const JaegerSpan* _span = dynamic_cast<const JaegerSpan*>(span);
    bool partialSpan = false;

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
        if (ver == JaegerizeVersion::V1)
        {
            for (auto iter = _span->_logs.rbegin(); iter != _span->_logs.rend(); iter++)
            {
                if ((logLimit == LogCount::WHOLE) ||
                    (logLimit != LogCount::WHOLE && logs.size() < (_span->_logs.size() / static_cast<size_t>(LogCount::WHOLE) * static_cast<size_t>(logLimit))))
                {
                    logs.push_back(jaegerizeLog(*iter));
                }
            }

            // the first one log will usually contain useful data, let's include it explicitly, if omitted
            if (logLimit != LogCount::WHOLE && logs.size() == /*>=*/ (_span->_logs.size() / static_cast<size_t>(LogCount::WHOLE) * static_cast<size_t>(logLimit)))
            {
                logs.push_back(jaegerizeLog(*_span->_logs.begin()));
                // there could be logs with log.level=1 tags in ommited ones, so let's include them also
                for (size_t iter = 1, iter_bound = _span->_logs.size() - logs.size() + 1; iter < iter_bound; iter++)
                {
                    for (auto& iter_tags : _span->_logs[iter]->_fields)
                    {
                        if (iter_tags->_key == "log.level" && iter_tags->_vType == TagType::DOUBLE && iter_tags->_vDouble == 1)
                        {
                            logs.push_back(jaegerizeLog(_span->_logs[iter]));
                        }
                    }
                }

                std::vector<Tag*> tags;
                tags.push_back(new Tag(std::string{ "info" }, std::string{ "<---Too many logs, some were omitted in order to fit UDP packet size--->" }));
                OpenTracing::Log* log = new OpenTracing::Log(tags, logs.back().timestamp);

                logs.push_back(jaegerizeLog(log));
                delete log;
                for (auto& iter : tags)
                    delete iter;
            }
        }
        if (ver == JaegerizeVersion::V2)
        {
            for (size_t iter = indStart; !badLog && iter < indStart + indCount; iter++)
            {
                logs.push_back(jaegerizeLog(_span->_logs[iter]));
            }

            // the first one log should contain information if logs were splitted to different spans
            if (
                ((logLimit == LogCount::WHOLE) && (_span->_logs.size() != indStart + indCount)) ||
                (indStart != 0) ||
                (indCount != _span->_logs.size()))
            {
                partialSpan = true;
                std::vector<Tag*> tags;
                tags.push_back(new Tag(std::string{ "info" }, std::string{ "<---Logs were divided into several spans. PART " + std::to_string(part) + ":--->" }));
                OpenTracing::Log* log = nullptr;

                if (!badLog)
                    log = new OpenTracing::Log(tags, logs.front().timestamp - 1);
                else
                    log = new OpenTracing::Log(tags, _span->_logs[indStart]->_timestamp - 1);
                logs.push_back(jaegerizeLog(log));

                delete log;
                for (auto& iter : tags)
                    delete iter;

                if (badLog)
                {
                    tags.push_back(new Tag(std::string{ "info" }, std::string{ "<---One log was ommited due its size, refer to the logging system--->" }));
                    log = new OpenTracing::Log(tags, logs.front().timestamp + 1);
                    logs.push_back(jaegerizeLog(log));
                    delete log;
                    for (auto& iter : tags)
                        delete iter;

                }
            }
        }
        jaegerSpan.__set_logs(logs);
    }

    jaegerSpan.__set_traceIdLow(_span->_context->_traceId);
    jaegerSpan.__set_traceIdHigh(0);
    partialSpan && part != 1 ? jaegerSpan.__set_spanId(Helper::generateId()) : jaegerSpan.__set_spanId(_span->_context->_spanId);
    jaegerSpan.__set_parentSpanId(_span->_context->_parentId);
    jaegerSpan.__set_operationName(_span->_operationName);
    jaegerSpan.__set_flags(_span->_context->_flags);
    partialSpan && part != 1 ? jaegerSpan.__set_startTime(_span->_startTime + part) : jaegerSpan.__set_startTime(_span->_startTime);
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
    jaegerTag.__set_vType(tag->_vType);

    switch (tag->_vType)
    {
    case TagType::BOOL:
        jaegerTag.__set_vBool(tag->_vBool);
        break;
    case TagType::DOUBLE:
        jaegerTag.__set_vDouble(tag->_vDouble);
        break;
    case TagType::STRING:
        jaegerTag.__set_vStr(tag->_vStr);
        break;
    default:
        break;
    }

    return jaegerTag;
}

::Log OpenTracing::Helper::jaegerizeLog(const OpenTracing::Log* log)
{
    ::Log jaegerLog;
    std::vector<::Tag> fields;

    std::ostringstream ss;
    {
        ss << "jaegerize - Log fields size: " << log->_fields.size();
        Tracer::file_logger.PrintLine(ss.str(), true);
        ss.str("");
        ss.clear();
    }

    for (auto& iter : log->_fields)
    {
        fields.push_back(jaegerizeTag(iter));
    }

    jaegerLog.__set_timestamp(log->_timestamp);
    jaegerLog.__set_fields(fields);

    {
        ss << "\tjaegerLog.fields.size(): " << jaegerLog.fields.size();
        for (auto& iter : jaegerLog.fields)
        {
            ss << "\niter.key: " << iter.key << " iter.vStr: " << iter.vStr << "\n";
        }
        Tracer::file_logger.PrintLine(ss.str(), true);
    }

    return jaegerLog;
}
