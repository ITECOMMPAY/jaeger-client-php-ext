#include <sstream>
#include "TextCarrier.h"

const std::string TextCarrier::KFK_DELIMITER = "|";
const std::string TextCarrier::OTR_DELIMITER = ":";

void TextCarrier::inject(const SpanContext* context, std::string& carrier)
{
    std::ostringstream ss;
    ss << context->_traceId << TextCarrier::OTR_DELIMITER <<
        context->_spanId << TextCarrier::OTR_DELIMITER <<
        context->_parentId << TextCarrier::OTR_DELIMITER <<
        context->_flags;
    carrier += TextCarrier::KFK_DELIMITER + ss.str();
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

#ifdef TRACER_DEBUG
    for (auto& iter : parse)
        Php::out << iter << ": size " << iter.size() << std::endl;
#endif

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
