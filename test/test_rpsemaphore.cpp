#include "catch.hpp"
#include "rpsemaphore.h"
#include "rpdebug.h"

using namespace rp;

#define SEMAPHORE_SIZE 1000

TEST_CASE("rpcore/rpsemaphore/trywait", "Initialise with value and trywait")
{
    RpSemaphore sema(0);
    int i = 0;

    for(; i < SEMAPHORE_SIZE-1; i++)
    {
        REQUIRE(sema.GetValue() == i);
        sema.Post();
    }
    i -= 1;
    for(; i >= 0; --i)
    {
        REQUIRE(sema.TryWait() == true);
        REQUIRE(sema.GetValue() == i);
    }
    REQUIRE(sema.TryWait() == false);
}
