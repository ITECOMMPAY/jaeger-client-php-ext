#ifndef TRACER_H
#define TRACER_H

#include <vector>
#include <phpcpp.h>
#include "ITracer.h"
#include "IReporter.h"
#include "ISampler.h"
#include "Printer.h"

namespace OpenTracing
{
    /*static class to be exported*/
    class Tracer : public Php::Base
    {
    public: /*data*/
        static Printer file_logger;
        static ITracer* global_tracer;
        static int header_flag;
        static bool udp_transport;
        static bool ini_settings_loaded;
        static bool single_ext_call;
        static std::vector<std::string> empty_span_hosts;
        static std::vector<std::string> not_instrumented_hosts;
        static std::map<std::string, std::string> userTracerSettings;
    public: /*methods*/
        Tracer() {};
        virtual ~Tracer();

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
        /*Add new span for external requests registration*/
        static Php::Value startExternalTracing(Php::Parameters& params);

        /*Debug Output*/
        /* usage e.g. \Tracer::print('---'.print_r($_SERVER['SCRIPT_NAME'],true));*/
        static void print(Php::Parameters& params);

        /*Get global tracer*/
        static Php::Value getTracer();
    };

    /*Helpers*/
    /*Removes scheme from URI*/
    void removeSchemeFromUri(std::string& uri);
    /*Removes params from URI*/
    void removeParamsQueryFromUri(std::string& uri);
    /*Filteres hosts*/
    bool hostsFilterPassed(const std::string& uri, const std::vector<std::string>& filtered);
    /*Get list of hosts from semicolon separated string */
    std::vector<std::string> parseHostsStr(std::string hosts);
    /*Build reporter*/
    IReporter* buildReporter(const Php::Value& settings);
    /*Build sampler*/
    ISampler* buildSampler(const Php::Value& settings);
    /*Create name for trarelic span based on its http.uri tag*/
    std::string getTrarelicSpanName(std::string uri, unsigned fetchCount);
    /*Create array with Tracer paramaters for curl requests */
    Php::Value createCurlParamsList();
    /*Load settings from php.ini */
    void loadIniSettings();
    /*Create array with default Tracer paramaters */
    Php::Value createDefaultParamsList();
    /*Create array with tag paramaters for curl external calls */
    Php::Value createCurlTagParamsList(const std::string& uri, const std::string& caller, const std::string& type);
    /*Init tracer implementation */
    void initInternal(const std::string& serviceName, const Php::Value& paramsList);
    /*Start span implementation */
    Php::Value startSpanInternal(const std::string& operationName, const Php::Value& options = nullptr);
}

#endif /* TRACER_H */
