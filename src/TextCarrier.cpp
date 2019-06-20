#include <sstream>
#include <iomanip>
#include <algorithm>
#include "TextCarrier.h"
#include "Tracer.h"
using namespace OpenTracing;

const std::string TextCarrier::KFK_DELIMITER = "|";

void OpenTracing::TextCarrier::inject(const SpanContext* context, Php::Value& carrier)
{
    if (carrier.isArray())
    {
        std::stringstream ss;
        std::ios::fmtflags ss_flags(ss.flags());

        if (context->_traceIdHigh == 0) {
            ss << std::hex << (context->_traceIdLow | 0);
        }
        else {
            ss << std::hex << (context->_traceIdHigh | 0) << std::setfill('0') << std::setw(16) << (context->_traceIdLow | 0);
        }
        carrier["x-b3-traceid"] = ss.str();
        ss.flags(ss_flags);
        ss.str(std::string());

        ss << std::hex << (context->_spanId | 0);
        carrier["x-b3-spanid"] = ss.str();
        ss.flags(ss_flags);
        ss.str(std::string());

        ss << std::hex << (context->_parentId | 0);
        carrier["x-b3-parentspanid"] = ss.str();
        ss.flags(ss_flags);
        ss.str(std::string());

        carrier["x-b3-sampled"] = std::to_string(context->_flags);
    }
    else if (carrier.isString())
    {
        size_t delimPos = carrier.stringValue().find(TextCarrier::KFK_DELIMITER);
        std::ostringstream ss{ carrier.stringValue().substr(0, delimPos != std::string::npos ? delimPos : carrier.length()) + TextCarrier::KFK_DELIMITER, std::ios::ate };

        ss << (std::string) * context;

        carrier = ss.str();
    }
}

SpanContext* TextCarrier::extract(const std::string& carrier)
{
    std::stringstream ss{ carrier.substr(carrier.find(TextCarrier::KFK_DELIMITER) + 1, carrier.length()) };

    try
    {
        return new SpanContext(ss);
    }
    catch (...)
    {
        return nullptr;
    }
}

SpanContext* OpenTracing::TextCarrier::extract(const std::map<std::string, std::string>& carrier)
{
    std::stringstream ss;

    //make lowercase keys
    std::map<std::string, std::string> carrier_lowercase;
    for (auto& iter : carrier)
    {
        std::string key{ iter.first };
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        carrier_lowercase[key] = iter.second;
    }

    try
    {
        ss << (carrier_lowercase.count("x-b3-traceid") > 0 ? carrier_lowercase.at("x-b3-traceid") : "") << ":"
            << (carrier_lowercase.count("x-b3-spanid") > 0 ? carrier_lowercase.at("x-b3-spanid") : "") << ":"
            << (carrier_lowercase.count("x-b3-parentspanid") > 0 ? carrier_lowercase.at("x-b3-parentspanid") : "") << ":"
            << (carrier_lowercase.count("x-b3-sampled") > 0 ? carrier_lowercase.at("x-b3-sampled") : "");
        Tracer::header_flag = std::stoul(carrier_lowercase.at("x-b3-sampled"));
    }
    catch (const std::out_of_range& e)
    {
    }

    return TextCarrier::extract(ss.str());
}
