#include "JaegerSpan.h"

JaegerSpan::JaegerSpan(SpanContext* context, const std::string& operationName, const Php::Value& startTime) :
    _operationName{ operationName },
    _context{ context }
{
    Php::out << "JaegerSpan::JaegerSpan addr: " << this << std::endl;
    _startTime = !startTime.isNull() ? static_cast<int64_t>(startTime) : Helper::now();
}

JaegerSpan::~JaegerSpan()
{
    Php::out << "~JaegerSpan addr: " << this << std::endl;
    delete _context;
    for (auto iter : _tags)
        delete iter;
    _tags.clear();
    for (auto iter : _logs)
        delete iter;
    _logs.clear();
}

void JaegerSpan::addTags(Php::Parameters& params)
{
    Php::out << "JaegerSpan::addTags addr: " << this << std::endl;
    /*
    public function addTags(array $tags)
    {
    foreach ($tags as $tagKey => $tagValue) {
    $this->tags[$tagKey] = new Tag($tagKey, $tagValue);
    }
    }
    */
}

void JaegerSpan::addLogs(Php::Parameters& logs)
{
    //$this->logs[] = new Log($logs);
}

const char * JaegerSpan::_name() const
{
    return "JaegerSpan";
}
