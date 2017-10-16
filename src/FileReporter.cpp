#include <iostream>
#include "FileReporter.h"
#include "Tracer.h"
using namespace OpenTracing;

FileReporter::~FileReporter()
{
    Tracer::file_logger.PrintLine("~FileReporter", true);
}

FileReporter::FileReporter(const Php::Value& params)
{
    Tracer::file_logger.PrintLine("FileReporter::FileReporter", true);
    /*
        $defaults = [
            'path' => 'tmp' . DIRECTORY_SEPARATOR,
            'file' => 'reports.log'
        ];

        $this->options = array_merge($defaults, $options);

        if (!is_dir($this->options['path'])) {
            throw Php::Exception("Directory doesn\'t not exist");
        }
    */
}

void FileReporter::flush(const std::string& data) const
{
    Tracer::file_logger.PrintLine("    FileReporter::flush addr: ", true);
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
