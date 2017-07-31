#ifndef ITRACER_H
#define ITRACER_H

#include <phpcpp.h>
#include <iostream>
#include "ITracer.h"

/*forward declaration*/
class ISpan;

class ITracer : public Php::Base
{
//protected:
//    ITracer(const ITracer& in)
//    {
//        Php::out<< "ITracer copy constructor" << std::endl;
//    }
public:
    /*Destroys the ITracer*/
    virtual ~ITracer();
    //ITracer()
    //{
    //    Php::out<< "ITracer constructor" << std::endl;
    //};
//    ITracer(const ITracer& in)
//    {
//        Php::out<< "ITracer copy constructor" << std::endl;
//    };
    
    /*Init tracing*/
    virtual void init(std::string serviceName) const = 0;

    //public function startSpan($operationName, array $options = []): SpanInterface;
    virtual ISpan* startSpan(const std::string& operationName) const = 0;
    //virtual ISpan* startSpan(const std::string& operationName, const StartSpanOptions& startSpanOptions = StartSpanOptions()) const = 0;
    
    /*Extract context from the carrier*/
    //virtual void extract() const = 0;  
    
    virtual void print() const = 0;
    
    
    
    /* Name of a class to pass in Php::Object*/
    virtual const char* _name() const = 0;
};


#endif /* ITRACER_H */

