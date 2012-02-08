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
        std::string strcon;
        
        for(i=0; i < 255; i++)
        {
            letter[0] += 1; /* performed first, can't init string with null */
            strcon.assign(letter);
            rpm.Insert(strcon, i);
            REQUIRE(rpm[strcon] == i);
        }
        i -= 1;
        for(; i >= 0; --i)
        {
            REQUIRE(rpm[strcon] == i);
            letter[0] -= 1;
            strcon.assign(letter);
        }
        for(i=0; i < 255; i++)
        {
            letter[0] += 1; /* performed first, can't init string with null */
            strcon.assign(letter);
            REQUIRE(rpm[strcon] == i);
        }
    }
    /* Reverse insertion */
    { 
        RpMap<int, int, MAP_SIZE> rpm;
        
        for(i=999; i >= 0; i--)
        {
            rpm.Insert(i, i);
            REQUIRE((rpm[i] == i));
        }
        for(i = 0; i < 999; i++)
        {
            REQUIRE(rpm[i] == i);
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

TEST_CASE("rpcore/rpmap/keys", "Testing Keys function")
{
    RpMap<int, int, MAP_SIZE> rpm;
    int keysbuf[MAP_SIZE];

    /* Empty */
    REQUIRE(rpm.Keys(keysbuf, MAP_SIZE) == 0);
    
    /* Multiple entries */
    int i = 0;
    for(; i < MAP_SIZE; i++)
    {
        rpm[i] = i;
        REQUIRE(rpm.Keys(keysbuf, MAP_SIZE) == (i+1))
        REQUIRE(keysbuf[i] == i)
    }
    
    /* Request only 1 */
    REQUIRE(rpm.Keys(keysbuf, 1) == 1)

    /* Request Zero */
    REQUIRE(rpm.Keys(keysbuf, 0) == 0)

}

TEST_CASE("rpcore/rpmap/find", "Testing Find function")
{
    RpMap<int, int, MAP_SIZE> rpm;
    int i = 0;

    /* Empty */
    REQUIRE(rpm.Find(i) == -1);
    
    /* Multiple entries */
    for(; i < MAP_SIZE; i++)
    {
        rpm[i] = i;
        REQUIRE(rpm.Find(i) == (i))
    }

    /* Entry not in map */
    i = MAP_SIZE+1;
    REQUIRE(rpm.Find(i) == -1);

}

TEST_CASE("rpcore/rpmap/index", "Testing index function")
{
    RpMap<int, int, MAP_SIZE> rpm;
    int i = 0;

    /* Empty */
    rpm.Index(0);
    
    /* Multiple entries */
    for(; i < MAP_SIZE; i++)
    {
        rpm[i] = i;
        REQUIRE(rpm.Index(i) == (i))
    }

    /* Entry not in map */
    i = MAP_SIZE+1;
    rpm.Index(i);
}

TEST_CASE("rpcore/rpmap/erase", "Testing erase function")
{
    RpMap<int, int, MAP_SIZE> rpm;
    int i = 0;

    /* Empty */
    REQUIRE(rpm.Erase(i) == -1);
    
    /* Multiple entries */
    for(; i < MAP_SIZE; i++)
    {
        rpm[i] = i;
    }

    /* Entry not in map */
    i = MAP_SIZE+1;
    REQUIRE(rpm.Erase(i) == -1);

    for(i=0; i < MAP_SIZE; i++)
    {
        REQUIRE(rpm.Erase(i) == 0 );
    }

}

TEST_CASE("rpcore/rpmap/replacing", "Testing replace map function")
{
    RpMap<int, int, MAP_SIZE> rpm;
    int i = 0;

    /* Multiple entries */
    for(; i < MAP_SIZE; i++)
    {
        rpm[i] = i;
        REQUIRE(rpm[i] == i)
    }
    
    /* Replace all with -1 */
    for(; i < MAP_SIZE; i++)
    {
        rpm[i] = -1;
        REQUIRE(rpm[i] == -1)
    }
}

