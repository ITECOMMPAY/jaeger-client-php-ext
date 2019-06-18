#include <iostream>
#include "Log.h"
#include "Helper.h"
#include "Tracer.h"

OpenTracing::Log::Log(std::vector<OpenTracing::Tag*>& logs, const Php::Value& timestamp) :
    _fields{ std::move(logs) }
{
    if (timestamp.isNull())
    {
        auto ts = Helper::now();
        if (!ts.errors.empty())
        {
            this->_fields.push_back(new Tag("error", ts.errors));
        }
        this->_timestamp = ts.usec;
    }
    else
    {
        this->_timestamp = static_cast<int64_t>(timestamp);
    }
}

OpenTracing::Log::~Log()
{
    for (auto& iter : _fields)
        delete iter;
    _fields.clear();

    Tracer::file_logger.PrintLine("    ~Log", true);
}

OpenTracing::Log::operator std::string() const {
    std::stringstream ss("Log: ");
    for (auto tag : _fields) 
    {
        ss << (std::string) * tag << "; ";
    }
    ss << _timestamp << ";";
    return ss.str();
}
