#ifndef HELPER_H
#define HELPER_H
#include <random>
#include <unistd.h>
#include <limits.h>
#include "ISpan.h"

extern std::default_random_engine re;
extern std::uniform_int_distribution<int> dist;

class Helper
{
private:
    
    //static std::default_random_engine re;
    //static std::uniform_int_distribution<unsigned int> dist;
public:
    /*Get current unix timestamp in microseconds*/
    static const int64_t now();
    /*Get random id*/
    static const unsigned long long int generateId();
    /*Get current ip*/
    static const std::string getCurrentIp();
    /*Get Host name*/
    static const std::string getHostName();
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
