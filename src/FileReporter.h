#ifndef FILEREPORTER_H
#define FILEREPORTER_H
#include <phpcpp.h>
#include <iostream>
#include "IReporter.h"

namespace OpenTracing
{
    class FileReporter : public IReporter
    {
    public:
        ~FileReporter()
        {
            Php::out << "~FileReporter" << std::endl;
        }

        FileReporter(const Php::Value& params)
        {
            Php::out << "FileReporter::FileReporter" << std::endl;
            /*$defaults = [
                'path' => 'tmp' . DIRECTORY_SEPARATOR,
                'file' => 'reports.log'
            ];

            $this->options = array_merge($defaults, $options);

            if (!is_dir($this->options['path'])) {
                throw new \Exception('Directory doesn\'t not exist');
            }*/


        }

        void flush() const
        {
            Php::out << "    FileReporter::flush addr: " << std::endl;
        }

        const char* _name() const
        {
            return "FileReporter";
        }
    };
}

#endif /* FILEREPORTER_H */

