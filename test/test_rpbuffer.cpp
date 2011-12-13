#include "catch.hpp"
#include "rpbuffer.h"
#include "rpdebug.h"
using namespace rp;

#define BUFFER_SIZE 1000

TEST_CASE("rpcore/rpbuffer/create", "Create and single use")
{
    int buffer[BUFFER_SIZE];
    RpBuffer<int> rpb(buffer, BUFFER_SIZE);
    REQUIRE(rpb.Allocate() == buffer);
}

TEST_CASE("rpcore/rpbuffer/fillandfree", "Allocate entire buffer and free")
{
    int buffer[BUFFER_SIZE], i;
    int* bp[BUFFER_SIZE];
    RpBuffer<int> rpb(buffer, BUFFER_SIZE);
    REQUIRE(rpb.Allocate(bp, BUFFER_SIZE) == BUFFER_SIZE);
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        REQUIRE((&bp[i] < (bp+BUFFER_SIZE)));
        REQUIRE((&bp[i] >= bp));
    }
    rpb.Release(bp, BUFFER_SIZE);
}

TEST_CASE("rpcore/rpbuffer/overflow", "Allocate entire buffer +1 and free")
{
    int buffer[BUFFER_SIZE], i;
    int* bp[BUFFER_SIZE+1];
    RpBuffer<int> rpb(buffer, BUFFER_SIZE);
    REQUIRE(rpb.Allocate(bp, BUFFER_SIZE+1) == BUFFER_SIZE);
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        REQUIRE((&bp[i] < (bp+BUFFER_SIZE)));
        REQUIRE((&bp[i] >= bp));
    }
    rpb.Release(bp, BUFFER_SIZE);
}

TEST_CASE("rpcore/rpbuffer/available", "Check available on fill and free")
{
    int buffer[BUFFER_SIZE], i;
    int* bp[BUFFER_SIZE];
    RpBuffer<int> rpb(buffer, BUFFER_SIZE);

    /* Fill by large array */
    REQUIRE(rpb.Available() == BUFFER_SIZE);
    REQUIRE(rpb.Allocate(bp, BUFFER_SIZE) == BUFFER_SIZE);
    REQUIRE(rpb.Available() == 0);
    rpb.Release(bp, BUFFER_SIZE);
    REQUIRE(rpb.Available() == BUFFER_SIZE);
    
    /* Singles at a time */
    for( i = 0; i < BUFFER_SIZE; i++)
    {
        bp[i] = rpb.Allocate();
        REQUIRE(rpb.Available() == (BUFFER_SIZE - 1 - i));
    }
    for( i = 0; i < BUFFER_SIZE; i++)
    {
        rpb.Release(bp[i]);
        REQUIRE(rpb.Available() == (i + 1));
    }
}
