#ifndef SPANCONTEXT_H
#define SPANCONTEXT_H
#include <phpcpp.h>
#include <iostream>

namespace OpenTracing
{
    class SpanContext : public Php::Base
    {
    public:
        unsigned long long int _traceId;
        unsigned long long int _spanId;
        unsigned long long int _parentId;
        int _flags;
        Php::Value _refType;

        SpanContext(
            const unsigned long long int& traceId,
            const unsigned long long int& spanId,
            const unsigned long long int& parentId,
            const int& flags,
            const Php::Value& refType = nullptr) :
            _traceId{ traceId },
            _spanId{ spanId },
            _parentId{ parentId },
            _flags{ flags },
            _refType{ refType }
        {
            Php::out << "    SpanContext: " << _traceId << " " << _spanId << " " << _parentId << " " << _flags << " " << std::endl;
        }
        SpanContext(const SpanContext&) = delete;

        virtual ~SpanContext()
        {
        }

        const char * _name() const
        {
            return "SpanContext";
        }
    };
}

#endif /* SPANCONTEXT_H */