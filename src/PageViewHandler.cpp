#include "Tracer.h"
using namespace OpenTracing;

Printer Tracer::file_logger{ "/data/logs", false };
ITracer* Tracer::global_tracer{ nullptr };
int Tracer::header_flag{ 0 };
bool Tracer::ini_settings_loaded{ false };
bool Tracer::udp_transport{ true };
std::unordered_set<std::string> Tracer::extended_span_name_hosts{};

void onStartup()
{
    //Tracer::file_logger.PrintLine("onStartup");
}

void onRequest()
{
    //Tracer::file_logger.PrintLine("onRequest");
}

void onIdle()
{
    //Tracer::file_logger.PrintLine("onIdle");

    if (Tracer::global_tracer != nullptr)
    {
        std::ostringstream ss;
        ss << Tracer::global_tracer;
        Tracer::file_logger.PrintLine("delete global_tracer " + ss.str() + " in onIdle");
        delete Tracer::global_tracer;
        Tracer::global_tracer = nullptr;
    }
}

void onShutDown()
{
    //Tracer::file_logger.PrintLine("onShutDown");

    if (Tracer::global_tracer != nullptr)
    {
        std::ostringstream ss;
        ss << Tracer::global_tracer;
        Tracer::file_logger.PrintLine("delete global_tracer " + ss.str() + " in onShutDown");
        delete Tracer::global_tracer;
        Tracer::global_tracer = nullptr;
    }
}
