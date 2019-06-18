#include <random>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include "Helper.h"
#include "NoopTracer.h"
#include "JaegerTracer.h"
#include "JaegerSpan.h"
#include "Tracer.h"

std::random_device rd;
std::uniform_int_distribution<int> dist{ 0, 99 };
std::uniform_int_distribution<unsigned int> dist_32bit{ 0x00000000, 0xFFFFFFFF };
std::uniform_int_distribution<int64_t> dist_64bit{ 0x0000000000000000, INT64_MAX };

const OpenTracing::TimeStamp OpenTracing::Helper::now() {
    OpenTracing::TimeStamp ts = { 0, "" };

    const int getTimeOfDayErrCode = OpenTracing::Helper::timeOfDayMicroSec(ts.usec);
    if (!getTimeOfDayErrCode) {
        return ts;
    }
    ts.errors += "gettimeofday error code: " + std::to_string(getTimeOfDayErrCode);

    const int clockGetTimeErrCode = OpenTracing::Helper::clockGetTimeMicroSec(ts.usec);
    if (!clockGetTimeErrCode) {
        return ts;
    }
    ts.errors += "clock_gettime error code: " + std::to_string(clockGetTimeErrCode);

    return ts;
}

const int OpenTracing::Helper::timeOfDayMicroSec(int64_t& microsec)
{
    struct timeval time { 0, 0 };
    int code = gettimeofday(&time, NULL);
    microsec = ((uint64_t)time.tv_sec * 1000 * 1000) + time.tv_usec;
    return code ? errno : 0;
}

const int OpenTracing::Helper::clockGetTimeMicroSec(int64_t& microsec)
{
    struct timespec time { 0, 0 };
    int code = clock_gettime(CLOCK_REALTIME, &time);
    microsec = ((uint64_t)time.tv_sec * 1000 * 1000) + time.tv_nsec / 1000;
    return code ? errno : 0;
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

#define IPVer2
const std::string OpenTracing::Helper::getCurrentIp()
{
    std::string retVal{};

#ifdef IPVer1
    const char* cmd = R"($(which ip) addr show | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -Ev '127.0.0.1|10.0.2.15')";

    FILE* ptr = popen(cmd, "r");
    if (ptr != NULL)
    {
        char buf[BUFSIZ];
        memset(buf, '\0', BUFSIZ);

        std::vector<std::string> ip;
        while (fgets(buf, BUFSIZ, ptr) != NULL)
        {
            ip.push_back(std::string(buf, strlen(buf) - 1));
            Php::out << ip.back() << std::endl;
        }

        // some logic to get ip
        // currently only last found
        retVal = ip.back();
    }
    pclose(ptr);
#endif

#ifdef IPVer2
    const char* cmd = R"(hostname -I)";

    FILE* ptr = popen(cmd, "r");
    if (ptr != NULL)
    {
        char buf[BUFSIZ];
        memset(buf, '\0', BUFSIZ);

        std::vector<std::string> ip;
        while (fgets(buf, BUFSIZ, ptr) != NULL)
        {
            std::istringstream iss(std::string(buf, strlen(buf) - 1));
            for (std::string s; iss >> s; )
            {
                ip.push_back(s);
            }
        }

        // some logic to get ip
        std::vector<std::string>::iterator iter = ip.begin();
        while (iter != ip.end())
        {
            if (*iter == "10.0.2.15")
            {
                iter = ip.erase(iter);
                continue;
            }
            iter++;
        }

        // currently only last found
        if (ip.size() > 0)
            retVal = ip.back();
    }

    pclose(ptr);
#endif

#ifdef IPVer3
    struct ifaddrs* ifaddr, * ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        //perror("getifaddrs");
        //exit(EXIT_FAILURE);
        //Php::out << "getifaddrs problem" << std::endl;
    }

    std::vector<std::string> ip;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if ( /*(strcmp(ifa->ifa_name,"wlan0")==0)&&( */ ifa->ifa_addr->sa_family == AF_INET) // )
        {
            if (s != 0)
            {
                //printf("getnameinfo() failed: %s\n", gai_strerror(s));
                //exit(EXIT_FAILURE);
                //Php::out << "getnameinfo() failed: " << gai_strerror(s) << std::endl;
                continue;
            }
            ip.push_back(std::string(host, strlen(host) - 1));
            Tracer::file_logger.PrintLine(ip.back());
            //printf("\tInterface : <%s>\n", ifa->ifa_name);
            //printf("\t  Address : <%s>\n", host);

            // some logic to get ip
            // currently only last found
            retVal = ip.back();
        }
    }

    freeifaddrs(ifaddr);
#endif

#ifdef IPVer4
    const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;

    struct sockaddr_in serv;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    //Socket could not be created
    if (sock < 0)
    {
        perror("Socket error");
    }

    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(google_dns_server);
    serv.sin_port = htons(dns_port);

    int err = connect(sock, (const struct sockaddr*) & serv, sizeof(serv));

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) & name, &namelen);

    char buffer[100];
    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);

    if (p != NULL)
    {
        retVal = std::string(buffer, strlen(buffer) - 1);
        Tracer::file_logger.PrintLine(retVal);
        //printf("Local ip is : %s \n", buffer);
    }
    else
    {
        //Some error
        //printf("Error number : %d . Error message : %s \n", errno, strerror(errno));
    }

    close(sock);
