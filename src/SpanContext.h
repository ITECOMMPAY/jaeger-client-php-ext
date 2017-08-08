#ifndef SPANCONTEXT_H
#define SPANCONTEXT_H
#include <phpcpp.h>
#include <iostream>

namespace OpenTracing
{
    class SpanContext : public Php::Base
    {
    public:
        uint64_t _traceId;
        uint64_t _spanId;
        uint64_t _parentId;
        int _flags;
        Php::Value _refType;

        SpanContext(const uint64_t& traceId, const uint64_t& spanId, const uint64_t& parentId, const int& flags, const Php::Value& refType = nullptr) :
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