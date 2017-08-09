#include <iostream>
#include "FileReporter.h"
using namespace OpenTracing;

FileReporter::~FileReporter()
{
#ifdef TRACER_DEBUG
    Php::out << "~FileReporter" << std::endl;
#endif    
}

FileReporter::FileReporter(const Php::Value& params)
{
#ifdef TRACER_DEBUG
    Php::out << "FileReporter::FileReporter" << std::endl;
#endif    
    /*
        $defaults = [
            'path' => 'tmp' . DIRECTORY_SEPARATOR,
            'file' => 'reports.log'
        ];

        $this->options = array_merge($defaults, $options);

        if (!is_dir($this->options['path'])) {
            throw new \Exception('Directory doesn\'t not exist');
        }
    */
}

void FileReporter::flush(const std::string& data) const
{
#ifdef TRACER_DEBUG
    Php::out << "    FileReporter::flush addr: " << std::endl;
#endif    
    /*
        file_put_contents(
            $this->options['path'] . DIRECTORY_SEPARATOR . $this->options['file'],
            $data . PHP_EOL,
            FILE_APPEND | LOCK_EX
        );
    */
}

const char* OpenTracing::FileReporter::_name() const
{
    return "FileReporter";
}