#endif

    return retVal;
}

const std::string OpenTracing::Helper::getHostName()
{
    char name[HOST_NAME_MAX];
    gethostname(name, HOST_NAME_MAX);
    return std::string{ name };
}

jaegertracing::thrift::Batch* OpenTracing::Helper::jaegerizeTracer(
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
    jaegertracing::thrift::Batch* batch = new jaegertracing::thrift::Batch();

    if (strcmp(tracer->_name(), "NoopTracer") == 0)
    {
    }
    else if (strcmp(tracer->_name(), "JaegerTracer") == 0)
    {
        std::vector<jaegertracing::thrift::Span> jaegerSpans;

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

jaegertracing::thrift::Span OpenTracing::Helper::jaegerizeSpan(
    const OpenTracing::ISpan* span,
    LogCount logLimit,
    JaegerizeVersion ver,
    size_t indStart,
    size_t indCount,
    size_t part,
    bool badLog
)
{
    jaegertracing::thrift::Span jaegerSpan;

    const JaegerSpan* _span = dynamic_cast<const JaegerSpan*>(span);
    bool partialSpan = false;
    bool keepId = false;

    if (_span->_tags.size() > 0)
    {
        std::vector<jaegertracing::thrift::Tag> tags;
        for (auto& iter : _span->_tags)
        {
            tags.push_back(jaegerizeTag(iter));
        }
        jaegerSpan.__set_tags(tags);
    }

    if (_span->_logs.size() > 0)
    {
        std::vector<jaegertracing::thrift::Log> logs;
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
                        if (iter_tags->_key == "log.level" && iter_tags->_vType == jaegertracing::thrift::TagType::DOUBLE && iter_tags->_vDouble == 1)
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

            if (part != 1 && (_span->_logs.size() == indStart + indCount))
            {
                keepId = true;
            }

        }
        jaegerSpan.__set_logs(logs);
    }

    jaegerSpan.__set_traceIdLow(_span->_context->_traceId);
    jaegerSpan.__set_traceIdHigh(0);
    partialSpan /*&& part != 1*/ && !keepId ? jaegerSpan.__set_spanId(Helper::generateId()) : jaegerSpan.__set_spanId(_span->_context->_spanId);
    jaegerSpan.__set_parentSpanId(_span->_context->_parentId);
    jaegerSpan.__set_operationName(_span->_operationName);
    jaegerSpan.__set_flags(_span->_context->_flags);
    (partialSpan && part != 1) ? jaegerSpan.__set_startTime(_span->_startTime + part) : jaegerSpan.__set_startTime(_span->_startTime);

    jaegerSpan.__set_duration(_span->_endTime != 0 ? _span->_endTime - _span->_startTime : Helper::now().usec - _span->_startTime);

    if (!_span->_context->_refType.isNull() && _span->_context->_traceId != _span->_context->_spanId)
    {
        std::vector<jaegertracing::thrift::SpanRef> references;

        jaegertracing::thrift::SpanRef spanRef;
        spanRef.__set_refType(static_cast<jaegertracing::thrift::SpanRefType::type>(_span->_context->_refType.numericValue()));
        spanRef.__set_traceIdLow(_span->_context->_traceId);
        spanRef.__set_traceIdHigh(0);
        spanRef.__set_spanId(_span->_context->_parentId);
        references.push_back(spanRef);

        jaegerSpan.__set_references(references);
    }

    return jaegerSpan;
}

jaegertracing::thrift::Process OpenTracing::Helper::jaegerizeProcess(const OpenTracing::Process* process)
{
    jaegertracing::thrift::Process jaegerProcess;
    std::vector<jaegertracing::thrift::Tag> tags;

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

jaegertracing::thrift::Tag OpenTracing::Helper::jaegerizeTag(const OpenTracing::Tag* tag)
{
    jaegertracing::thrift::Tag jaegerTag;
    jaegerTag.__set_key(tag->_key);
    jaegerTag.__set_vType(tag->_vType);

    switch (tag->_vType)
    {
    case jaegertracing::thrift::TagType::BOOL:
        jaegerTag.__set_vBool(tag->_vBool);
        break;
    case jaegertracing::thrift::TagType::DOUBLE:
        jaegerTag.__set_vDouble(tag->_vDouble);
        break;
    case jaegertracing::thrift::TagType::STRING:
        jaegerTag.__set_vStr(tag->_vStr);
        break;
    default:
        break;
    }

    return jaegerTag;
}

jaegertracing::thrift::Log OpenTracing::Helper::jaegerizeLog(const OpenTracing::Log* log)
{
    jaegertracing::thrift::Log jaegerLog;
    std::vector<jaegertracing::thrift::Tag> fields;

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
