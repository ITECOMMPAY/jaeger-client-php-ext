#include <iostream>
#include "JaegerSpan.h"
#include "Helper.h"
using namespace OpenTracing;

JaegerSpan::JaegerSpan(SpanContext* context, const std::string& operationName, const Php::Value& startTime) :
    _operationName{ operationName },
    _startTime{},
    _endTime{},
    _context{ context }
{
#ifdef TRACER_DEBUG
    Php::out << "    JaegerSpan::JaegerSpan addr: " << this << std::endl;
#endif    
    _startTime = !startTime.isNull() ? static_cast<int64_t>(startTime) : Helper::now();
}

JaegerSpan::~JaegerSpan()
{
#ifdef TRACER_DEBUG
    Php::out << "    ~JaegerSpan addr: " << this << std::endl;
#endif    
    delete _context;
    for (auto& iter : _tags)
        delete iter;
    _tags.clear();
    for (auto& iter : _logs)
        delete iter;
    _logs.clear();
}

void JaegerSpan::addTags(Php::Parameters& params)
{
#ifdef TRACER_DEBUG
    Php::out << "   ::addTags addr: " << this << std::endl;
#endif    

    if (!params.empty() > 0)
    {
        //std::map<std::string, std::string> tags = params[0];
        //std::unordered_map<std::string, std::string> tags_unordered;
        //std::vector<std::pair<std::string, std::string>> tags_order; //appears how PHP shows an array using var_dump etc.

        //to keep the order of tags
        {
            Php::Value keys = Php::array_keys(params[0]);
            Php::Value values = Php::array_values(params[0]);

            for (int i = 0; i < keys.size(); i++)
            {
                //std::string key = keys[i];
                //std::string value = values[i];
                //Php::out << key << "-" << value << std::endl;
                //tags_order.push_back(std::make_pair(key, value));
                try
                {
                    this->_tags.push_back(new Tag(keys[i], values[i]));
                }
                catch (...)
                {
                    throw Php::Exception("  ::addTags - wrong parameters, check to be a [string=>string,]");
                }
            }
        }
    }
}

void JaegerSpan::addLogs(Php::Parameters& logs)
{
#ifdef TRACER_DEBUG
    Php::out << "   JaegerSpan(" << this << ")::addLogs " << std::endl;
#endif    

    if (!logs.empty() > 0)
    {
        std::vector<Tag*> tags;

        //to keep the order of tags
        {
            Php::Value keys = Php::array_keys(logs[0]);
            Php::Value values = Php::array_values(logs[0]);

            for (int i = 0; i < keys.size(); i++)
            {
                try
                {
                    tags.push_back(new Tag(keys[i], values[i]));
                }
                catch (...)
                {
                    throw Php::Exception("  ::addTags - wrong parameters, check it to be a [string=>string,]");
                }

            }
        }

        this->_logs.push_back(new Log(tags));
    }
#ifdef TRACER_DEBUG
    JaegerSpan* temp = dynamic_cast<JaegerSpan*>(this);
    int i = 0;
    Php::out << "    Log count: " << temp->_logs.size() << std::endl;
    for (auto& iter1 : temp->_logs)
    {
        Php::out << "    Log" << ++i << ": " << iter1 << " time : " << iter1->_timestamp << std::endl;
        for (auto& it : iter1->_fields)
        {
            Php::out << "        " << it->_key << " " << it->_value << std::endl;
        }
    }
#endif
}

bool JaegerSpan::isSampled() const
{
    return _context->_flags & SAMPLED_FLAG;
}

const char* JaegerSpan::_name() const
{
    return "JaegerSpan";
}
