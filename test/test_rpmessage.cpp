#include "catch.hpp"
#include "rpmessage.h"
using namespace rp;

typedef enum 
{
    MSGTYPE_1 = 1,
    MSGTYPE_2 = 2,
    MSGTYPE_3 = 3,
    MSGTYPE_4 = 4
} msgtypes;

#define ARRAY_SIZE 10

TEST_CASE("rpcore/rpmessage/create", "Create and single use")
{
    int i[ARRAY_SIZE];
    for( int jj = 0; jj < ARRAY_SIZE; jj++ ) i[jj] = 0;

    /* RpMessage Test */
    RpMessage<msgtypes> rpm(MSGTYPE_1);
    REQUIRE(rpm.GetType() == MSGTYPE_1);

    /* RpMessageContainer */
    RpMessageContainer<msgtypes, int*> rpmc(MSGTYPE_1, &i[0]);
    REQUIRE(rpmc.GetPayload() == &i[0]);
}

TEST_CASE("rpcore/rpmessage/operators", "Test the various operators")
{
    /* RpMessage Test */
    RpMessage<msgtypes> rpm1(MSGTYPE_1);
    RpMessage<msgtypes> rpm2(MSGTYPE_2);
    RpMessage<msgtypes> rpm3(MSGTYPE_3);
    RpMessage<msgtypes> temp(rpm1);

    REQUIRE(rpm1 == rpm1);
    REQUIRE(rpm1 == temp);
    REQUIRE(!(rpm1 != temp));
    REQUIRE(rpm1 != rpm2);
    REQUIRE(rpm1 != rpm3);
    REQUIRE(!(rpm1 == rpm3));
    
    temp = rpm2;
    REQUIRE(rpm2 == rpm2);
    REQUIRE(rpm2 == temp);
    REQUIRE(!(rpm2 != temp));
    REQUIRE(rpm2 != rpm1);
    REQUIRE(!(rpm2 == rpm3));

}
