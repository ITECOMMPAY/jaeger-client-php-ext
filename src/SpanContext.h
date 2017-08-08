#ifndef SPANCONTEXT_H
#define SPANCONTEXT_H

#include <phpcpp.h>

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

        SpanContext(const uint64_t& traceId, const uint64_t& spanId, const uint64_t& parentId, const int& flags, const Php::Value& refType = nullptr);
        SpanContext(const SpanContext&) = delete;
        const char* _name() const;
    };
}

#endif /* SPANCONTEXT_H */