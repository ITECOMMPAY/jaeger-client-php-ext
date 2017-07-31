#ifndef TRACER_H
#define TRACER_H
#include <phpcpp.h>
#include "ITracer.h"
#include "NoopTracer.h"
#include "JaegerTracer.h"

#include "IReporter.h"

#ifndef TRACER_DEBUG
#  define TRACER_DEBUG
#endif

extern ITracer* global_tracer;
void GlobalInit();

class Tracer : public Php::Base
{
public:
    Tracer(){};
    virtual ~Tracer();
    
    static Php::Value getTracer();
    
    /*Create tracer instance and call its init method*/
    static void init(Php::Parameters &params);
    /*Pass startSpan call to the tracer*/
    static Php::Value startSpan(Php::Parameters &params);
    
    static IReporter* buildReporter(const Php::Value& settings);
    
    static Php::Value return_array(Php::Parameters &params);
};

/*class _tracer
{
public:
    _tracer()=delete;
    _tracer(const _tracer&)=delete;
    _tracer(const _tracer&&)=delete;
    void operator = (const _tracer&)=delete;    
    void operator = (const _tracer&&)=delete;

    ITracer* getTracer() const
    {
        return global_tracer;
    }
};*/

#endif /* TRACER_H */

