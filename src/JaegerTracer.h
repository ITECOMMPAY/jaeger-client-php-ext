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
    ~JaegerTracer()
    {
        delete _reporter;
        delete _sampler;
        delete _process;
        for (auto& iter : _spans)
            delete iter.second;
        _spans.clear();
        _activeSpans.clear();

        Php::out << "~JaegerTracer" << std::endl;
    }

    JaegerTracer(IReporter* reporter, ISampler* sampler) :
        _reporter{ reporter },
        _sampler{ sampler },
        _isSampled{ false }
    {
        Php::out << "JaegerTracer::JaegerTracer" << std::endl;
    };

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

