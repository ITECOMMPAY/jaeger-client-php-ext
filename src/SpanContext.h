#ifndef SPANCONTEXT_H
#define SPANCONTEXT_H
#include <phpcpp.h>
#include <iostream>

class SpanContext
{
private:
    int _traceId;
    int _parentId;
    int _flags;
    Php::Value _refType;
public:
    int _spanId;
    SpanContext(const int& traceId, const int& spanId, const int& parentId, const int& flags, const Php::Value& refType = nullptr) :
        _traceId{ traceId },
        _parentId{ parentId },
        _flags{ flags },
        _refType{ refType },
        _spanId{ spanId }
    {
        Php::out << _traceId << " " << _spanId << " " << _parentId << " " << _flags << " " << std::endl;
    }
    SpanContext(const SpanContext&) = delete;

    ~SpanContext()
    {
    }
};


#endif /* SPANCONTEXT_H */