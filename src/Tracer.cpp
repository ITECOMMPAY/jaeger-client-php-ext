#include <iostream>
#include <vector>
#include <algorithm>
#include "Tracer.h"
#include "UdpReporter.h"
#include "FileReporter.h"
#include "PercentageSampler.h"
#include "NoopTracer.h"
#include "JaegerTracer.h"
#include "NoopSpan.h"
#include "JaegerSpan.h"
using namespace OpenTracing;

Tracer::~Tracer()
{
    Tracer::file_logger.PrintLine("~Tracer");
}

IReporter* OpenTracing::buildReporter(const Php::Value& settings)
{
    if (Php::array_key_exists("type", settings) && Php::array_key_exists("options", settings) && is_array(settings["options"]))
    {
        if (settings["type"] == "file")
        {
            return new FileReporter(settings["options"]);
        }
        else if (settings["type"] == "udp")
        {
            return new UdpReporter(settings["options"]);
        }
    }

    Php::Value defaults;
    defaults["reporter"]["type"] = "udp";
    defaults["reporter"]["options"]["addr"] = "localhost";
    defaults["reporter"]["options"]["port"] = 6831;

    return buildReporter(defaults["reporter"]);
}

ISampler* OpenTracing::buildSampler(const Php::Value& settings)
{
    if (Php::array_key_exists("type", settings) && Php::array_key_exists("options", settings) && is_array(settings["options"]))
    {
        if (settings["type"] == "percentage")
        {
            return new PercentageSampler(settings["options"]);
        }
    }

    Php::Value defaults;
    defaults["sampler"]["type"] = "percentage";
    defaults["sampler"]["options"]["percents"] = 1;

    return buildSampler(defaults["sampler"]);
}

void Tracer::init(Php::Parameters& params)
{
    Php::Value defaults = createDefaultParamsList();

    std::string serviceName = params[0];
    Php::Value settings = nullptr;

    if (params.size() == 1)
    {
        settings = defaults;
    }
    else
    {
        settings = params[1];
        if (settings.isArray())
        {
            settings = Php::call("array_merge", defaults, settings);
            userTracerSettings["serviceName"] = serviceName;
            userTracerSettings["reporter_type"] = settings["reporter"]["type"].value().stringValue();
            userTracerSettings["reporter_addr"] = settings["reporter"]["options"]["addr"].value().stringValue();
            userTracerSettings["reporter_port"] = settings["reporter"]["options"]["port"].value().stringValue();
        }
    }

    initInternal(serviceName, settings);
}

void OpenTracing::initInternal(const std::string& serviceName, const Php::Value& settings)
{
    Tracer::file_logger._enabled = settings["debug_output"];
    Tracer::udp_transport = settings["udp_transport"];

#ifdef EXTENDED_DEBUG
    {
        std::ostringstream ss;
        ss << Tracer::global_tracer;
        Tracer::file_logger.PrintLine("Tracer::init - delete global_tracer " + ss.str());
    }
#endif

    delete Tracer::global_tracer;
    if (!settings["enabled"])
    {
        Tracer::global_tracer = new NoopTracer();
    }
    else
    {
        IReporter* reporter = buildReporter(settings["reporter"]);
        ISampler* sampler = buildSampler(settings["sampler"]);
        Tracer::global_tracer = new JaegerTracer(reporter, sampler);
    }

    {
        std::ostringstream ss;
        ss << Tracer::global_tracer;
        Tracer::file_logger.PrintLine("Tracer::init " + ss.str() + " " + serviceName);
    }

    Tracer::global_tracer->init(serviceName);
}

Php::Value Tracer::startSpan(Php::Parameters& params)
{
    Tracer::file_logger.PrintLine("Tracer::startSpan", true);

    std::string operationName = params[0];
    Php::Value options = nullptr;

    if (params.size() == 1)
    {
        Tracer::file_logger.PrintLine("    1 parameter", true);
    }
    else
    {
        options = params[1];
        Tracer::file_logger.PrintLine("    2 parameters", true);
    }

    return startSpanInternal(operationName, options);
}

