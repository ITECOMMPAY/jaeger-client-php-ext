#ifndef TEXTCARRIER_H
#define TEXTCARRIER_H

#include <phpcpp.h>
#include "SpanContext.h"

namespace OpenTracing
{
    class TextCarrier
    {
    public:
        static const std::string KFK_DELIMITER;

        //Inject context into carrier - string (key) or array (headers)
        //carrier key e.g. 1-30-1499218504
        static void inject(const SpanContext* context, Php::Value& carrier);

        //Extract context from the text carrier
        //carrier e.g. 1-30-1499218504|3929956739498646081:8078931732949298510:6117163659416795557:1
        static SpanContext* extract(const std::string& carrier);

        //Extract context from the text map carrier
        static SpanContext* extract(const std::map<std::string, std::string>& carrier);

    };
}

#endif /* TEXTCARRIER_H */
