#ifndef PROCESS_H
#define PROCESS_H
#include <phpcpp.h>
#include <iostream>
#include "Tag.h"

namespace OpenTracing
{
    class Process
    {
    private:
        std::string _serviceName;
        std::vector<Tag*> _tags;
    public:
        static const std::string DEFAULT_SERVICE_NAME;

        Process(const std::string& serviceName);
        Process(const Process&) = delete;
        ~Process();
    };
}


#endif /* PROCESS_H */