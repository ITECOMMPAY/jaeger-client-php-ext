#include <iostream>
#include "Tracer.h"
#include "UdpReporter.h"
#include "FileReporter.h"
#include "PercentageSampler.h"
#include "NoopTracer.h"
#include "JaegerTracer.h"
//#include "Printer.h"
using namespace OpenTracing;

#include "JaegerSpan.h"
//Php::Value ref_global_tracer; //increase ref count so PHP GC will not delete it



Tracer::~Tracer()
{
#ifdef TRACER_DEBUG
    Php::out << "~Tracer" << std::endl;
#endif
    Tracer::file_logger.PrintLine("~Tracer");
}

IReporter* Tracer::buildReporter(const Php::Value& settings)
{
    if (Php::array_key_exists("type", settings) && Php::array_key_exists("options", settings) && is_array(settings["options"]))
    {
        if (settings["type"] == "file")
        {
            return new FileReporter(settings["options"]);
        }
        else if (settings["type"] == "udp")
        {
            return new UdpReporter(settings["options"]);
        }
    }

    Php::Value defaults;
    defaults["reporter"]["type"] = "udp";
    defaults["reporter"]["options"]["addr"] = "localhost";
    defaults["reporter"]["options"]["port"] = 6831;

    return buildReporter(defaults["reporter"]);
}

ISampler* Tracer::buildSampler(const Php::Value& settings)
{
    if (Php::array_key_exists("type", settings) && Php::array_key_exists("options", settings) && is_array(settings["options"]))
    {
        if (settings["type"] == "percentage")
        {
            return new PercentageSampler(settings["options"]);
        }
    }

    Php::Value defaults;
    defaults["sampler"]["type"] = "percentage";
    defaults["sampler"]["options"]["percents"] = 1;

    return buildSampler(defaults["sampler"]);
}

void Tracer::init(Php::Parameters& params)
{
    //config defaults
    //private static $defaults = [
    //    'enabled' => false,
    //    'reporter' => [
    //        'type' => 'udp',
    //        'options' => [
    //            'addr' => 'localhost',
    //            'port' => 6831,
    //        ],
    //    ],
    //    'sampler' => [
    //        'type' => 'percentage',
    //        'options' => [
    //            'percents' => 1
    //        ]
    //    ]
    //];

#ifdef TRACER_DEBUG
#endif

    Php::Value defaults;
    defaults["enabled"] = false;
    defaults["reporter"]["type"] = "udp";
    defaults["reporter"]["options"]["addr"] = "localhost";
    defaults["reporter"]["options"]["port"] = 6831;
    defaults["sampler"]["type"] = "percentage";
    defaults["sampler"]["options"]["percents"] = 1;

    std::string serviceName = params[0];
    Php::Value settings;

    if (params.size() == 1)
    {
        settings = defaults;
    }
    else
    {
        settings = params[1];
        settings = Php::call("array_merge", defaults, settings);
    }

    {
        Tracer::file_logger.PrintLine("Tracer::init " + serviceName);
        Tracer::file_logger.PrintLine("\t delete global_tracer in Tracer::init");
    }

    delete global_tracer;

    if (!settings["enabled"])
    {
        global_tracer = new NoopTracer();
    }
    else
    {
        IReporter* reporter = buildReporter(settings["reporter"]);
        ISampler* sampler = buildSampler(settings["sampler"]);
        global_tracer = new JaegerTracer(reporter, sampler);
    }

    global_tracer->init(serviceName);
}

//Php::Value Tracer::getTracer()
//{
//#ifdef TRACER_DEBUG
//    Php::out << "Tracer::getTracer addr: " << global_tracer << std::endl;
//    //Php::out << "Tracer::getTracer addr: " << global_tracer << " ref addr: " << &ref_global_tracer << std::endl;
//#endif
//    //ref_global_tracer = Php::Object(global_tracer->_name(), global_tracer);
//    return global_tracer == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(global_tracer->_name(), global_tracer);
//}

