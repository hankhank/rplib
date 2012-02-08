#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <string>
#include "rpqueue.h"
#include "rpdebug.h"

using namespace rp;

/*
** Initialisation of Library
*/
class TestInitialiser
{   
public:
    TestInitialiser()
    { 
        InitLogging();
        LogStdoutSink* StdoutSink = new LogStdoutSink();
        AddLogSink(StdoutSink);
    };
    ~TestInitialiser()
    { 
        ShutdownLogging();
    };
};

TestInitialiser now;
