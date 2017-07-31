#ifndef JAEGERTRACER_H
#define JAEGERTRACER_H

#include <phpcpp.h>
#include <iostream>
#include "ITracer.h"
#include "JaegerSpan.h"
#include "IReporter.h"
#include "ISampler.h"

class JaegerTracer : public ITracer
{
private:
    IReporter* _reporter;
    ISampler* _sampler;
public:
    ~JaegerTracer()
    {
        delete _reporter;
        delete _sampler;
        Php::out << "JaegerTracer::~JaegerTracer" << std::endl;
    }

    JaegerTracer(IReporter* reporter, ISampler* sampler) :
        _reporter{ reporter },
        _sampler{ sampler }
    {
        Php::out << "JaegerTracer::JaegerTracer" << std::endl;
    };

    void init(const std::string& serviceName) const;
    ISpan* startSpan(const std::string& operationName, const Php::Value& options = nullptr) const;
    ISpan* getCurrentSpan() const;
    void finishSpan() const;
    void inject() const;
    void extract() const;
    void flush() const;

    const char* _name() const;
};


#endif /* JAEGERTRACER_H */

