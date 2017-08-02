#ifndef NOOPTRACER_H
#define NOOPTRACER_H

#include <phpcpp.h>
#include <iostream>
#include "ITracer.h"
#include "NoopSpan.h"

class NoopTracer : public ITracer
{
public:
    ~NoopTracer();
    NoopTracer();
    void init(const std::string& serviceName);
    ISpan* startSpan(const std::string& operationName, const Php::Value& options = nullptr);
    ISpan* getCurrentSpan();
    void finishSpan(ISpan* span, const Php::Value& endTime = nullptr);
    void inject() const;
    void extract() const;
    void flush() const;

    const char* _name() const;
};


#endif /* NOOPTRACER_H */

