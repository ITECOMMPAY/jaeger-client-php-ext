#ifndef ISPAN_H
#define ISPAN_H

#include <phpcpp.h>
#include "BaseExported.h"

namespace OpenTracing
{
    class ISpan : public BaseExported
    {
    public:
        virtual ~ISpan() = 0;
        /*Add tags*/
        virtual void addTags(Php::Parameters& tags) = 0;
        /*Add logs*/
        virtual void addLogs(Php::Parameters& logs) = 0;
        /*Name of a class to pass in Php::Object*/
        virtual const char* _name() const = 0;
    };
}

#endif /* ISPAN_H */

