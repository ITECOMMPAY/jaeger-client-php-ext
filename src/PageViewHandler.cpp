#include "Tracer.h"
using namespace OpenTracing;

Printer Tracer::file_logger{ "/data/logs", false };
ITracer* Tracer::global_tracer{ nullptr };

void GlobalInit()
{

}

void onStartup()
{
    Tracer::file_logger.PrintLine("onStartup");
}

void onRequest()
{
    Tracer::file_logger.PrintLine("onRequest");
}

void onIdle()
{
    Tracer::file_logger.PrintLine("onIdle");
    if (Tracer::global_tracer != nullptr)
    {
        Tracer::file_logger.PrintLine("\t delete global_tracer in onIdle");
        delete Tracer::global_tracer;
        Tracer::global_tracer = nullptr;
    }
}

void onShutDown()
{
    Tracer::file_logger.PrintLine("onShutDown");

    if (Tracer::global_tracer != nullptr)
    {
        Tracer::file_logger.PrintLine("\t delete global_tracer in onShutDown");
        delete Tracer::global_tracer;
        Tracer::global_tracer = nullptr;
    }
}
