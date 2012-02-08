#include "catch.hpp"
#include "rpdynamic.h"
#include "rpdebug.h"
#include "rpdynamictestlib.h"

using namespace rp;

TEST_CASE("rpcore/rpdynamic/basicload", "Basic loading")
{
    RpDynamic rptestlibs;
    std::string loadedlibs[20];
    REQUIRE(rptestlibs.Register(std::string("./test/librpdynamictestlib.so")) == 
        RpDynamic::NOERR)
    RpTestLibInterface *testclass 
        = static_cast<RpTestLibInterface*>(rptestlibs["RpTestLibv1.0"]);
    testclass->Test();
    REQUIRE(rptestlibs.ListLoaded(loadedlibs, 20) == 1);
    REQUIRE(loadedlibs[0] == VERSIONSTRING)
    delete testclass;
}