Php::Value Tracer::startSpan(Php::Parameters& params)
{
#ifdef TRACER_DEBUG
    Php::out << "Tracer::startSpan" << std::endl;
#endif    

    std::string operationName = params[0];
    Php::Value options = nullptr;

    if (params.size() == 1)
    {
#ifdef TRACER_DEBUG
        Php::out << "    1 parameter" << std::endl;
#endif
    }
    else
    {
        options = params[1];
#ifdef TRACER_DEBUG
        Php::out << "    2 parameters" << std::endl;
#endif
    }

    ISpan* span = nullptr;

    if (global_tracer != nullptr)
    {
        span = global_tracer->startSpan(operationName, options);

#ifdef TRACER_DEBUG
        Php::out << "    total spans: " << dynamic_cast<JaegerTracer*>(global_tracer)->_spans.size() << std::endl;
#endif

        std::ostringstream ss;
        ss << "--- tracer " << global_tracer << "\n\t\t\t\t\t\t\t\t\t\tstartSpan returned: " << span <<
            " _spans.size() " << dynamic_cast<JaegerTracer*>(global_tracer)->_spans.size();

        //file_logger.PrintLine(ss.str());
        //ss.str("");
        //ss.clear();

        for (auto& iter : dynamic_cast<JaegerTracer*>(global_tracer)->_spans)
        {
            JaegerSpan* temp = dynamic_cast<JaegerSpan*>(iter.second);
            ss <<
                "\n\t\t\t\t\t\t\t\t\t\t\t\tspan id: " << iter.first <<
                "\n\t\t\t\t\t\t\t\t\t\t\t\t\t_startTime: " << temp->_startTime <<
                "\n\t\t\t\t\t\t\t\t\t\t\t\t\t_endTime  : " << temp->_endTime;
            if (temp->_context != nullptr)
            {
                ss <<
                    "\n\t\t\t\t\t\t\t\t\t\t\t\t\t_context: " <<
                    "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t_traceId: " << temp->_context->_traceId <<
                    "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t_spanId: " << temp->_context->_spanId <<
                    "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t_parentId: " << temp->_context->_parentId;
            }
        }
        file_logger.PrintLine(ss.str());
        ss.str("");
        ss.clear();
    }

    return span == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(span->_name(), span);
}

Php::Value Tracer::getCurrentSpan()
{
    ISpan* span = nullptr;
    if (global_tracer != nullptr)
    {
        global_tracer->getCurrentSpan();
    }
    //todo - return a copy of object... fucking zend
    return span == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(span->_name(), span);
}

void Tracer::finishSpan(Php::Parameters& params)
{
    Php::Value param = params[0];
    if (!param.isNull())
    {
        if (!param.instanceOf("ISpan"))
            throw Php::Exception("Wrong parameter passed");
        ISpan* span = (ISpan*)param.implementation();
        if (global_tracer != nullptr)
        {
            global_tracer->finishSpan(span);
        }
    }
}

void Tracer::inject(Php::Parameters& params)
{
    Php::Value context = params[0];
    std::string format = params[1];
    std::string carrier = params[2];

    if (!context.isNull())
    {
        if (global_tracer != nullptr)
        {
            global_tracer->inject(context, format, carrier);
            params[2] = carrier;
        }
    }
}

Php::Value Tracer::extract(Php::Parameters& params)
{
    const std::string& format = params[0];
    const std::string& carrier = params[1];

    SpanContext* context = nullptr;
    if (global_tracer != nullptr)
    {
        context = global_tracer->extract(format, carrier);
    }

    return context == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(context->_name(), context);
}

void Tracer::flush()
{
    if (global_tracer != nullptr)
    {
        global_tracer->flush();
    }
}

void Tracer::addTags(Php::Parameters& params)
{
    if (global_tracer != nullptr)
    {
        ISpan* span = global_tracer->getCurrentSpan();
        if (span != nullptr)
        {
            span->addTags(params);
        }
    }
}

void Tracer::addLogs(Php::Parameters& params)
{
#ifdef TRACER_DEBUG
    Php::out << "Tracer::addLogs" << std::endl;
#endif
    // = nullptr;
    if (global_tracer != nullptr)
    {
        if (strcmp(global_tracer->_name(), "JaegerTracer") == 0)
        {
            ISpan* span = global_tracer->getCurrentSpan();
            if (span != nullptr)
            {
                span->addLogs(params);
            }

        }
        //std::ostringstream ss;
        //ISpan* span1 = nullptr;
        //ss << "--- tracer::addLogs " << span1 << global_tracer;// <<
        ////    "\n\t\t\t\t\t\t\t\t\t\getCurrentSpan returned: " << span <<
        ////    " _spans.size() " << dynamic_cast<JaegerTracer*>(global_tracer)->_spans.size();
        //file_logger.PrintLine(ss.str());
        //ss.str("");
        //ss.clear();
    }
}
