#include <sstream>
#include <algorithm>
#include "TextCarrier.h"
#include "Tracer.h"
using namespace OpenTracing;

const std::string TextCarrier::KFK_DELIMITER = "|";

void OpenTracing::TextCarrier::inject(const SpanContext* context, Php::Value& carrier)
{
    if (carrier.isArray())
    {
        carrier["X-B3-Traceid"] = std::to_string(context->_traceId);
        carrier["X-B3-Spanid"] = std::to_string(context->_spanId);
        carrier["X-B3-Parentspanid"] = std::to_string(context->_parentId);
        carrier["X-B3-Sampled"] = std::to_string(context->_flags);
    }
    else if (carrier.isString())
    {
        size_t delimPos = carrier.stringValue().find(TextCarrier::KFK_DELIMITER);
        std::ostringstream ss{ carrier.stringValue().substr(0, delimPos != std::string::npos ? delimPos : carrier.length()) + TextCarrier::KFK_DELIMITER, std::ios::ate };

        ss << (std::string)*context;

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
        ss <<
            (carrier_lowercase.count("x-b3-traceid") > 0 ? carrier_lowercase.at("x-b3-traceid") : "") << ":" <<
            (carrier_lowercase.count("x-b3-spanid") > 0 ? carrier_lowercase.at("x-b3-spanid") : "") << ":" <<
            (carrier_lowercase.count("x-b3-parentspanid") > 0 ? carrier_lowercase.at("x-b3-parentspanid") : "") << ":" <<
            (carrier_lowercase.count("x-b3-sampled") > 0 ? carrier_lowercase.at("x-b3-sampled") : "");
        Tracer::header_flag = std::stoul(carrier_lowercase.at("x-b3-sampled"));
    }
    catch (const std::out_of_range& e)
    {
    }

    return TextCarrier::extract(ss.str());
}
