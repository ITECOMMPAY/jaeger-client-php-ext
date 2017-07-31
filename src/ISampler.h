#ifndef ISAMPLER_H
#define ISAMPLER_H

class ISampler
{
public:
    //IReporter() = delete;
    //IReporter(const IReporter&) = delete;
    //IReporter(const IReporter&&) = delete;
    //IReporter& operator=(const IReporter&) = delete;
    //IReporter& operator=(const IReporter&&) = delete;

    //IReporter(const Php::Value &params){};

    virtual ~ISampler()
    {
        Php::out << "    ISampler::~ISampler addr: " << this << std::endl;
    }

    virtual bool isSampled() = 0;

    /*Add logs*/
    //public function addLogs(array $logs);

};


#endif /* ISAMPLER_H */

