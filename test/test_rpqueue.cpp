#include "catch.hpp"
#include "rpqueue.h"
#include "rpdebug.h"
using namespace rp;

#define QUEUE_SIZE 1000

TEST_CASE("rpcore/rpqueue/PushBackPopFront", "Fill PushBack Empty PopFront")
{
    RpQueue<int*, QUEUE_SIZE> rpq;
    int array[QUEUE_SIZE] = {0};

    int i = 0;
    for(; i < QUEUE_SIZE-1; i++)
    {
        array[i] = i;
        rpq.PushBack(&array[i]);
    }
    
    for( i = 0; i < QUEUE_SIZE-1; i++)
    {
        REQUIRE((rpq.PopFront()) == &array[i]);
    }

}

TEST_CASE("rpcore/rpqueue/PushFrontPopBack", "Fill PushFront Empty PopBack")
{
    RpQueue<int*, QUEUE_SIZE> rpq;
    int array[QUEUE_SIZE] = {0};

    int i = 0;
    for(; i < QUEUE_SIZE-1; i++)
    {
        array[i] = i;
        rpq.PushFront(&array[i]);
    }
    
    for( i = 0; i < QUEUE_SIZE-1; i++)
    {
        REQUIRE((rpq.PopBack()) == &array[i]);
    }

}

TEST_CASE("rpcore/rpqueue/PushFrontPopFront", "Fill PushFront Empty PopFront")
{
    RpQueue<int*, QUEUE_SIZE> rpq;
    int array[QUEUE_SIZE] = {0};

    int i = 0;
    for(; i < QUEUE_SIZE-1; i++)
    {
        array[i] = i;
        rpq.PushFront(&array[i]);
    }
    i -= 1;
    for(; i >= 0; --i)
    {
        REQUIRE((rpq.PopFront()) == &array[i]);
    }

}

TEST_CASE("rpcore/rpqueue/PushBackPopBack", "Fill PushBack Empty PopBack")
{
    RpQueue<int*, QUEUE_SIZE> rpq;
    int array[QUEUE_SIZE] = {0};

    int i = 0;
    for(; i < QUEUE_SIZE-1; i++)
    {
        array[i] = i;
        rpq.PushBack(&array[i]);
    }
    i -= 1;
    for(; i >= 0; --i)
    {
        REQUIRE((rpq.PopBack()) == &array[i]);
    }

}

TEST_CASE("rpcore/rpqueue/front", "Check front() works")
{
    RpQueue<int*, QUEUE_SIZE> rpq;
    int array[QUEUE_SIZE] = {0};

    int i = 0;
    for(; i < QUEUE_SIZE-1; i++)
    {
        array[i] = i;
        rpq.PushFront(&array[i]);
        REQUIRE((rpq.Front()) == &array[i]);
    }
    i -= 1;
    for(; i >= 0; --i)
    {
        REQUIRE((rpq.Front()) == &array[i]);
        rpq.PopFront();
    }

}

TEST_CASE("rpcore/rpqueue/Back", "Check Back() works")
{
    RpQueue<int*, QUEUE_SIZE> rpq;
    int array[QUEUE_SIZE] = {0};

    int i = 0;
    for(; i < QUEUE_SIZE-1; i++)
    {
        array[i] = i;
        rpq.PushBack(&array[i]);
        REQUIRE((rpq.Back()) == &array[i]);
    }
    i -= 1;
    for(; i >= 0; --i)
    {
        REQUIRE((rpq.Back()) == &array[i]);
        rpq.PopBack();
    }

}
