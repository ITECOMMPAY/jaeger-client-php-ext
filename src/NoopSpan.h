#ifndef NOOPSPAN_H
#define NOOPSPAN_H
#include "ISpan.h"

class NoopSpan : public ISpan
{
public:
    NoopSpan()
    {
        Php::out<<"NoopSpan::NoopSpan addr: " << this << std::endl;
    }
    
    ~NoopSpan()
    {
        Php::out<<"NoopSpan::~NoopSpan addr: " << this << std::endl;
    }
    
    void addTag(Php::Parameters &params)
    {
        Php::out << "NoopSpan::addTag addr: " << this << std::endl;
    };
    void addTags(const std::string &key){};
    
    const char* _name() const
    {
        return "NoopSpan";
    }
};

#endif /* NOOPSPAN_H */

