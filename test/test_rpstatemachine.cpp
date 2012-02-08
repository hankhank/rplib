#include "catch.hpp"
#include "rpstatemachine.h"
#include "rpmessage.h"
#include "rpdebug.h"
using namespace rp;

typedef enum 
{
    EVENT_1,
    EVENT_2,
    EVENT_3,
    EVENT_4,
    NEXT
} TestEvents;

class TestMachine : public RpStateMachine<RpMessage<TestEvents>*>
{
public:
    TestMachine() : 
        RpStateMachine<RpMessage<TestEvents>*>((StateHandler)&TestMachine::function1), 
        state_(1), event_(EVENT_1) {};
    void function1(RpMessage<TestEvents> *msg)
    {
        if( msg->GetType() == NEXT )
        {
            Trans((StateHandler)&TestMachine::function2);
            state_ = 2;
        }
        else
        {
            event_ = msg->GetType();
        }
    };
    void function2(RpMessage<TestEvents> *msg)
    {
        if( msg->GetType() == NEXT )
        {
            Trans((StateHandler)&TestMachine::function3);
            state_ = 3;
        }
        else
        {
            event_ = msg->GetType();
        }
    };

    void function3(RpMessage<TestEvents> *msg)
    {
        if( msg->GetType() == NEXT )
        {
            Trans((StateHandler)&TestMachine::function4);
            state_ = 4;
        }
        else
        {
            event_ = msg->GetType();
        }
    };

    void function4(RpMessage<TestEvents> *msg)
    {
        if( msg->GetType() == NEXT )
        {
            Trans((StateHandler)&TestMachine::function1);
            state_ = 1;
        }
        else
        {
            event_ = msg->GetType();
        }
    };

    int state_;
    TestEvents event_;
};

TEST_CASE("rpcore/rpstatemachine/basic", "Basic usage")
{
    int state = 1;
    int times = 0;
    TestMachine tm;
    RpMessage<TestEvents> event1(EVENT_1);
    RpMessage<TestEvents> event2(EVENT_2);
    RpMessage<TestEvents> event3(EVENT_3);
    RpMessage<TestEvents> event4(EVENT_4);
    RpMessage<TestEvents> next(NEXT);
    for(times = 0; times < 2; times++)
    {
        for(state = 1; state < 5; state++)
        {
            tm.HandleEvent(&event1);
            REQUIRE(tm.state_ == state);
            REQUIRE(tm.event_ == EVENT_1);

            tm.HandleEvent(&event2);
            REQUIRE(tm.state_ == state);
            REQUIRE(tm.event_ == EVENT_2);

            tm.HandleEvent(&event3);
            REQUIRE(tm.state_ == state);
            REQUIRE(tm.event_ == EVENT_3);

            tm.HandleEvent(&event4);
            REQUIRE(tm.state_ == state);
            REQUIRE(tm.event_ == EVENT_4);

            tm.HandleEvent(&next);
        }
    }
}
