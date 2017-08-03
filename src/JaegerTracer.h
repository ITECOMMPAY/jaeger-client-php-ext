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

class JaegerTracer : public ITracer
{
private:
    IReporter* _reporter;
    ISampler* _sampler;
    Process* _process;
    bool _isSampled;
public:
    std::unordered_map<int, ISpan*> _spans;
    std::vector<int> _activeSpans;
    ~JaegerTracer();
    JaegerTracer(IReporter* reporter, ISampler* sampler);

    void init(const std::string& serviceName);
    ISpan* startSpan(const std::string& operationName, const Php::Value& options = nullptr);
    ISpan* getCurrentSpan();
    void finishSpan(ISpan* span, const Php::Value& endTime = nullptr);
    void inject() const;
    void extract() const;
    void flush() const;

    const char* _name() const;
};


#endif /* JAEGERTRACER_H */

