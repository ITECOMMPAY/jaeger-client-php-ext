#ifndef HELPER_H
#define HELPER_H

#include "ISpan.h"
#include "ITracer.h"
#include "Process.h"
#include "Log.h"
#include "Tag.h"

#include "thrift-gen/jaeger_types.h"

namespace OpenTracing
{
    enum class JaegerizeVersion {
        ERROR = -1,
        V1 = 1,
        V2 = 2
    };

    class Helper
    {
    public:
        /*Get current unix timestamp in microseconds*/
        static const int64_t now();
        /*Get random id*/
        static const int64_t generateId();
        /*Get random percentage in [0;100]*/
        static const int genPercentage();
        /*Get current ip*/
        static const std::string getCurrentIp();
        /*Get Host name*/
        static const std::string getHostName();
        /*Get jaeger version of the Tracer*/
        static jaegertracing::thrift::Batch* jaegerizeTracer(
            const OpenTracing::ITracer* tracer,
            const OpenTracing::ISpan* span = nullptr,
            LogCount logLimit = LogCount::WHOLE,
            JaegerizeVersion ver = JaegerizeVersion::V1,
            size_t indStart = 0,
            size_t indCount = 0,
            size_t part = 0,
            bool badLog = false
        );
        /*Get jaeger version of the Span*/
        static jaegertracing::thrift::Span jaegerizeSpan(
            const OpenTracing::ISpan* span,
            LogCount logLimit = LogCount::WHOLE,
            JaegerizeVersion ver = JaegerizeVersion::V1,
            size_t indStart = 0,
            size_t indCount = 0,
            size_t part = 0,
            bool badLog = false
        );
        /*Get jaeger version of the Process*/
        static jaegertracing::thrift::Process jaegerizeProcess(const OpenTracing::Process* process);
        /*Get jaeger version of the Tag*/
        static jaegertracing::thrift::Tag jaegerizeTag(const OpenTracing::Tag* tag);
        /*Get jaeger version of the Log*/
        static jaegertracing::thrift::Log jaegerizeLog(const OpenTracing::Log* log);
    };
}
#endif /* HELPER_H */
