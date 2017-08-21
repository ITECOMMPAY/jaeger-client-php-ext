#include <iostream>
#include "Tag.h"

const std::string OpenTracing::Tag::TAG_TYPE_HOST{ "host" };
const std::string OpenTracing::Tag::TAG_TYPE_IP{ "ip" };

OpenTracing::Tag::Tag(const std::string& key, const bool& value) :
    _key{ key },
    _vType{ TagType::BOOL },
    _vBool{ value },
    _vDouble{},
    _vStr{}
{
}

OpenTracing::Tag::Tag(const std::string& key, const double& value) :
    _key{ key },
    _vType{ TagType::DOUBLE },
    _vBool{},
    _vDouble{ value },
    _vStr{}
{
}

OpenTracing::Tag::Tag(const std::string& key, const std::string& value) :
    _key{ key },
    _vType{ TagType::STRING },
    _vBool{},
    _vDouble{},
    _vStr{ value }
{
#ifdef TRACER_DEBUG
    Php::out << "        Tag::Tag addr: " << this << std::endl;
#endif
}

OpenTracing::Tag::~Tag()
{
#ifdef TRACER_DEBUG
    Php::out << "        ~Tag" << std::endl;
#endif
}
