#ifndef NOOPTRACER_H
#define NOOPTRACER_H

#include <phpcpp.h>
#include "ITracer.h"

namespace OpenTracing
{
    class NoopTracer : public ITracer
    {
    public:
        ~NoopTracer();
        NoopTracer();
        void init(const std::string& serviceName);
        ISpan* startSpan(const std::string& operationName, const Php::Value& options = nullptr);
        ISpan* getCurrentSpan();
        const std::string getCurrentTraceId();
        int64_t getCurrentSpanId(ISpan* span);
        int64_t getCurrentParentId(ISpan* span);
        void finishSpan(ISpan* span, const Php::Value& endTime = nullptr);
        void inject(const Php::Value& context, Php::Value& carrier);
        SpanContext* extract(const Php::Value& carrier) const;
        void flush();

        const char* _name() const;
    };
}

#endif /* NOOPTRACER_H */
