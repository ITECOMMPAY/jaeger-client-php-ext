#ifndef JAEGERSPAN_H
#define JAEGERSPAN_H
#include "ISpan.h"
#include "Tag.h"
#include "Log.h"
#include "SpanContext.h"
#include "Helper.h"

class JaegerSpan : public ISpan
{
private:
    std::string _operationName;
    int64_t _startTime;
    std::vector<Tag*> _tags;
    std::vector<Log*> _logs;
public:
    static const int SAMPLED_FLAG = 0x01;
    SpanContext* _context;
    int64_t _endTime;

    JaegerSpan(SpanContext* context, const std::string& operationName, const Php::Value& startTime = nullptr);
    JaegerSpan(const JaegerSpan&) = delete;
    ~JaegerSpan();
    void addTags(Php::Parameters& params);
    void addLogs(Php::Parameters& logs);
    /*Whether the span is sampled*/
    bool isSampled() const
    {
        return _context->_flags & SAMPLED_FLAG;
    }
    const char* _name() const;
};

#endif /* JAEGERSPAN_H */
