#include <phpcpp.h>
#include "Tracer.h"
//#include "NoopTracer.h"
//#include "ChildMaster.h"
//#include "bubleSort.h"
//#include "print.h"
//#include "_test.h"
//#include "testClass.h"
//#include "ISpan.h"
#include "NoopSpan.h"
#include "NoopTracer.h"


#include "IReporter.h"
#include "UdpReporter.h"

/**
*  tell the compiler that the get_module is a pure C function
*/
extern "C" {

    /**
    *  Function that is called by PHP right after the PHP process
    *  has started, and that returns an address of an internal PHP
    *  strucure with all the details and features of your extension
    *
    *  @return void*   a pointer to an address that is understood by PHP
    */
    PHPCPP_EXPORT void *get_module()
    {
        /* Some globals init */
        GlobalInit();

        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("tracer-cpp", "1.0");

        Php::Class<Tracer> TracerClass("Tracer");
        TracerClass.method<&Tracer::getTracer>("getTracer", Php::Static, {});
        TracerClass.method<&Tracer::init>("init", Php::Static, {
            Php::ByVal("serviceName",Php::Type::String,true),
            Php::ByVal("settings",Php::Type::Array,false)
        });
        TracerClass.method<&Tracer::startSpan>("startSpan", Php::Static, {
            Php::ByVal("operationName",Php::Type::String,true),
            Php::ByVal("options",Php::Type::Array,false)
        });
        TracerClass.method<&Tracer::return_array>("return_array", Php::Static, {});

        extension.add(std::move(TracerClass));

        /*todo - try to leave only abstract interface*/
        //{
            //Php::Class<ITracer> ITracerClass("ITracer");
            //ITracerClass.method("print",{});
            //extension.add(std::move(ITracerClass));

            //Php::Class<NoopTracer> NoopTracerClass("NoopTracer");
            //NoopTracerClass.method<&NoopTracer::print>("print",{});
            //extension.add(NoopTracerClass);
        //}
        //Php::Interface ITracerInterface("ITracer");

        Php::Class<NoopTracer> NoopTracerClass("NoopTracer");
        NoopTracerClass.method<&NoopTracer::print>("print", {});
        //NoopTracerClass.method<&NoopTracer::init>("init",{});
        //NoopTracerClass.implements(ITracerInterface);
        extension.add(std::move(NoopTracerClass));

        Php::Class<JaegerTracer> JaegerTracerClass("JaegerTracer");
        JaegerTracerClass.method<&JaegerTracer::print>("print", {});
        extension.add(std::move(JaegerTracerClass));

        //extension.add(std::move(ITracerInterface));

        Php::Interface ISpanInterface("ISpan");
        ISpanInterface.method("addTag", {});

        Php::Class<NoopSpan> NoopSpanClass("NoopSpan");
        NoopSpanClass.implements(ISpanInterface);
        NoopSpanClass.method<&NoopSpan::addTag>("addTag", {});

        Php::Class<JaegerSpan> JaegerSpanClass("JaegerSpan");
        JaegerSpanClass.implements(ISpanInterface);
        JaegerSpanClass.method<&JaegerSpan::addTag>("addTag", {});

        extension.add(std::move(ISpanInterface));
        extension.add(std::move(JaegerSpanClass));
        extension.add(std::move(NoopSpanClass));






        //Php::Class<Master> master("master");
        //Php::Class<Child> child("child");
        //master.method<&Master::child>("child");
        //extension.add(std::move(master));
        //extension.add(std::move(child));




        // add the bubblesort function to the extension, we also tell the 
        // extension that the function receives one parameter by value, and
        // that that parameter must be an array
        /*
        //extension.add<native_bubblesort>("native_bubblesort");
        extension.add<native_bubblesort>("native_bubblesort", { Php::ByVal("input", Php::Type::Array) });
        ///("native_bubblesort", native_bubblesort, { Php::ByVal("input", Php::Type::Array) });

        extension.add<print_start>("print_start");
        extension.add<print_end>("print_end");
        extension.add<arrayTest>("arrayTest",{
        Php::ByVal("in1",Php::Type::String),
        Php::ByVal("in2",Php::Type::Array),
        });

        Php::Class<TestClass> testClass("TestClass");
        testClass.method<&TestClass::value>("value",{});
        testClass.method<&TestClass::increment>("increment",{
        Php::ByVal("change",Php::Type::Numeric,false)});
        testClass.method<&TestClass::decrement>("decrement",{
        Php::ByVal("change",Php::Type::Numeric,false)});
        //testClass.property("temp",50,Php::Public | Php::Static);
        extension.add(std::move(testClass));

        Php::Class<PublicClass> class1("PublicClass");
        class1.method<&PublicClass::method>("method",{});
        //class1.method<regularFunction>("static1",{});
        extension.add(std::move(class1));

        extension.add<regularFunction>("regularFunction");

        Php::Class<Child> _child("Child");
        Php::Class<Master> _master("Master");
        _master.method<&Master::child>("child",{});
        _master.method<&Master::ret_child>("ret_child",{});
        _master.method<&Master::__construct>("__construct");
        // the Example class has one public property
        //Php::Static
        //_master.property("ind", 3, Php::Static);
        extension.add(std::move(_child));
        extension.add(std::move(_master));

        Php::Class<test_static> class3("test_static");
        class3.method<&test_static::inc>("inc",{});
        //class3.method<&test_static::increment>("increment");
        class3.method<&test_static::inc2>("inc2",{});
        class3.method<&test_static::access>("access",{});
        extension.add(std::move(class3));

        //Child::_val = 11;
        */
        // return the extension
        return extension;
    }
}
