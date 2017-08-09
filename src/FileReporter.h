#ifndef FILEREPORTER_H
#define FILEREPORTER_H

#include <phpcpp.h>
#include "IReporter.h"

namespace OpenTracing
{
    class FileReporter : public IReporter
    {
    public:
        ~FileReporter();
        FileReporter(const Php::Value& params);
        void flush(const uint8_t* data, int len) const;
        const char* _name() const;
    };
}

#endif /* FILEREPORTER_H */

