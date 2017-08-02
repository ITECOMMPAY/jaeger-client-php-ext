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
    SpanContext* _context;
    int64_t _endTime;

    static const int SAMPLED_FLAG = 0x01;
    JaegerSpan(SpanContext* context, const std::string& operationName, const Php::Value& startTime = nullptr) :
        _operationName { operationName },
        _context{ context }
    {
        Php::out << "JaegerSpan::JaegerSpan addr: " << this << std::endl;
        _startTime = !startTime.isNull() ? static_cast<int64_t>(startTime) : Helper::now();
    }
    JaegerSpan(const JaegerSpan&) = delete;
    ~JaegerSpan()
    {
        Php::out << "~JaegerSpan addr: " << this << std::endl;
        delete _context;
        for (auto iter : _tags)
            delete iter;
        _tags.clear();
        for (auto iter : _logs)
            delete iter;
        _logs.clear();
    }
    void addTags(Php::Parameters &params);
    //void addTags(const std::string &key) {};
    void addLogs(Php::Parameters& logs);
    /*Whether the span is sampled*/
    bool isSampled() const
    {
        return _context->_flags & SAMPLED_FLAG;
    }
    const char* _name() const;
};

#endif /* JAEGERSPAN_H */
