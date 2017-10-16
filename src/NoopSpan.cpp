#include <iostream>
#include "NoopSpan.h"
#include "Tracer.h"
using namespace OpenTracing;

NoopSpan::NoopSpan()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopSpan " + ss.str() + " constructor");
    }
}

NoopSpan::~NoopSpan()
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("~NoopSpan " + ss.str() + " destructor");
    }
}

void NoopSpan::addTags(Php::Parameters& tags)
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopSpan " + ss.str() + " addTags");
    }
}

void NoopSpan::addLogs(Php::Parameters& logs)
{
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopSpan " + ss.str() + " addLogs");
    }
}

const char* NoopSpan::_name() const
{
    return "NoopSpan";
}
