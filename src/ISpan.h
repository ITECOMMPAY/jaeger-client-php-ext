#ifndef ISPAN_H
#define ISPAN_H
#include <phpcpp.h>
#include <iostream>

class ISpan : public Php::Base
{
public:
    virtual ~ISpan();

    /*Add tags*/
    //public function addTags(array $tags);

    //template <typename T>
    //ISpan * setTag(const std::string & key, const T & value);

    //virtual ISpan * setTag(const std::string & key, const std::string & value) = 0;


    virtual void addTag(Php::Parameters &params) = 0;
    virtual void addTags(const std::string &key) = 0;



    /*Add logs*/
    //public function addLogs(array $logs);

    virtual const char* _name() const = 0;

};

#endif /* ISPAN_H */

