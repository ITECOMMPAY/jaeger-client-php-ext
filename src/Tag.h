#ifndef TAG_H
#define TAG_H

#include <phpcpp.h>
#include "thrift-gen/jaeger_types.h"

namespace OpenTracing
{
    class Tag
    {
    public:
        std::string _key;
        jaegertracing::thrift::TagType::type _vType;
        bool _vBool;
        double _vDouble;
        std::string _vStr;

        static const std::string TAG_TYPE_HOST;
        static const std::string TAG_TYPE_IP;

        Tag(const std::string& key, const bool& value);
        Tag(const std::string& key, const double& value);
        Tag(const std::string& key, const std::string& value);
        Tag(const Tag&) = delete;
        ~Tag();

        operator std::string() const;
    };
}

#endif /* TAG_H */
