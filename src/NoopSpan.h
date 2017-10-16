#ifndef NOOPSPAN_H
#define NOOPSPAN_H

#include "ISpan.h"

namespace OpenTracing
{
    class NoopSpan : public ISpan
    {
    public:
        NoopSpan();
        ~NoopSpan();
        void addTags(Php::Parameters& tags);
        void addLogs(Php::Parameters& logs);
        const char* _name() const;
    };
}

#endif /* NOOPSPAN_H */
