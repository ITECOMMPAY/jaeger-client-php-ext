#ifndef JAEGERSPAN_H
#define JAEGERSPAN_H

#include "ISpan.h"
#include "Tag.h"
#include "Log.h"
#include "SpanContext.h"

namespace OpenTracing
{
    class JaegerSpan : public ISpan
    {
    public:
        std::string _operationName;
        int64_t _startTime;
        int64_t _endTime;
        std::vector<Tag*> _tags;
        std::vector<Log*> _logs;
        static const int SAMPLED_FLAG = 0x01;
        SpanContext* _context;

        JaegerSpan(SpanContext* context, const std::string& operationName, const Php::Value& startTime = nullptr);
        JaegerSpan(const JaegerSpan&) = delete;
        ~JaegerSpan();
        void addTags(Php::Parameters& params);
        void addLogs(Php::Parameters& logs);
        /*Whether the span is sampled*/
        bool isSampled() const;
        const char* _name() const;
    };
}

#endif /* JAEGERSPAN_H */
