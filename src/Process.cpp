#include "Process.h"

const std::string Process::DEFAULT_SERVICE_NAME{ "unknown" };

Process::Process(const std::string & serviceName) :
    _serviceName{ serviceName }
{
    Php::out << "    Process::Process addr: " << this << std::endl;

    _tags.push_back(new Tag(Tag::TAG_TYPE_HOST, "temp1"/*gethostname()*/));
    _tags.push_back(new Tag(Tag::TAG_TYPE_IP, "temp2"/*Helper::getCurrentIp()*/));
}

Process::~Process()
{
    for (auto iter : _tags)
        delete iter;
    _tags.clear();

    Php::out << "    ~Process" << std::endl;
}
