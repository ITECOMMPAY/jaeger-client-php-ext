#ifndef JAEGERTRACER_H
#define JAEGERTRACER_H

#include <phpcpp.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "ITracer.h"
#include "JaegerSpan.h"
#include "IReporter.h"
#include "ISampler.h"
#include "Process.h"

namespace OpenTracing
{
    class JaegerTracer : public ITracer
    {
    public:
        IReporter* _reporter;
        ISampler* _sampler;
        Process* _process;
        bool _isSampled;
        std::unordered_map<uint64_t, ISpan*> _spans;
        std::vector<uint64_t> _activeSpans;

        ~JaegerTracer();
        JaegerTracer(IReporter* reporter, ISampler* sampler);
        JaegerTracer(const JaegerTracer&) = delete;

        void init(const std::string& serviceName);
        ISpan* startSpan(const std::string& operationName, const Php::Value& options = nullptr);
        ISpan* getCurrentSpan();
        void finishSpan(ISpan* span, const Php::Value& endTime = nullptr);
        void inject(const Php::Value& context, const std::string& format, std::string& carrier);
        SpanContext* extract(const std::string& format, const std::string& carier) const;
        void flush();

        void clearSpans();
        const char* _name() const;
    };
}

#endif /* JAEGERTRACER_H */

