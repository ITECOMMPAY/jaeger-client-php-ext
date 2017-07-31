#include "Tracer.h"
#include "UdpReporter.h"
#include "FileReporter.h"

//namespace
//{
    ITracer* global_tracer;  
    
//}

void GlobalInit()
{
    Php::out<< "main::GlobalInit" << std::endl;
    global_tracer = new NoopTracer();
    
    

}

Tracer::~Tracer()
{
    Php::out<<"Tracer::~Tracer" << std::endl;
}

Php::Value Tracer::return_array(Php::Parameters &params)
{
    /*private static $defaults = [
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
    
    /*todo - potential loss of performance*/
    Php::Value arr = Php::call("array_merge", defaults, params[0]);
    
    return arr;
}

IReporter* Tracer::buildReporter(const Php::Value& settings)
{
    if(Php::array_key_exists("type", settings) && Php::array_key_exists("options", settings) && is_array(settings))
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

void Tracer::init(Php::Parameters &params)
{    
    /*try {
        self::$tracer = TracerFactory::build($settings);
        self::$tracer->init($serviceName);
    } catch (\Throwable $e) {
        self::$tracer = new OpenTracing\TracerNoop();
    }
    
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

   
    //Php::out<< "___defaults" << std::endl;
    //Php::call("var_dump",defaults);
    //if(params.size() == 2)
    //{
    //    Php::out<< "___settings" << std::endl;
    //    Php::Value arr = Php::call("array_merge", defaults, params[1]);
    //    Php::call("var_dump",arr);        
    //}
    
    //std::map<std::string,std::string> map = defaults;
        
    Php::Value defaults;
    defaults["enabled"] = false;
    defaults["reporter"]["type"] = "udp";
    defaults["reporter"]["options"]["addr"] = "localhost";
    defaults["reporter"]["options"]["port"] = 6831;
    defaults["sampler"]["type"] = "percentage";
    defaults["sampler"]["options"]["percents"] = 1;
    
    std::string serviceName = params[0];
    Php::Value settings;

    if (params.size()==1)
    {
        settings = defaults;
        //Php::call("var_dump",settings); 
        //Php::out<<"1 parameter in init" << std::endl;
    }
    else
    {
        settings = params[1];
        //Php::out<<"2 parameters in init" << std::endl;
        /*todo - potential loss of performance*/
        settings = Php::call("array_merge", defaults, settings);
    }

    
    
    delete global_tracer;
    
    if (!settings["enabled"]) 
    {
        //Php::out<< "settings[\"enabled\"] no" << std::endl;
        global_tracer = new NoopTracer();
    }
    else
    {
        IReporter* reporter = buildReporter(settings["reporter"]);
        //delete reporter;
        
        //$reporter = self::buildReporter($settings['reporter']);
        //$sampler = self::buildSampler($settings['sampler']);
        
        //Php::out<< "settings[\"enabled\"] yes" << std::endl; 
        global_tracer = new JaegerTracer(reporter);//JaegerTracer($reporter, $sampler);            
    }
    //std::string temp = settings["reporter"]["options"]["addr"];
    //Php::out<<"temp="<<temp<<std::endl;
    
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
/*     * Build reporter
    private static function buildReporter(array $settings): ReporterInterface
    {
        if (isset($settings['type']) && isset($settings['options']) && is_array($settings['options'])) {
            switch ($settings['type']) {
                case 'file':
                    return new FileReporter($settings['options']);

                case 'udp':
                    return new UdpReporter($settings['options']);
            }
        }

        return self::buildReporter(self::$defaults['reporter']);
    }

    Build sampler
    private static function buildSampler(array $settings): SamplerInterface
    {
        if (isset($settings['type']) && isset($settings['options']) && is_array($settings['options'])) {
            switch ($settings['type']) {
                case 'percentage':
                    return new PercentageSampler($settings['options']);
            }
        }

        return self::buildSampler(self::$defaults['sampler']);
    }*/
    
Php::Value Tracer::getTracer()
{

    /*
    if (self::$tracer === null) {
        self::$tracer = new OpenTracing\TracerNoop();
    }

    return self::$tracer;
     */

    #ifdef TRACER_DEBUG
    Php::out<< "Tracer::getTracer addr: " << global_tracer << std::endl;
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

    Php::out<< "Tracer::startSpan" << std::endl;

    std::string operationName = params[0];
    Php::Value options;

    if (params.size()==1)
    {
        options="";//[]
//#ifdef TRACER_DEBUG
        Php::out<<"1 parameter" << std::endl;
//#endif
    }
    else
    {
        options = params[1];
        Php::out<<"2 parameters" << std::endl;
    }

    ISpan* span = global_tracer->startSpan(operationName);


    return Php::Object(span->_name(),span);
}