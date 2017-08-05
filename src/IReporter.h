#ifndef IREPORTER_H
#define IREPORTER_H

class IReporter
{
public:
    virtual ~IReporter()
    {
        Php::out << "    ~IReporter addr: " << this << std::endl;
    }

    virtual void flush() const = 0;
    virtual const char* _name() const = 0;
};


#endif /* IREPORTER_H */

