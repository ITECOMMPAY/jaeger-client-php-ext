#ifndef ITRACER_H
#define ITRACER_H

#include <phpcpp.h>
#include <iostream>
#include "ITracer.h"

/*forward declaration*/
class ISpan;

class ITracer : public Php::Base
{
public:
    virtual ~ITracer();

    /*Init tracing*/
    virtual void init(const std::string& serviceName) = 0;
    /*Start span*/
    virtual ISpan* startSpan(const std::string& operationName, const Php::Value& options = nullptr) const = 0;
    /*Get current OPENED span*/
    virtual ISpan* getCurrentSpan() const = 0;
    /*Finish span*/
    virtual void finishSpan() const = 0;//($span, $endTime = null);
    /*Inject context into carrier*/
    virtual void inject() const = 0;//($context, $format, &$carrier);
    /*Extract context from the carrier*/
    virtual void extract() const = 0;//($format, $carrier);
    /*Flush everything via reporter*/
    virtual void flush() const = 0;
    /*Name of a class to pass in Php::Object*/
    virtual const char* _name() const = 0;
};

#endif /* ITRACER_H */

