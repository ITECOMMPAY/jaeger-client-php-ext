#include <iostream>
#include "Tag.h"
using namespace OpenTracing;

const std::string Tag::TAG_TYPE_HOST{ "host" };
const std::string Tag::TAG_TYPE_IP{ "ip" };

Tag::Tag(const std::string& key, const std::string& value) :
    _key{ key },
    _value{ value }
{
#ifdef TRACER_DEBUG
    Php::out << "        Tag::Tag addr: " << this << std::endl;
#endif    
}

Tag::~Tag()
{
#ifdef TRACER_DEBUG
    Php::out << "        ~Tag" << std::endl;
#endif    
}
