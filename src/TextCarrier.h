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
        static const std::string OTR_DELIMITER;

        //Inject context into text carrier
        //carrier e.g. 1-30-1499218504
        static void inject(const SpanContext* context, std::string& carrier);
        //Extract context from the text carrier
        //carrier e.g. 1-30-1499218504|3929956739498646081:8078931732949298510:6117163659416795557:1
        static SpanContext* extract(const std::string& carrier);
    private:

    };
}
#endif /* TEXTCARRIER_H */