Php::Value OpenTracing::startSpanInternal(const std::string& operationName, const Php::Value& options)
{
    ISpan* span = nullptr;

    if (Tracer::global_tracer != nullptr)
    {
        span = Tracer::global_tracer->startSpan(operationName, options);

#ifdef EXTENDED_DEBUG
        {
            std::ostringstream ss;
            ss << "--- startSpanInternal (tracer " << Tracer::global_tracer << ")\n\t\t\t\t\t\t\t\t\t\t    startSpan returned: " << span;
            if (strcmp(span->_name(), "JaegerSpan") == 0)
            {
                ss << " (SpanContext: " << dynamic_cast<JaegerSpan*>(span)->_context << " )"
                    << " total spans: " << dynamic_cast<JaegerTracer*>(Tracer::global_tracer)->_spans.size()
                    << " " << dynamic_cast<JaegerTracer*>(Tracer::global_tracer)->_process->_serviceName << " "
                    << dynamic_cast<JaegerSpan*>(span)->_operationName;
            }
            Tracer::file_logger.PrintLine(ss.str());
            ss.str("");
            ss.clear();

            {
                std::ostringstream ss;
                for (auto& iter : dynamic_cast<JaegerTracer*>(Tracer::global_tracer)->_spans)
                {
                    JaegerSpan* temp = dynamic_cast<JaegerSpan*>(iter.second);
                    ss << "\t\t\tspan id: " << iter.first
                        << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t_startTime: " << temp->_startTime
                        << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t_endTime  : " << temp->_endTime;
                    if (temp->_context != nullptr)
                    {
                        ss << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t_context: "
                            << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t_traceIdHigh: " << temp->_context->_traceIdHigh
                            << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t_traceIdLow: " << temp->_context->_traceIdLow
                            << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t_spanId: " << temp->_context->_spanId
                            << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t_parentId: " << temp->_context->_parentId;
                        if (!temp->_context->_refType.isNull())
                        {
                            ss << "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t_refType: " << temp->_context->_refType.numericValue();
                        }

                        ss << "\n\n\t\t\t\t\t\t\t\t\t\t";
                    }
                }
                Tracer::file_logger.PrintLine(ss.str());
            }
        }
#endif
    }

#ifdef EXTENDED_DEBUG
    {
        std::ostringstream ss;
        ss << "--- startSpanInternal (tracer " << Tracer::global_tracer << ")";
        Tracer::file_logger.PrintLine(ss.str());
    }
#endif

    return span == nullptr ? Php::Object("NoopSpan", new NoopSpan()) : Php::Object(span->_name(), span);
}

Php::Value Tracer::getCurrentSpan()
{
    ISpan* span = nullptr;
    if (global_tracer != nullptr)
    {
        span = global_tracer->getCurrentSpan();
    }
    //todo - return a copy of object... (zend issue)
    return span == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(span->_name(), span);
}

Php::Value Tracer::getCurrentTraceId()
{
    std::string _traceId = std::string();
    if (global_tracer != nullptr)
    {
        _traceId = global_tracer->getCurrentTraceId();
    }

    return Php::Value(_traceId);
}

Php::Value Tracer::getCurrentSpanId(Php::Parameters& params)
{
    Php::Value _span = params[0];
    int64_t _spanId = int64_t();
    if (!_span.isNull())
    {
        if (!_span.instanceOf("ISpan"))
            throw Php::Exception("Wrong parameter passed, should be ISpan");
        ISpan* span = (ISpan*)_span.implementation();
        if (global_tracer != nullptr)
        {
            _spanId = global_tracer->getCurrentSpanId(span);
        }
    }
    return Php::Value(_spanId);
}

Php::Value Tracer::getCurrentParentId(Php::Parameters& params)
{
    Php::Value _span = params[0];
    int64_t _parentId = int64_t();
    if (!_span.isNull())
    {
        if (!_span.instanceOf("ISpan"))
            throw Php::Exception("Wrong parameter passed, should be ISpan");
        ISpan* span = (ISpan*)_span.implementation();
        if (global_tracer != nullptr)
        {
            _parentId = global_tracer->getCurrentParentId(span);
        }
    }
    return Php::Value(_parentId);
}

