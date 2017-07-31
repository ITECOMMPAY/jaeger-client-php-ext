#ifndef ISAMPLER_H
#define ISAMPLER_H

class ISampler
{
public:
    virtual ~ISampler()
    {
        Php::out << "    ISampler::~ISampler addr: " << this << std::endl;
    }

    virtual bool isSampled() = 0;
};

#endif /* ISAMPLER_H */

