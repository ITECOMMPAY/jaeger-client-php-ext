#include "Tracer.h"
#include "UdpReporter.h"
#include "FileReporter.h"
#include "PercentageSampler.h"
using namespace OpenTracing;

ITracer* global_tracer;
//Php::Value ref_global_tracer; //increase ref count so PHP GC will not delete it

void GlobalInit()
{
#ifdef TRACER_DEBUG
    Php::out << "main::GlobalInit" << std::endl;
#endif
    global_tracer = new NoopTracer();
}

Tracer::~Tracer()
{
#ifdef TRACER_DEBUG
    Php::out << "~Tracer" << std::endl;
#endif
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

Php::Value Tracer::getTracer()
{
#ifdef TRACER_DEBUG
    Php::out << "Tracer::getTracer addr: " << global_tracer << std::endl;
    //Php::out << "Tracer::getTracer addr: " << global_tracer << " ref addr: " << &ref_global_tracer << std::endl;
#endif
    //ref_global_tracer = Php::Object(global_tracer->_name(), global_tracer);
    return global_tracer == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(global_tracer->_name(), global_tracer);
}

Php::Value Tracer::startSpan(Php::Parameters& params)
{
    Php::out << "Tracer::startSpan" << std::endl;

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

    ISpan* span = global_tracer->startSpan(operationName, options);

#ifdef TRACER_DEBUG
    JaegerTracer* temp = dynamic_cast<JaegerTracer*>(global_tracer);
    Php::out << "    total spans: " << temp->_spans.size() << std::endl;
#endif
    return span == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(span->_name(), span);
}

Php::Value Tracer::getCurrentSpan()
{
    ISpan* span = global_tracer->getCurrentSpan();

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
        global_tracer->finishSpan(span);
    }
}

void Tracer::inject(Php::Parameters& params)
{
    Php::Value context = params[0];
    std::string format = params[1];
    std::string carrier = params[2];

    if (!context.isNull())
    {
        global_tracer->inject(context, format, carrier);
        params[2] = carrier;
    }
}

Php::Value Tracer::extract(Php::Parameters& params)
{
    const std::string& format = params[0];
    const std::string& carrier = params[1];

    SpanContext* context = global_tracer->extract(format, carrier);

    return context == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(context->_name(), context);
}

void Tracer::flush()
{
    global_tracer->flush();
}

void Tracer::addTags(Php::Parameters& params)
{
    ISpan* span = global_tracer->getCurrentSpan();
    if (span)
    {
        span->addTags(params);
    }
}

void Tracer::addLogs(Php::Parameters& params)
{
    Php::out << "Tracer::addLogs" << std::endl;
    ISpan* span = global_tracer->getCurrentSpan();
    if (span)
    {
        span->addLogs(params);
    }
}
