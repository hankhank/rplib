#include "catch.hpp"
#include "rpmap.h"
#include "rpdebug.h"

#include "string.h"
using namespace rp;

#define MAP_SIZE 1000

TEST_CASE("rpcore/rpmap/InsertAndRetrieve", "Add a value to map and retreive")
{
    int i = 0;
    /* Forwards insertion */
    { 
        RpMap<std::string, int, MAP_SIZE> rpm;
        char letter[] = "\0";
        
        for(i=0; i < 255; i++)
        {
            letter[0] += 1; /* performed first, can't init string with null */
            rpm.Insert(std::string(letter), i);
            REQUIRE(rpm[std::string(letter)] == i);
        }
        i -= 1;
        for(; i >= 0; --i)
        {
            REQUIRE(rpm[std::string(letter)] == i);
            letter[0] -= 1;
        }
        for(i=0; i < 255; i++)
        {
            letter[0] += 1; /* performed first, can't init string with null */
            REQUIRE(rpm[std::string(letter)] == i);
        }
    }
    /* Reverse insertion */
    { 
        RpMap<std::string, int, MAP_SIZE> rpm;
        char letter[] = "\0";
        letter[0] += 255;
        
        for(i=0; i < 255; i++)
        {
            letter[0] -= 1; 
            rpm.Insert(std::string(letter), i);
            REQUIRE(rpm[std::string(letter)] == i);
        }
        i -= 1;
        for(; i >= 0; --i)
        {
            letter[0] += 1;
            REQUIRE(rpm[std::string(letter)] == i);
        }
        for(i=0; i < 255; i++)
        {
            letter[0] -= 1; /* performed first, can't init string with null */
            REQUIRE(rpm[std::string(letter)] == i);
        }
    }
}

TEST_CASE("rpcore/rpmap/AssignAndRetrieve", 
    "Add a value to map using assignment and retreive")
{
    char letter[] = "\0";
    int i = 0;
    RpMap<std::string, int, MAP_SIZE> rpm;

    for(; i < 255; i++)
    {
        letter[0] += 1; /* performed first, can't init string with null */
        rpm[std::string(letter)] = i;
        REQUIRE(rpm[std::string(letter)] == i);
    }
    i -= 1;
    for(; i >= 0; --i)
    {
        REQUIRE(rpm[std::string(letter)] == i);
        letter[0] -= 1;
    }
    for(i=0; i < 255; i++)
    {
        letter[0] += 1; /* performed first, can't init string with null */
        REQUIRE(rpm[std::string(letter)] == i);
    }
}