void Tracer::finishSpan(Php::Parameters& params)
{
    Php::Value param = params[0];
    if (!param.isNull())
    {
        if (!param.instanceOf("ISpan"))
            throw Php::Exception("Wrong parameter passed, should be ISpan");
        ISpan* span = (ISpan*)param.implementation();
        if (global_tracer != nullptr)
        {
            global_tracer->finishSpan(span);
        }

        // restore previous Tracer service name for process
        if (Tracer::single_ext_call && !userTracerSettings.empty())
        {
            initInternal(userTracerSettings["serviceName"], createCurlParamsList());
            Tracer::single_ext_call = false;
        }
    }
}

Php::Value Tracer::inject(Php::Parameters& params)
{
    const Php::Value& context = params[0];
    Php::Value carrier = params[1];

    std::ostringstream ss;
    {
        if (carrier.isString())
            ss << "Tracer::inject start  " << global_tracer << ", carrier = " << carrier << "\n";
        else if (carrier.isArray())
            ss << "Tracer::inject start  " << global_tracer << ", carrier = [array]" << "\n";
        else if (carrier.isNull())
            ss << "Tracer::inject start  " << global_tracer << ", carrier = [NULL]" << "\n";
    }

    if (!context.isNull())
    {
        if (global_tracer != nullptr)
        {
            global_tracer->inject(context, carrier);
        }

        {
            SpanContext* paramContext = nullptr;
            if (context.instanceOf("SpanContext"))
                paramContext = (SpanContext*)context.implementation();
            else if (context.instanceOf("ISpan"))
            {
                ISpan* span = (ISpan*)context.implementation();
                if (strcmp(span->_name(), "JaegerSpan") == 0)
                    paramContext = dynamic_cast<JaegerSpan*>(span)->_context;
                else if (strcmp(span->_name(), "NoopSpan") == 0)
                    paramContext = nullptr;
            }

            ss << "\t\t\t\t\t\t\t\t\t\tcontext (SpanContext): " << paramContext << "\n";
            if (paramContext != nullptr)
            {
                ss << "\t\t\t\t\t\t\t\t\t\t\t_traceIdHigh: " << paramContext->_traceIdHigh << "\n"
                    << "\t\t\t\t\t\t\t\t\t\t\t_traceIdLow: " << paramContext->_traceIdLow << "\n"
                    << "\t\t\t\t\t\t\t\t\t\t\t\t_spanId: " << paramContext->_spanId << "\n"
                    << "\t\t\t\t\t\t\t\t\t\t\t\t_parentId: " << paramContext->_parentId << "\n"
                    << "\t\t\t\t\t\t\t\t\t\t\t\t_flags: " << paramContext->_flags;
            }
            Tracer::file_logger.PrintLine(ss.str());
            ss.str("");
            ss.clear();
        }
    }

    {
        if (carrier.isString())
            ss << "Tracer::inject end    " << global_tracer << ", carrier = " << carrier;
        else if (carrier.isArray())
            ss << "Tracer::inject end    " << global_tracer << ", carrier = [array]" << "\n";
        else if (carrier.isNull())
            ss << "Tracer::inject end    " << global_tracer << ", carrier = [NULL]" << "\n";
        Tracer::file_logger.PrintLine(ss.str());
        ss.str("");
        ss.clear();

        if (carrier.isArray())
            for (auto& iter : carrier)
            {
                ss << iter.first.stringValue() << " " << iter.second.stringValue() << std::endl;
            }
        Tracer::file_logger.PrintLine(ss.str());
    }

    return carrier;
}

