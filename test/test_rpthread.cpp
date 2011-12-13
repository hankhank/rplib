#include "catch.hpp"
#include "rpthread.h"
#include "rpdebug.h"
using namespace rp;

#define COUNT_MAX 100
static int threadcount = 0;

class ThreadTester : public RpThread
{
public:
    void Run(){
        LOG(DEBUG) << "Thread has called run. Do making some addititions";
        for(; threadcount < COUNT_MAX; threadcount++)
        {
            LOG(DEBUG) << threadcount;
        }
    }
};

TEST_CASE("rpcore/rpthread/create", "Create, run and stop threaded class")
{
    ThreadTester tt;
    REQUIRE(threadcount == 0);
    tt.Go();
    tt.Stop();
    REQUIRE(threadcount == COUNT_MAX);
}
