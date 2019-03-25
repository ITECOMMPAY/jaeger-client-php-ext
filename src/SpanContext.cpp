#include <iostream>
#include "SpanContext.h"
#include "Tracer.h"

const std::string OpenTracing::SpanContext::OTR_DELIMITER = ":";

OpenTracing::SpanContext::SpanContext(const int64_t& traceId, const int64_t& spanId, const int64_t& parentId, const int& flags, const Php::Value& refType) :
    _traceId{ traceId },
    _spanId{ spanId },
    _parentId{ parentId },
    _flags{ flags },
    _refType( refType )
{
    {
        std::ostringstream ss;
        ss << "    SpanContext: " << _traceId << " " << _spanId << " " << _parentId << " " << _flags;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
}

OpenTracing::SpanContext::SpanContext(std::stringstream& ss) {
    std::string item;
    std::vector<std::string> parse;
    while (std::getline(ss, item, OTR_DELIMITER[0]))
        parse.push_back(item);

    if (parse.size() != 4)
        throw std::exception();

#ifdef EXTENDED_DEBUG
    {
        std::ostringstream ss;
        for (auto& iter : parse)
            ss << iter << ": size " << iter.size() << std::endl;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
#endif
    this->_traceId = std::stoul(parse[0]);
    this->_spanId = std::stoul(parse[1]);
    this->_parentId = std::stoul(parse[2]);
    this->_flags = std::stoul(parse[3]);
}

const char* OpenTracing::SpanContext::_name() const
{
    return "SpanContext";
}

OpenTracing::SpanContext::operator std::string() const {
    std::stringstream ss;
    ss <<
        _traceId << OpenTracing::SpanContext::OTR_DELIMITER <<
        _spanId << OpenTracing::SpanContext::OTR_DELIMITER <<
        _parentId << OpenTracing::SpanContext::OTR_DELIMITER <<
       _flags;

    return ss.str();
}
