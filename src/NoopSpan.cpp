#include <iostream>
#include "NoopSpan.h"
#include "Tracer.h"
using namespace OpenTracing;

NoopSpan::NoopSpan()
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::NoopSpan addr: " << this << std::endl;
#endif
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopSpan " + ss.str() + " constructor");
    }
}

NoopSpan::~NoopSpan()
{
#ifdef TRACER_DEBUG
    Php::out << "~NoopSpan addr: " << this << std::endl;
#endif
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("~NoopSpan " + ss.str() + " destructor");
    }
}

void NoopSpan::addTags(Php::Parameters& tags)
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::addTags addr: " << this << std::endl;
#endif
    {
        std::ostringstream ss;
        ss << this;
        Tracer::file_logger.PrintLine("NoopSpan " + ss.str() + " addTags");
    }
}

void NoopSpan::addLogs(Php::Parameters& logs)
{
#ifdef TRACER_DEBUG
    Php::out << "NoopSpan::addLogs addr: " << this << std::endl;
#endif
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
