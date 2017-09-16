#include <iostream>
#include "SpanContext.h"
#include "Tracer.h"

OpenTracing::SpanContext::SpanContext(const int64_t& traceId, const int64_t& spanId, const int64_t& parentId, const int& flags, const Php::Value& refType) :
    _traceId{ traceId },
    _spanId{ spanId },
    _parentId{ parentId },
    _flags{ flags },
    _refType{ refType }
{
    {
        std::ostringstream ss;
        ss << "    SpanContext: " << _traceId << " " << _spanId << " " << _parentId << " " << _flags;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
}

const char* OpenTracing::SpanContext::_name() const
{
    return "SpanContext";
}
