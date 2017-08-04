#include "JaegerSpan.h"
#include <unordered_map>
#include <map>

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
    //public function addTags(array $tags)
    //{
    //    foreach($tags as $tagKey = > $tagValue) {
    //        $this->tags[$tagKey] = new Tag($tagKey, $tagValue);
    //    }
    //}

    Php::out << "   ::addTags addr: " << this << std::endl;

    if (!params.empty() > 0)
    {
        //std::map<std::string, std::string> tags = params[0];
        //std::unordered_map<std::string, std::string> tags_unordered;
        //std::vector<std::pair<std::string, std::string>> tags_order; //appears how PHP shows an array using var_dump etc.

        /*hack to keep the order of tags*/
        /*could be expensive*/
        {
            Php::Value keys = Php::array_keys(params[0]);
            Php::Value values = Php::array_values(params[0]);

            //Php::call("var_dump", keys);
            for (int i = 0; i < keys.size(); i++)
            {
                //std::string key = keys[i];
                //std::string value = values[i];
                //Php::out << key << "-" << value << std::endl;
                //tags_order.push_back(std::make_pair(key, value));
                try
                {
                    this->_tags.push_back(new Tag(keys[i], values[i]));
                }
                catch (...)
                {
                    throw Php::Exception("  ::addTags - wring parameters, check to be a [string=>string,]");
                }
            }
        }

        //Php::out << std::endl;
        //auto m = params[0].mapValue();
        //for (auto& iter : m)
        //    Php::out << iter.first << " " << iter.second << std::endl;

        //Php::out << std::endl;
        //for (auto& iter : tags)
        //    Php::out << iter.first << " " << iter.second << std::endl;

        //Php::out << std::endl;
        //for (auto& iter : tags_order)
        //    Php::out << iter.first << " " << iter.second << std::endl;


        //Php::out << std::endl;
        //for (auto& iter : this->_tags)
        //    Php::out << iter->_key << " " << iter->_value << std::endl;
    }
}

void JaegerSpan::addLogs(Php::Parameters& logs)
{
    //$this->logs[] = new Log($logs);

    //foreach($logs as $logKey = > $logValue) {
    //    $this->fields[] = new Tag($logKey, $logValue);

    Php::out << "   JaegerSpan::addLogs" << std::endl;

    if (!logs.empty() > 0)
    {
        std::vector<Tag*> tags;

        /*hack to keep the order of tags*/
        /*could be expensive*/
        {
            Php::Value keys = Php::array_keys(logs[0]);
            Php::Value values = Php::array_values(logs[0]);

            for (int i = 0; i < keys.size(); i++)
            {
                //std::string key = keys[i];
                //std::string value = values[i];
                //Php::out << key << "-" << value << std::endl;
                //tags_order.push_back(std::make_pair(key, value));
                try
                {
                    tags.push_back(new Tag(keys[i], values[i]));
                }
                catch (...)
                {
                    throw Php::Exception("  ::addTags - wring parameters, check to be a [string=>string,]");
                }

            }
            Php::out << "     size tags = " << tags.size() << std::endl;
        }

        this->_logs.push_back(new Log(tags));
        Php::out << "     size tags = " << tags.size() << std::endl;

        //Php::out << std::endl;
        //for (auto& iter : this->_logs)
        //{
        //    Php::out << iter->_timestamp << std::endl;
        //    for (auto& it : iter->_fields)
        //    {
        //        Php::out << "   "<<it->_key << " " << it->_value << std::endl;
        //    }
        //}
    }
}

bool JaegerSpan::isSampled() const
{
    return _context->_flags & SAMPLED_FLAG;
}

const char * JaegerSpan::_name() const
{
    return "JaegerSpan";
}
