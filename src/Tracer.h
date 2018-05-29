#ifndef TRACER_H
#define TRACER_H

#include <phpcpp.h>
#include "ITracer.h"
#include "IReporter.h"
#include "ISampler.h"
#include "Printer.h"

namespace OpenTracing
{
    class Tracer : public Php::Base
    {
    public:
        Tracer() {};
        virtual ~Tracer();

        static Printer file_logger;
        static ITracer* global_tracer;
        static int header_flag;
        static bool udp_transport;

        /*Create tracer instance and call its init method*/
        static void init(Php::Parameters& params);
        /*Pass startSpan call to the tracer*/
        static Php::Value startSpan(Php::Parameters& params);
        /*Pass getCurrentSpan call to the tracer*/
        static Php::Value getCurrentSpan();
        /*Pass getCurrentTraceId call to the tracer*/
        static Php::Value getCurrentTraceId();
        /*Pass getCurrentSpanId call to the tracer*/
        static Php::Value getCurrentSpanId(Php::Parameters& params);
        /*Pass getCurrentParentId call to the tracer*/
        static Php::Value getCurrentParentId(Php::Parameters& params);
        /*Pass finishSpan call to the tracer*/
        static void finishSpan(Php::Parameters& params);
        /*Pass inject call to the tracer*/
        static Php::Value inject(Php::Parameters& params);
        /*Pass extract call to the tracer*/
        static Php::Value extract(Php::Parameters& params);
        /*Pass flush call to the tracer*/
        static void flush();
        /*Add tags to current span*/
        static void addTags(Php::Parameters& params);
        /*Add logs to current span*/
        static void addLogs(Php::Parameters& params);

        /*Build reporter*/
        static IReporter* buildReporter(const Php::Value& settings);
        /*Build sampler*/
        static ISampler* buildSampler(const Php::Value& settings);

        /*Debug Output*/
        /* usage e.g. \Tracer::print('---'.print_r($_SERVER['SCRIPT_NAME'],true));*/
        static void print(Php::Parameters& params);

        /*Get global tracer*/
        static Php::Value getTracer();
    };
}
#endif /* TRACER_H */

