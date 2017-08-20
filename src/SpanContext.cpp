#include <iostream>
#include "SpanContext.h"

OpenTracing::SpanContext::SpanContext(const int64_t& traceId, const int64_t& spanId, const int64_t& parentId, const int& flags, const Php::Value& refType) :
    _traceId{ traceId },
    _spanId{ spanId },
    _parentId{ parentId },
    _flags{ flags },
    _refType{ refType }
{
#ifdef TRACER_DEBUG
    Php::out << "    SpanContext: " << _traceId << " " << _spanId << " " << _parentId << " " << _flags << " " << std::endl;
#endif    
}

const char* OpenTracing::SpanContext::_name() const
{
    return "SpanContext";
}
