#include <iostream>
#include <sstream>
#include <iomanip>
#include "SpanContext.h"
#include "Tracer.h"

const std::string OpenTracing::SpanContext::OTR_DELIMITER = ":";

OpenTracing::SpanContext::SpanContext(const int64_t& traceIdHigh, const int64_t& traceIdLow, const int64_t& spanId, const int64_t& parentId, const int& flags, const Php::Value& refType) :
    _traceIdHigh{ traceIdHigh },
    _traceIdLow{ traceIdLow },
    _spanId{ spanId },
    _parentId{ parentId },
    _flags{ flags },
    _refType(refType)
{
    {
        std::ostringstream ss;
        ss << "    SpanContext: " << _traceIdHigh << _traceIdLow << " " << _spanId << " " << _parentId << " " << _flags << " " << _refType.numericValue();
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
}

OpenTracing::SpanContext::SpanContext(std::stringstream& ss) {
    std::string item;
    std::vector<std::string> parse;
    while (std::getline(ss, item, OTR_DELIMITER[0]))
        parse.push_back(item);

    if (parse.size() < 4)
        throw Php::Exception("Context params less than 4");

#ifdef EXTENDED_DEBUG
    {
        std::ostringstream ss;
        for (auto& iter : parse)
            ss << iter << ": size " << iter.size() << std::endl;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
#endif
    std::string traceId{ parse[0] }, high, low;
    if (traceId.length() > 16)
    {
        high = traceId.substr(0, traceId.length() - 16);
        low = traceId.substr(high.length(), traceId.length());
    }
    else
    {
        high = "0";
        low = traceId == "" ? "0" : traceId.substr(0, traceId.length());
    }

    this->_traceIdHigh = std::stoul(high, 0, 16);
    this->_traceIdLow = std::stoul(low, 0, 16);
    this->_spanId = std::stoul(parse[1], 0, 16);
    this->_parentId = std::stoul(parse[2], 0, 16);
    this->_flags = std::stoul(parse[3]);
}

const char* OpenTracing::SpanContext::_name() const
{
    return "SpanContext";
}

OpenTracing::SpanContext::operator std::string() const {
    std::stringstream ss;
    std::ios::fmtflags ss_flags(ss.flags());

    if (_traceIdHigh == 0) {
        ss << std::hex << (_traceIdLow | 0);
    }
    else {
        ss << std::hex << (_traceIdHigh | 0) << std::setfill('0') << std::setw(16) << (_traceIdLow | 0);
    }

    ss.flags(ss_flags);
    ss <<
        std::hex << OpenTracing::SpanContext::OTR_DELIMITER <<
        _spanId << OpenTracing::SpanContext::OTR_DELIMITER <<
        _parentId << OpenTracing::SpanContext::OTR_DELIMITER <<
        _flags;

    return ss.str();
}
