#ifndef SPANCONTEXT_H
#define SPANCONTEXT_H

#include <phpcpp.h>
#include "BaseExported.h"

namespace OpenTracing
{
    class SpanContext : public BaseExported
    {
    public:
        static const std::string OTR_DELIMITER;

        int64_t _traceId;
        int64_t _spanId;
        int64_t _parentId;
        int _flags;
        Php::Value _refType;

        SpanContext(const int64_t& traceId, const int64_t& spanId, const int64_t& parentId, const int& flags, const Php::Value& refType = nullptr);
        SpanContext(std::stringstream&);
        SpanContext(const SpanContext&) = delete;
        const char* _name() const;

        operator std::string() const override;
    };
}

#endif /* SPANCONTEXT_H */