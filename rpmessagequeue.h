#ifndef RPMESSAGEQUEUE_H_
#define RPMESSAGEQUEUE_H_
#include "rpqueue.h"
#include "rpmessage.h"

namespace rp
{
    template <typename T, int QUEUE_SIZE>
    class RpMessageQueue : public RpQueue<RpMessage<T>, QUEUE_SIZE>
    {
    public:
        RpMessageQueue(){};
        RpMessageQueue(const RpMessageQueue&);
    };
};

#endif
