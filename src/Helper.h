#ifndef HELPER_H
#define HELPER_H
#include <random>
#include <unistd.h>
#include <limits.h>
#include "ISpan.h"
#include "ITracer.h"
#include "Process.h"
#include "Log.h"
#include "thrift-gen/jaeger_types.h"

extern std::default_random_engine re;
extern std::uniform_int_distribution<int> dist;

namespace OpenTracing
{
    class Helper
    {
    public:
        /*Get current unix timestamp in microseconds*/
        static const int64_t now();
        /*Get random id*/
        static const uint64_t generateId();
        /*Get current ip*/
        static const std::string getCurrentIp();
        /*Get Host name*/
        static const std::string getHostName();
        /*Get jaeger version of the Tracer*/
        static const ::Batch* jaegerizeTracer(const OpenTracing::ITracer* tracer);
        /*Get jaeger version of the Span*/
        static ::Span jaegerizeSpan(const OpenTracing::ISpan* span);
        /*Get jaeger version of the Process*/
        static ::Process jaegerizeProcess(const OpenTracing::Process* process);
        /*Get jaeger version of the Tag*/
        static ::Tag jaegerizeTag(const OpenTracing::Tag* tag);
        /*Get jaeger version of the Log*/
        static ::Log jaegerizeLog(const OpenTracing::Log* log);
    };
}
#endif /* HELPER_H */
