#ifndef ITRACER_H
#define ITRACER_H

#include <phpcpp.h>
#include "ISpan.h"
#include "SpanContext.h"

namespace OpenTracing
{
    class ITracer : public Php::Base
    {
    public:
        virtual ~ITracer() = 0;
        
        /*Init tracing*/
        virtual void init(const std::string& serviceName) = 0;
        /*Start span*/
        virtual OpenTracing::ISpan* startSpan(const std::string& operationName, const Php::Value& options = nullptr) = 0;
        /*Get current opened span*/
        virtual OpenTracing::ISpan* getCurrentSpan() = 0;
        /*Get current trace id*/
        virtual int64_t getCurrentTraceId() = 0;
        /*Finish span*/
        virtual void finishSpan(ISpan* span, const Php::Value& endTime = nullptr) = 0;
        /*Inject context into carrier*/
        virtual void inject(const Php::Value& context, Php::Value& carrier) = 0;
        /*Extract context from the carrier*/
        virtual OpenTracing::SpanContext* extract(const Php::Value& carrier) const = 0;
        /*Flush everything via reporter*/
        virtual void flush() = 0;
        /*Name of a class to pass in Php::Object*/
        virtual const char* _name() const = 0;
    };
}

#endif /* ITRACER_H */

