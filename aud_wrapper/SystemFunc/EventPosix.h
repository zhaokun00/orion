// Event definition
// Jianqiang Wei
//////////////////////////////////////////////////////////////////////////

#ifndef _EVENTPOSIX_H_
#define _EVENTPOSIX_H_

#include <pthread.h>
#include <time.h>
#include "typedefs_audwrapper.h"
#include <semaphore.h>

#define USE_SEMAPHORE

#define RTIP_CLOCK_TYPE_REALTIME

#define RTIP_EVENT_INFINITE 0xffffffff
#define RTIP_EVENT_10_SEC 10000

enum State
{
    kUp = 1,
    kDown = 2
};

enum EventTypePosix
{
    kEventSignaled = 1,
    kEventError = 2,
    kEventTimeout = 3
};

class EventPosix
{
public:
    static EventPosix* Create();

    virtual ~EventPosix();

    EventTypePosix Wait(Rtip_UWord32 maxTime);
    bool Set();
    bool Reset();

private:
    EventPosix();
    Rtip_Word32 Construct();

private:

#ifdef USE_SEMAPHORE
	sem_t m_sem;
#else
    pthread_cond_t  m_cond;
    pthread_mutex_t m_mutex;
#endif

    State m_state;
};

#endif // _EVENTPOSIX_H_
