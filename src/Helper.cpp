#include <sys/time.h>
#include "Helper.h"
#include "NoopTracer.h"
#include "JaegerTracer.h"
using namespace OpenTracing;

std::random_device rd;
std::default_random_engine re{ rd() };
std::uniform_int_distribution<int> dist{ 0, 99 };
std::uniform_int_distribution<unsigned int> dist_32bit{ 0x00000000, 0xFFFFFFFF };
std::uniform_int_distribution<uint64_t> dist_64bit{ 0x0000000000000000, 0xFFFFFFFFFFFFFFFF };

const int64_t Helper::now()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t microsec = ((uint64_t)time.tv_sec * 1000 * 1000) + time.tv_usec;
    return microsec;
}

const uint64_t Helper::generateId()
{
    return dist_64bit(re);
}

const std::string Helper::getCurrentIp()
{
    const char *cmd = "ifconfig | grep -Eo 'inet (addr:)?([0-9]*\\.){3}[0-9]*' | grep -Eo '([0-9]*\\.){3}[0-9]*' | grep -v '127.0.0.1'";
    char buf[BUFSIZ];
    FILE *ptr;
    if ((ptr = popen(cmd, "r")) != NULL)
    {
        while (fgets(buf, BUFSIZ, ptr) != NULL);
        pclose(ptr);
    }

    return std::string{ buf };
}

const std::string Helper::getHostName()
{
    char name[HOST_NAME_MAX];
    gethostname(name, HOST_NAME_MAX);
    return std::string{ name };
}

const ::Batch* OpenTracing::Helper::jaegerizeTracer(OpenTracing::ITracer* tracer)
{
    ::Batch* batch = new ::Batch();

    if (strcmp(tracer->_name(), "NoopTracer") == 0)
    {
    }
    else if (strcmp(tracer->_name(), "JaegerTracer") == 0)
    {
        std::vector<::Span> jaegerSpans;

        JaegerTracer* jaegerTracer = dynamic_cast<JaegerTracer*>(tracer);
        for (auto& iter : jaegerTracer->_spans)
        {
            if (dynamic_cast<JaegerSpan*>(iter.second)->isSampled())
            {
                jaegerSpans.push_back(jaegerizeSpan(iter.second));
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

    //jaegerSpan.
    return jaegerSpan;
}

::Process OpenTracing::Helper::jaegerizeProcess(const OpenTracing::Process* process)
{
    ::Process jaegerProcess;
    std::vector<::Tag> tags;

    for (auto& iter : process->_tags)
    {
        tags.push_back(jaegerizeTag(iter));
    }

    jaegerProcess.__set_tags(tags);
    jaegerProcess.__set_serviceName(process->_serviceName);
    return jaegerProcess;
}

::Tag OpenTracing::Helper::jaegerizeTag(const OpenTracing::Tag* tag)
{
    ::Tag jaegerTag;
    jaegerTag.__set_key(tag->_key);
    jaegerTag.__set_vType(TagType::type::STRING);
    jaegerTag.__set_vStr(tag->_value);

    /*
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