Php::Value Tracer::extract(Php::Parameters& params)
{
    const Php::Value& carrier = params[0];

#ifdef EXTENDED_DEBUG
    std::ostringstream ss;
    {
        if (carrier.isString())
            ss << "Tracer::extract start " << global_tracer << ", carrier = " << carrier;
        else
            ss << "Tracer::extract start " << global_tracer << ", carrier = [array]";
    }
#endif

    SpanContext* context = nullptr;
    if (global_tracer != nullptr)
    {
        context = global_tracer->extract(carrier);
    }

#ifdef EXTENDED_DEBUG
    {
        if (context != nullptr)
        {
            ss << "\n\t\t\t\t\t\t\t\t\t\t\treturned context: " << context
                << "\n\t\t\t\t\t\t\t\t\t\t\t\t_traceIdHigh: " << context->_traceIdHigh
                << "\n\t\t\t\t\t\t\t\t\t\t\t\t_traceIdLow: " << context->_traceIdLow
                << "\n\t\t\t\t\t\t\t\t\t\t\t\t_spanId: " << context->_spanId
                << "\n\t\t\t\t\t\t\t\t\t\t\t\t_parentId: " << context->_parentId
                << "\n\t\t\t\t\t\t\t\t\t\t\t\t_flags: " << context->_flags;
        }
        else
        {
            ss << "\n\t\t\t\t\t\t\t\t\t\treturned context: " << context << " probably was not created";
        }
        Tracer::file_logger.PrintLine(ss.str());
        ss.str("");
        ss.clear();
    }
#endif

#ifdef EXTENDED_DEBUG
    {
        if (carrier.isString())
            ss << "Tracer::extract end   " << global_tracer << ", carrier = " << carrier;
        else
            ss << "Tracer::extract end   " << global_tracer << ", carrier = [array]"
            << "\n";
        Tracer::file_logger.PrintLine(ss.str());
    }
#endif

    return context == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(context->_name(), context);
}

void Tracer::flush()
{
    if (global_tracer != nullptr)
    {
        global_tracer->flush();
    }
}

void Tracer::addTags(Php::Parameters& params)
{
    if (global_tracer != nullptr)
    {
        ISpan* span = global_tracer->getCurrentSpan();
        if (span != nullptr)
        {
            span->addTags(params);
        }
    }
}

void Tracer::addLogs(Php::Parameters& params)
{
    Tracer::file_logger.PrintLine("Tracer::addLogs", true);

    if (global_tracer != nullptr)
    {
        if (strcmp(global_tracer->_name(), "JaegerTracer") == 0)
        {
            ISpan* span = global_tracer->getCurrentSpan();
            if (span != nullptr)
            {
                span->addLogs(params);
            }
        }
    }
}

void Tracer::print(Php::Parameters& params)
{
    try
    {
        const std::string& str = params[0];
        Tracer::file_logger.PrintLine(str);
    }
    catch (...)
    {
    }
}

Php::Value Tracer::getTracer()
{
    {
        std::ostringstream ss;
        ss << "Tracer::getTracer addr: " << global_tracer;
        Tracer::file_logger.PrintLine(ss.str(), true);
    }

    return global_tracer == nullptr ? static_cast<Php::Value>(nullptr) : Php::Object(global_tracer->_name(), global_tracer);
}

Php::Value Tracer::startExternalTracing(Php::Parameters& params)
{
    std::ostringstream ss;
    ss << "Tracer::startExternalTracing" << std::endl;

    Php::Object newSpan;
    bool spanAdded = false;

    if (params[0].isArray() && params[0].count() >= 4)
    {
        if (!ini_settings_loaded)
        {
            loadIniSettings();
        }

        Php::Value request_data = params[0];

        std::string uri = request_data.get(0).stringValue();
        std::string caller = request_data.get(1).stringValue();
        std::string type = request_data.get(2).stringValue();
        unsigned fetchCount = request_data.get(3).numericValue();

        removeSchemeFromUri(uri);
        removeParamsQueryFromUri(uri);

        // check that external host is not in ignore list
        if (hostsFilterPassed(uri, not_instrumented_hosts))
        {
            Php::Value options;

            Php::Value span = getCurrentSpan();

            Tracer::single_ext_call = false;

            if (span.isNull())
            {
                ss << "Tracer::startExternalTracing no spans found, create new" << std::endl;
                initInternal("curl external", createCurlParamsList());
                Tracer::single_ext_call = true;
            }
            else
            {
                ss << "Tracer::startExternalTracing set current span as parent" << std::endl;
                options["childOf"] = span;
            }

            newSpan = static_cast<Php::Object>(startSpanInternal(getTrarelicSpanName(uri, fetchCount), options));

            // check that info tags required for external call span
            if (hostsFilterPassed(uri, empty_span_hosts))
            {
                newSpan.call("addTags", createCurlTagParamsList(uri, caller, type));
                ss << "Tracer::startExternalTracing create new span with http.uri = " + uri + ", caller = " + caller + ", type = " + type << std::endl;
            }

            spanAdded = true;
        }
    }
    else
    {
        ss << "Tracer::startExternalTracing incorrect input" << std::endl;
    }

    Tracer::file_logger.PrintLine(ss.str(), true);

    return spanAdded ? newSpan : static_cast<Php::Value>(nullptr);
}

