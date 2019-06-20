#include <iostream>
#include "JaegerSpan.h"
#include "Helper.h"
#include "Tracer.h"
using namespace OpenTracing;

JaegerSpan::JaegerSpan(SpanContext* context, const std::string& operationName, const Php::Value& startTime) :
    _operationName{ operationName },
    _startTime{},
    _endTime{},
    _context{ context }
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("\tJaegerSpan " + ss.str() + " constructor", true);
    }
    if (!startTime.isNull())
    {
        _startTime = static_cast<int64_t>(startTime);
    }
    else
    {
        auto ts = Helper::now();
        _startTime = ts.usec;
        if (!ts.errors.empty())
        {
            Php::Value log;
            log["error"] = "cannot generate starting timestamp for span " + std::to_string(context->_spanId) + ": " + ts.errors;
            this->addLogsInternal(log);
        }
    }
}

JaegerSpan::~JaegerSpan()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("~JaegerSpan " + ss.str() + " destructor");
    }
    delete _context;
    for (auto& iter : _tags)
        delete iter;
    _tags.clear();
    for (auto& iter : _logs)
        delete iter;
    _logs.clear();
}

void JaegerSpan::addTags(Php::Parameters& tags)
{
    {
        std::ostringstream ss;
        ss << "   ::addTags addr: " << this;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }

    if (!tags.empty() && Tracer::udp_transport)
    {
        //std::map<std::string, std::string> tags = params[0];
        //std::unordered_map<std::string, std::string> tags_unordered;
        //std::vector<std::pair<std::string, std::string>> tags_order; //appears how PHP shows an array using var_dump etc.

        //to keep the order of tags
        {
            Php::Value keys = Php::array_keys(tags[0]);
            Php::Value values = Php::array_values(tags[0]);

            for (int i = 0; i < keys.size(); i++)
            {
                //std::string key = keys[i];
                //std::string value = values[i];
                //std::cout << key << "-" << value << std::endl;
                //tags_order.push_back(std::make_pair(key, value));
                try
                {
                    bool exist = false;
                    for (auto& iter : this->_tags)
                    {
                        if (iter->_key == keys.get(i).stringValue())
                            exist = true;
                    }
                    if (!exist)
                    {
                        if (values.get(i).isBool())
                            this->_tags.push_back(new Tag(keys[i], values.get(i).boolValue()));
                        else if (values.get(i).isFloat())
                            this->_tags.push_back(new Tag(keys[i], values.get(i).floatValue()));
                        else
                            this->_tags.push_back(new Tag(keys[i], values.get(i).stringValue()));
                    }
                }
                catch (...)
                {
                    throw Php::Exception("  ::addTags - wrong parameters, check to be a [string=>string,]");
                }
            }
        }
    }
}

void JaegerSpan::addLogsInternal(const Php::Value& logs)
{
    if (!Tracer::udp_transport)
    {
        return;
    }

    std::vector<Tag*> tags;
    Php::Value keys = Php::array_keys(logs);
    Php::Value values = Php::array_values(logs);

    for (int i = 0; i < keys.size(); i++)
    {
        try
        {
            if (values.get(i).isBool())
                tags.push_back(new Tag(keys[i], values.get(i).boolValue()));
            else if (values.get(i).isFloat() || values.get(i).isNumeric())
                tags.push_back(new Tag(keys[i], values.get(i).floatValue()));
            else
                tags.push_back(new Tag(keys[i], values.get(i).stringValue()));
        }
        catch (...)
        {
            throw Php::Exception("  ::addLogs - wrong parameters, check it to be a [string=>string,]");
        }
    }
    this->_logs.push_back(new Log(tags));
}

void JaegerSpan::addLogs(Php::Parameters& logs)
{
    {
        std::ostringstream ss;
        ss << "   JaegerSpan(" << this << ")::addLogs ";
        Tracer::file_logger.PrintLine(ss.str(), true);
    }

    if (!logs.empty())
    {
        this->addLogsInternal(logs[0]);
    }

#ifdef EXTENDED_DEBUG
    {
        std::ostringstream ss;
        int i = 0;
        ss << "    Log count: " << this->_logs.size() << std::endl;
        for (auto& iter : this->_logs)
        {
            ss << "    Log" << ++i << ": " << iter << " time : " << iter->_timestamp << std::endl;
            for (auto& it : iter->_fields)
            {
                if (it->_vType == jaegertracing::thrift::TagType::BOOL)
                    ss << "        " << it->_key << " " << it->_vBool << std::endl;
                if (it->_vType == jaegertracing::thrift::TagType::DOUBLE)
                    ss << "        " << it->_key << " " << it->_vDouble << std::endl;
                if (it->_vType == jaegertracing::thrift::TagType::STRING)
                    ss << "        " << it->_key << " " << it->_vStr << std::endl;
            }
        }
        Tracer::file_logger.PrintLine(ss.str(), true);
    }
#endif
}

bool JaegerSpan::isSampled() const
{
    return (_context->_flags & SAMPLED_FLAG) == SAMPLED_FLAG;
}

bool JaegerSpan::isDebug() const
{
    return (_context->_flags & DEBUG_FLAG) == DEBUG_FLAG;
}

const char* JaegerSpan::_name() const
{
    return "JaegerSpan";
}

OpenTracing::JaegerSpan::operator std::string() const {
    std::stringstream ss(_name());
    // add span timing
    ss << ":" << "[" << _startTime << "-" << _endTime << "] ";

    ss << "Logs: ";
    for (auto log : _logs)
    {
        ss << (std::string) * log << "; ";
    }
    ss << "Tags: ";
    for (auto tag : _tags)
    {
        ss << (std::string) * tag << "; ";
    }
    ss << "Context: " << (std::string) * _context;

    return ss.str();
}
