#include <iostream>
#include "FileReporter.h"
using namespace OpenTracing;

FileReporter::~FileReporter()
{
    Php::out << "~FileReporter" << std::endl;
}

FileReporter::FileReporter(const Php::Value& params)
{
    Php::out << "FileReporter::FileReporter" << std::endl;
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

void FileReporter::flush(const uint8_t* data, int len) const
{
    Php::out << "    FileReporter::flush addr: " << std::endl;
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
