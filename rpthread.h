#ifndef RPTHREAD_H_
#define RPTHREAD_H_

#include "pthread.h"

namespace rp 
{
    class RpSemaphore;

    class RpThread
    {
    public:
        RpThread();
        ~RpThread();

        void Go(); /* Start the thread */
        void Stop(); /* Stop the thread */

    protected:
        virtual void Run() = 0; /* User thread entry function here */

    private:
        static void* StartThread(void *obj); /* Kicks off thread and calls Users Run */

        bool running_;
        pthread_t thread_;
        RpSemaphore* runlock_;
    };
};

#endif 
