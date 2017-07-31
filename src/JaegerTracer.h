#ifndef JAEGERTRACER_H
#define JAEGERTRACER_H

#include <phpcpp.h>
#include <iostream>
#include "ITracer.h"
#include "JaegerSpan.h"
#include "IReporter.h"

class JaegerTracer : public ITracer
{
private:
    IReporter* _reporter;
    //IReader*
public:
    ~JaegerTracer()
    {
        delete _reporter;
        Php::out<<"JaegerTracer::~JaegerTracer" << std::endl;
    }
    
    JaegerTracer(IReporter* reporter):_reporter{reporter}
    {
        Php::out<< "JaegerTracer constructor" << std::endl;
    };
    
    void init(std::string serviceName) const
    {
        this->print();
    }
    //JaegerTracer();
    //JaegerTracer(const ITracer& in)
    //{
    //    Php::out<< "ITracer copy constructor" << std::endl;
    //}
    ISpan* startSpan(const std::string& operationName) const
    {
        Php::out << "JaegerTracer::startSpan, operationName = " << operationName << std::endl;
        return new JaegerSpan();
    }
    
    void print() const
    {
        Php::out << "JaegerTracer::print" << std::endl;
    };
    
    const char* __toString() const
    {
        return "JaegerTracer::__toString";
    }

    const char* _name() const
    {
        return "JaegerTracer";
    }    
};


#endif /* JAEGERTRACER_H */

