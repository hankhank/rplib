
#include "rpsemaphore.h"
#include "rpdebug.h"
#include "string.h"
#include <errno.h>
#include <time.h>
#ifdef OS_MACOSX
#define NAMED_SEMAPHORE
#include <unistd.h>
#include "stdio.h"
#endif
using namespace rp;

RpSemaphore::RpSemaphore(int value)
    : value_(value)
{
#ifdef NAMED_SEMAPHORE
    char name[32];
    long now;
    now = (long) time(0);
    srand(clock());
    snprintf(name, 32, "/%s-%04d-%10ld", "rpsema", rand(), now);
    semaphore_ = sem_open(name, O_CREAT, 0, value);
    if( semaphore_ == SEM_FAILED)
    {
      printf("SEMAPHORE FAILED TO BE CREATED err %s\n", strerror(errno));
    }
    snprintf(name, 32, "/%s-%04d-%10ld", "value", rand(), now);
    valsema_ = sem_open(name, O_CREAT, 0, 1);
    if( semaphore_ == SEM_FAILED)
    {
      printf("SEMAPHORE VALUE FAILED TO BE CREATED err %d\n",errno);
    }
#else
    semaphore_ = new sem_t;
    sem_init(semaphore_, 0, value_);
    valsema_ = new sem_t;
    sem_init(valsema_, 0, 1);
#endif
}

RpSemaphore::~RpSemaphore()
{
#ifdef NAMED_SEMAPHORE
    sem_close(semaphore_);
    sem_close(valsema_);
#else
    sem_destroy(semaphore_);
    sem_destroy(valsema_);
    delete semaphore_;
    delete valsema_;
#endif
}

bool RpSemaphore::TryWait()
{
    if(sem_trywait(semaphore_) == 0)
    {
        sem_wait(valsema_);
        value_ -= 1;
        sem_post(valsema_);
        return true;
    }
    return false;
}

bool RpSemaphore::TimedWait(int milliseconds)
{
    struct timespec timeout;
    time_t now = time(0);
    timeout.tv_sec = now + milliseconds/1000;
    timeout.tv_nsec = (milliseconds%1000)*1000000;
    if(sem_timedwait(semaphore_, &timeout) == 0)
    {
        sem_wait(valsema_);
        value_ -= 1;
        sem_post(valsema_);
        return true;
    }
    return false;
}

void RpSemaphore::Wait()
{
    sem_wait(semaphore_);
    sem_wait(valsema_);
    value_ -= 1;
    sem_post(valsema_);
}

void RpSemaphore::Post()
{
    sem_post(semaphore_);
    sem_wait(valsema_);
    value_ += 1;
    sem_post(valsema_);
}

int RpSemaphore::GetValue()
{
    int ret; 
    sem_wait(valsema_);
    ret = value_;
    sem_post(valsema_);
    return ret;
}
