#include "JaegerSpan.h"

void JaegerSpan::addTags(Php::Parameters & params)
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

void JaegerSpan::addLogs(Php::Parameters & logs)
{
    //$this->logs[] = new Log($logs);
}

const char * JaegerSpan::_name() const
{
    return "JaegerSpan";
}
