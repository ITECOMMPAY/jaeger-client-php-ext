#ifndef IREPORTER_H
#define IREPORTER_H

class IReporter
{
public:
    //IReporter() = delete;
    //IReporter(const IReporter&) = delete;
    //IReporter(const IReporter&&) = delete;
    //IReporter& operator=(const IReporter&) = delete;
    //IReporter& operator=(const IReporter&&) = delete;
    
    //IReporter(const Php::Value &params){};
    
    virtual ~IReporter()
    {
        Php::out<<"    IReporter::~IReporter addr: " << this << std::endl;
    }
    
    virtual void flush() const = 0;
    
    /*Add tags*/
    //public function addTags(array $tags);
    
    //template <typename T>
    //ISpan * setTag(const std::string & key, const T & value);

    //virtual ISpan * setTag(const std::string & key, const std::string & value) = 0;
    
    
    /*Add logs*/
    //public function addLogs(array $logs);
    
};


#endif /* IREPORTER_H */

