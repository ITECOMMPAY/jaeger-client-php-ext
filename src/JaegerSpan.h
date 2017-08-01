#ifndef JAEGERSPAN_H
#define JAEGERSPAN_H
#include "ISpan.h"
#include "Tag.h"
#include "Log.h"
#include "SpanContext.h"

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
    JaegerSpan() :
        _context{ nullptr }
    {
        Php::out << "JaegerSpan::JaegerSpan addr: " << this << std::endl;

    }
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
    void addTags(Php::Parameters &params);;
    //void addTags(const std::string &key) {};
    void addLogs(Php::Parameters& logs);
    const char* _name() const;
};

#endif /* JAEGERSPAN_H */
