#ifndef HELPER_H
#define HELPER_H
#include <unistd.h>
#include <limits.h>
#include "ISpan.h"

class Helper
{
public:
    /*Get current unix timestamp in microseconds*/
    static const int64_t now();
    /*Get Host name*/
    static const std::string getHostName();
    /*Get random id*/
    //public static function generateId()
    //{
    //    return rand() << 32 | rand();
    //}
    /*Get current ip*/
    static const std::string getCurrentIp();
    /*Get jaeger version of the Tracer*/
    //public static function jaegerizeTracer(JaegerTracer $tracer): Autogen\Batch
    /*Get jaeger version of the Process*/
    //public static function jaegerizeProcess(Process $process): Autogen\Process
    /*Get jaeger version of the Tag*/
    //public static function jaegerizeTag(Tag $tag) : Autogen\Tag
    /*Get jaeger version of the Span*/
    //public static function jaegerizeSpan(Span $span) : Autogen\Span
    /*Get jaeger version of the Log*/
    //public static function jaegerizeLog(Log $log) : Autogen\Log
};

#endif /* HELPER_H */
