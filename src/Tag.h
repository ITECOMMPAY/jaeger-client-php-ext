#ifndef TAG_H
#define TAG_H
#include <phpcpp.h>
#include <iostream>

class Tag
{
private:
    std::string _key;
    std::string _value;

public:
    static const std::string TAG_TYPE_HOST;
    static const std::string TAG_TYPE_IP;

    Tag(const std::string& key, const std::string& value);
    Tag(const Tag&) = delete;
    ~Tag();
};

#endif /* TAG_H */