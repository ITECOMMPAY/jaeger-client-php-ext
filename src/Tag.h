#ifndef TAG_H
#define TAG_H

#include <phpcpp.h>

namespace OpenTracing
{
    class Tag
    {
    public:
        std::string _key;
        std::string _value;
        static const std::string TAG_TYPE_HOST;
        static const std::string TAG_TYPE_IP;

        Tag(const std::string& key, const std::string& value);
        Tag(const Tag&) = delete;
        ~Tag();
    };
}

#endif /* TAG_H */