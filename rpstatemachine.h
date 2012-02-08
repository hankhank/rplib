#ifndef RPSTATEMACHINE_H_
#define RPSTATEMACHINE_H_

#include "rp.h"
#include "rpdebug.h"

namespace rp 
{
    /**
     * @brief   Basic state machine class that can be inherited to enable different
     *          event handling functions accept the incoming events depending upon
     *          what state the class is in. Events in this statemachine are limited
     *          to the RpMessage class.
     * 
     * @tparam  T   The event type that will be passed to state machine and  
     *              subsequently to state handlers
     */
    template <typename T>
    class RpStateMachine
    {
    public:
        typedef void (RpStateMachine::* StateHandler)(T);
        RpStateMachine(StateHandler statefn);
        ~RpStateMachine();
        
        void HandleEvent(T event);
        void Trans(StateHandler statefn);
    private:
        StateHandler curStateHandler_;

    };

    /**
     * @brief Init statemachine to some state
     *
     * @param   statefn First state function
     */
    template <typename T>
    RpStateMachine<T>::RpStateMachine(StateHandler statefn) : 
        curStateHandler_(statefn)
    {
    }

    /**
     * @brief 
     *
     * @param
     */
    template <typename T>
    RpStateMachine<T>::~RpStateMachine()
    {
    }

    /**
     * @brief   Generic event handler for the statemachine which then pushes the 
     *          messages to the current state handler function.
     *
     * @param   event   Incoming event to be handled
     */
    template <typename T>
    void RpStateMachine<T>::HandleEvent(T event)
    {
        (this->*curStateHandler_)(event);
    }

    /**
     * @brief   Handles the transistion between different states. It accepts the next
     *          state function to be piped the incoming events to. Functions must not
     *          return anything and accept a single RpMessage argument.
     *
     * @param   statefn Next function to handle the incoming events.
     */
    template <typename T>
    void RpStateMachine<T>::Trans(StateHandler statefn)
    {
        curStateHandler_ = statefn;
    }

};

#endif
