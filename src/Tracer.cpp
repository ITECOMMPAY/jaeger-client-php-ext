#include "Tracer.h"
#include "UdpReporter.h"
#include "FileReporter.h"
#include "PercentageSampler.h"

ITracer* global_tracer;

void GlobalInit()
{
#ifdef TRACER_DEBUG
    Php::out << "main::GlobalInit" << std::endl;
#endif
    global_tracer = new NoopTracer();
}

Tracer::~Tracer()
{
    Php::out << "Tracer::~Tracer" << std::endl;
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

void Tracer::init(Php::Parameters &params)
{
    /*
    private static $defaults = [
        'enabled' => false,
        'reporter' => [
            'type' => 'udp',
            'options' => [
                'addr' => 'localhost',
                'port' => 6831,
            ],
        ],
        'sampler' => [
            'type' => 'percentage',
            'options' => [
                'percents' => 1
            ]
        ]
    ];*/

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
        //Php::out << std::boolalpha << sampler->isSampled() << std::endl;
    }

    global_tracer->init(serviceName);

#ifdef TRACER_DEBUG

    //Php::out<< "Tracer::init" << std::endl;
    //if (!params.empty())
    //{
        /*if (nullptr == global_tracer)
        {
            delete global_tracer;
        }*/
        //bool type{params[0]};
        //if(type)
        //{
            //global_tracer = new JaegerTracer();
            //return;
        //}
    //}
    //global_tracer = new NoopTracer();
#endif   
}

Php::Value Tracer::getTracer()
{

    /*
    if (self::$tracer === null) {
        self::$tracer = new OpenTracing\TracerNoop();
    }

    return self::$tracer;
     */

#ifdef TRACER_DEBUG
    Php::out << "Tracer::getTracer addr: " << global_tracer << std::endl;
    //global_tracer->print();
#endif

    return Php::Object(global_tracer->_name(), global_tracer);
}

Php::Value Tracer::startSpan(Php::Parameters &params)
{
    /*try {
        return self::getTracer()->startSpan($operationName, $options);
    } catch (\Throwable $e) {
        return new OpenTracing\SpanNoop();
    }*/

    Php::out << "Tracer::startSpan" << std::endl;

    std::string operationName = params[0];
    Php::Value options;

    if (params.size() == 1)
    {
        options = "";//[]
//#ifdef TRACER_DEBUG
        Php::out << "1 parameter" << std::endl;
        //#endif
    }
    else
    {
        options = params[1];
        Php::out << "2 parameters" << std::endl;
    }

    ISpan* span = global_tracer->startSpan(operationName);


    return Php::Object(span->_name(), span);
}