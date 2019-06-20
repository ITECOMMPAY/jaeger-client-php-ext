#ifndef BASE_EXPORTED_H
#define BASE_EXPORTED_H

#include <phpcpp.h>

namespace OpenTracing
{

    class BaseExported : public Php::Base {
    public:
        virtual operator std::string() const {
            return "";
        }

        Php::Value __toString() {
            return (std::string) * this;
        }
    };

}


#endif // BASE_EXPORTED_H