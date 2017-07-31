#ifndef NOOPTRACER_H
#define NOOPTRACER_H

#include <phpcpp.h>
#include <iostream>
#include "ITracer.h"
#include "NoopSpan.h"

class NoopTracer : public ITracer
{
public:
    ~NoopTracer()
    {
        Php::out << "NoopTracer::~NoopTracer addr: " << this << std::endl;
    }

    NoopTracer()
    {
        Php::out << "NoopTracer constructor" << std::endl;
    };

    void init(std::string serviceName) const
    {
        this->print();
    }

    //NoopTracer(const NoopTracer& in)
    //{
    //    Php::out<< "NoopTracer copy constructor" << std::endl;
    //}

    ISpan* startSpan(const std::string& operationName) const
    {
        Php::out << "NoopTracer::startSpan, operationName = " << operationName << std::endl;

        return new NoopSpan();
    }

    void print() const
    {
        Php::out << "NoopTracer::print" << std::endl;
    };

    const char* __toString() const
    {
        return "NoopTracer::__toString";
    }

    const char* _name() const
    {
        return "NoopTracer";
    }
};


#endif /* NOOPTRACER_H */

