#include "rp.h"
#include "rpthread.h"
#include "rpsemaphore.h"

using namespace rp;

RpThread::RpThread(): 
    running_(false)
{
    runlock_ = new RpSemaphore(1);
}

void RpThread::Stop()
{
    runlock_->Wait(); /* Make access running thread safe */
    if(running_ == true)
    {
         running_ = false;
         pthread_join(thread_,0);
    }
    runlock_->Post();
}   

void RpThread::Go()
{
    runlock_->Wait(); /* Make access running thread safe */
    if(running_ == false)
    {
        running_ = true;
        pthread_create(&thread_, 0, (&RpThread::StartThread), this);
    }
    runlock_->Post();
}

void *RpThread::StartThread(void *obj)
{
    reinterpret_cast<RpThread *>(obj)->Run();
    return NULL;
}
