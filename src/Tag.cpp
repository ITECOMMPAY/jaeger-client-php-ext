#include "Tag.h"

const std::string Tag::TAG_TYPE_HOST{ "host" };
const std::string Tag::TAG_TYPE_IP{ "ip" };

Tag::Tag(const std::string & key, const std::string & value) :
    _key{ key },
    _value{ value }
{
    Php::out << "    Tag::Tag addr: " << this << std::endl;
}

Tag::~Tag()
{
    Php::out << "    ~Tag" << std::endl;
}
