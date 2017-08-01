#include "Log.h"
#include "Helper.h"

Log::Log(const std::vector<Tag*>& logs, const Php::Value& timestamp)
{
    _timestamp = (!timestamp.isNull()) ? static_cast<int64_t>(timestamp) : Helper::now();
    Php::out << "    Log::time " << _timestamp << std::endl;
    //foreach($logs as $logKey = > $logValue) {
    //    $this->fields[] = new Tag($logKey, $logValue);
}

Log::~Log()
{
    for (auto iter : _fields)
        delete iter;
    _fields.clear();

    Php::out << "    ~Log" << std::endl;
}
