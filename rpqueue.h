#ifndef RPQUEUE_H_
#define RPQUEUE_H_

#include "rp.h"
#include "rpsemaphore.h"
#include "rpdebug.h"

namespace rp {

    /**
     * RpQueue is a fixed size thread safe blocking circular queue.
     */
    
    /* Template Definition */
    template <typename T, int QUEUE_SIZE>
    class RpQueue
    {
    public:
        RpQueue();
        ~RpQueue();
        void PushBack(T newobj);
        void PushFront(T newobj);
        T PopBack();
        T PopFront();
        T Front();
        T Back();

    private:
        void DecrWrap(unsigned int& num);
        void IncrWrap(unsigned int& num);
        RpSemaphore *empty_;
        RpSemaphore *lock_;
        RpSemaphore *usd_;
        unsigned int head_;
        unsigned int tail_;
        T rpq[QUEUE_SIZE];
    };

    /* Implementation */
    template <typename T, int QUEUE_SIZE>
    RpQueue<T, QUEUE_SIZE>::RpQueue()
    {
        empty_ =  new RpSemaphore(QUEUE_SIZE);
        lock_ =  new RpSemaphore(1);
        usd_ =  new RpSemaphore(0);
        head_ = 0;
        tail_ = 1;
    }  

    template <typename T, int QUEUE_SIZE>
    RpQueue<T, QUEUE_SIZE>::~RpQueue()
    {
        delete empty_;
        delete lock_;
        delete usd_;
    }

    template <typename T, int QUEUE_SIZE>
    void RpQueue<T, QUEUE_SIZE>::IncrWrap(unsigned int& num)
    {
        num += 1;
        if( num >= QUEUE_SIZE ) /* Wrap around */
        {
            num = 0;
        }
    }

    template <typename T, int QUEUE_SIZE>
    void RpQueue<T, QUEUE_SIZE>::DecrWrap(unsigned int& num)
    {
        if( num == 0 ) /* Wrap around */
        {
            num = QUEUE_SIZE - 1;
        }
        else
        {
            num -= 1;
        }
    }

    template <typename T, int QUEUE_SIZE>
    void RpQueue<T, QUEUE_SIZE>::PushBack(T newobj)
    {
        empty_->Wait();
        lock_->Wait();
        if( tail_ == head_ ) /* Move head if we are overflowing */
        {
            LOG(WARNING) << "Queue has overflowed";
            IncrWrap(head_);
        }
        else
        {
            usd_->Post(); /* Only post if we are within bounds */
        }
        rpq[tail_] = newobj;
        IncrWrap(tail_);
        lock_->Post();
    }

    template <typename T, int QUEUE_SIZE>
    void RpQueue<T, QUEUE_SIZE>::PushFront(T newobj)
    {
        empty_->Wait();
        lock_->Wait();
        if( head_ == tail_ ) /* Move tail if we are underflowing */
        {
            LOG(WARNING) << "Queue has underflowed";
            DecrWrap(tail_);
        }
        rpq[head_] = newobj;
        DecrWrap(head_);
        usd_->Post();
        lock_->Post();
    }

    template <typename T, int QUEUE_SIZE>
    T RpQueue<T, QUEUE_SIZE>::PopBack()
    {
        usd_->Wait();
        lock_->Wait();
        DecrWrap(tail_);
        if( tail_ == head_ )
        {
            LOG(WARNING) << "Queue empty. This should never happen";
            usd_->Post();
            lock_->Post();
            return rpq[0];
        }
        /**
         * Declaring ret here allows us to use copy-constructors for objects that
         * may not have default constructors declared.
         */
        T ret = rpq[tail_]; 
        empty_->Post();
        lock_->Post();
        return ret;
    }

    template <typename T, int QUEUE_SIZE>
    T RpQueue<T, QUEUE_SIZE>::PopFront()
    {
        usd_->Wait();
        lock_->Wait();
        IncrWrap(head_);
        if( head_ == tail_ )
        {
            LOG(WARNING) << "Queue empty. This should never happen";
            usd_->Post();
            lock_->Post();
            return rpq[0];
        }
        T ret = rpq[head_];
        empty_->Post();
        lock_->Post();
        return ret;
    }

    template <typename T, int QUEUE_SIZE>
    T RpQueue<T, QUEUE_SIZE>::Back()
    {
        usd_->Wait();
        lock_->Wait();
        DecrWrap(tail_);
        T ret = rpq[tail_];
        IncrWrap(tail_);
        usd_->Post();
        lock_->Post();
        return ret;
    }

    template <typename T, int QUEUE_SIZE>
    T RpQueue<T, QUEUE_SIZE>::Front()
    {
        usd_->Wait();
        lock_->Wait();
        IncrWrap(head_);
        T ret = rpq[head_];
        DecrWrap(head_);
        usd_->Post();
        lock_->Post();
        return ret;
    }
};
#endif