Php::Value OpenTracing::createDefaultParamsList()
{
    Php::Value defaults;

    defaults["enabled"] = false;
    defaults["debug_output"] = false;
    defaults["udp_transport"] = Tracer::udp_transport;
    defaults["reporter"]["type"] = "udp";
    defaults["reporter"]["options"]["addr"] = "localhost";
    defaults["reporter"]["options"]["port"] = 6831;
    defaults["sampler"]["type"] = "percentage";
    defaults["sampler"]["options"]["percents"] = 1;

    return defaults;
}

Php::Value OpenTracing::createCurlParamsList()
{
    Php::Value defaults;

    defaults["enabled"] = true;
    defaults["mode"] = 0;
    defaults["debug_output"] = false;
    defaults["udp_transport"] = true;

    defaults["sampler"]["type"] = "percentage";
    defaults["sampler"]["options"]["percents"] = 100;
    if (!Tracer::userTracerSettings.empty())
    {
        defaults["reporter"]["type"] = Tracer::userTracerSettings["reporter_type"];
        defaults["reporter"]["options"]["addr"] = Tracer::userTracerSettings["reporter_addr"];
        defaults["reporter"]["options"]["port"] = Tracer::userTracerSettings["reporter_port"];
    }
    else
    {
        defaults["reporter"]["type"] = "udp";
        defaults["reporter"]["options"]["addr"] = "localhost";
        defaults["reporter"]["options"]["port"] = 6831;
    }

    return defaults;
}

Php::Value OpenTracing::createCurlTagParamsList(const std::string& uri, const std::string& caller, const std::string& type)
{
    Php::Value tagAttrs;
    tagAttrs["is_external"] = true;
    tagAttrs["http.uri"] = uri;
    tagAttrs["caller"] = caller;
    tagAttrs["type"] = type;

    return tagAttrs;
}

void OpenTracing::removeSchemeFromUri(std::string& uri)
{
    size_t pos;
    if ((pos = uri.find("://")) != std::string::npos)
    {
        uri.erase(0, pos + 3);
    }
}

void OpenTracing::removeParamsQueryFromUri(std::string& uri)
{
    size_t pos;
    if ((pos = uri.find("?")) != std::string::npos)
    {
        uri.erase(pos);
    }
}

bool OpenTracing::hostsFilterPassed(const std::string& uri, const std::vector<std::string>& filtered)
{
    for (auto host : filtered)
    {
        if (uri.find(host) == 0)
        {
            return false;
        }
    }
    return true;
}

std::vector<std::string> OpenTracing::parseHostsStr(std::string hosts)
{
    std::vector<std::string> hostsList{};
    if (!hosts.empty())
    {
        size_t dPos;
        // read hosts from semicolon separated ini string
        while ((dPos = hosts.find(';')) != std::string::npos)
        {
            hostsList.emplace_back(hosts.substr(0, dPos));
            hosts.erase(0, dPos + 1);
        }
        hostsList.emplace_back(hosts);
    }
    return hostsList;
}

void OpenTracing::loadIniSettings()
{
    Tracer::empty_span_hosts = parseHostsStr(Php::ini_get("trarelic.empty_spans_for_hosts").stringValue());
    Tracer::not_instrumented_hosts = parseHostsStr(Php::ini_get("trarelic.no_spans_for_hosts").stringValue());

    Tracer::ini_settings_loaded = true;
}

std::string OpenTracing::getTrarelicSpanName(std::string uri, unsigned fetchCount)
{
    size_t nameLen = 0;
    while (fetchCount && nameLen != std::string::npos)
    {
        nameLen = uri.find('/', nameLen + 1);
        fetchCount--;
    }

    std::string spanName = uri.substr(0, nameLen);

    size_t endHostPos = spanName.find('/');
    // replace first '/' with '-' if span name contains payment system info
    return (endHostPos == std::string::npos) || (endHostPos == spanName.length() - 1) ? spanName : spanName.replace(spanName.find('/'), 1, "-");
}
