#ifndef SPANCONTEXT_H
#define SPANCONTEXT_H
#include <phpcpp.h>
#include <iostream>

class SpanContext
{
public:
    unsigned int _traceId;
    unsigned int _spanId;
    unsigned int _parentId;
    int _flags;
    Php::Value _refType;

    SpanContext(const unsigned int& traceId, const unsigned int& spanId, const unsigned int& parentId, const int& flags, const Php::Value& refType = nullptr) :
        _traceId{ traceId },
        _spanId{ spanId },
        _parentId{ parentId },
        _flags{ flags },
        _refType{ refType }
    {
        Php::out << "SpanContext: " << _traceId << " " << _spanId << " " << _parentId << " " << _flags << " " << std::endl;
    }
    SpanContext(const SpanContext&) = delete;

    ~SpanContext()
    {
    }
};


#endif /* SPANCONTEXT_H */