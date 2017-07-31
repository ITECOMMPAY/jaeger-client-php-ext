#ifndef JAEGERSPAN_H
#define JAEGERSPAN_H
#include "ISpan.h"

class JaegerSpan : public ISpan
{
public:
    JaegerSpan()
    {
        Php::out << "JaegerSpan::JaegerSpan addr: " << this << std::endl;
    }

    ~JaegerSpan()
    {
        Php::out << "JaegerSpan::~JaegerSpan addr: " << this << std::endl;
    }

    void addTag(Php::Parameters &params)
    {
        Php::out << "JaegerSpan::addTag addr: " << this << std::endl;
    };
    void addTags(const std::string &key) {};

    const char* _name() const
    {
        return "JaegerSpan";
    }
};

#endif /* JAEGERSPAN_H */

