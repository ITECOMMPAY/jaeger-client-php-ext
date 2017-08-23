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
        static ::Batch* jaegerizeTracer(const OpenTracing::ITracer* tracer, const OpenTracing::ISpan* span = nullptr, LogCount logLimit = LogCount::WHOLE);
        /*Get jaeger version of the Span*/
        static ::Span jaegerizeSpan(const OpenTracing::ISpan* span, LogCount logLimit = LogCount::WHOLE);
        /*Get jaeger version of the Process*/
        static ::Process jaegerizeProcess(const OpenTracing::Process* process);
        /*Get jaeger version of the Tag*/
        static ::Tag jaegerizeTag(const OpenTracing::Tag* tag);
        /*Get jaeger version of the Log*/
        static ::Log jaegerizeLog(const OpenTracing::Log* log);
    };
}
#endif /* HELPER_H */
