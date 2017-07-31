#ifndef JAEGERSPAN_H
#define JAEGERSPAN_H
#include "ISpan.h"

class JaegerSpan : public ISpan
{
public:
    JaegerSpan();
    ~JaegerSpan();
    void addTags(Php::Parameters &params);;
    //void addTags(const std::string &key) {};
    void addLogs(Php::Parameters& logs);
    const char* _name() const;
};

#endif /* JAEGERSPAN_H */
