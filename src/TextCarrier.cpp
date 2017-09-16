#include <sstream>
#include <algorithm>
#include "TextCarrier.h"
#include "Tracer.h"
using namespace OpenTracing;

const std::string TextCarrier::KFK_DELIMITER = "|";
const std::string TextCarrier::OTR_DELIMITER = ":";

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

        ss <<
            context->_traceId << TextCarrier::OTR_DELIMITER <<
            context->_spanId << TextCarrier::OTR_DELIMITER <<
            context->_parentId << TextCarrier::OTR_DELIMITER <<
            context->_flags;

        carrier = ss.str();
    }
}

SpanContext* TextCarrier::extract(const std::string& carrier)
{
    std::stringstream ss{ carrier.substr(carrier.find(TextCarrier::KFK_DELIMITER) + 1, carrier.length()) };
    std::string item;
    std::vector<std::string> parse;
    while (std::getline(ss, item, TextCarrier::OTR_DELIMITER[0]))
        parse.push_back(item);

    if (parse.size() != 4)
        return nullptr;

    if (0)
    {
        std::ostringstream ss;
        for (auto& iter : parse)
            ss << iter << ": size " << iter.size() << std::endl;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }

    try
    {
        return new SpanContext(
            std::stoul(parse[0]),
            std::stoul(parse[1]),
            std::stoul(parse[2]),
            std::stoul(parse[3])
        );
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
