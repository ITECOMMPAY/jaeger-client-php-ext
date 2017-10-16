#include <iostream>
#include "Log.h"
#include "Helper.h"
#include "Tracer.h"

OpenTracing::Log::Log(std::vector<OpenTracing::Tag*>& logs, const Php::Value& timestamp) :
    _fields{ std::move(logs) }
{
    this->_timestamp = !timestamp.isNull() ? static_cast<int64_t>(timestamp) : Helper::now();
}

OpenTracing::Log::~Log()
{
    for (auto& iter : _fields)
        delete iter;
    _fields.clear();

    Tracer::file_logger.PrintLine("    ~Log", true);
}
