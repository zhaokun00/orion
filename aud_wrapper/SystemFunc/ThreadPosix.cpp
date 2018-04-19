// Thread definition
// Jianqiang Wei
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/prctl.h>

#include "CriticalSectionPosix.h"
#include "EventPosix.h"
#include "ThreadPosix.h"
#include "RTInterphoneLog.h"

extern "C"
{
    static void* StartThread(void* lpParameter)
    {
        static_cast<ThreadPosix*>(lpParameter)->Run();

        return 0;
    }
}

ThreadPosix* ThreadPosix::Create(ThreadRunFunction func, ThreadObj obj,
                                 ThreadPriority prio, const char* threadName)
{
	ThreadPosix* ptr = new ThreadPosix(func, obj, prio, threadName);
    if (!ptr)
    {
        return NULL;
    }

    const Rtip_Word32 error = ptr->Construct();
    if (error)
    {
        delete ptr;

        return NULL;
    }

    return ptr;
}

ThreadPosix::ThreadPosix(ThreadRunFunction func, ThreadObj obj,
                         ThreadPriority prio, const char* threadName)
    : m_runFunction(func),
      m_obj(obj),
      m_pCS(CriticalSectionPosix::Create()),
      m_alive(false),
      m_dead(true),
      m_prio(prio),
      m_pEvent(EventPosix::Create()),
      m_name(),
      m_setThreadName(false),
      m_attr(),
      m_thread(0)
#ifdef AUD_ALG_BIND_CPU0      
      , m_isBind(false),
      m_cpu_id(-1)
#endif      
{
    if (threadName != NULL)
    {
        m_setThreadName = true;
        strncpy(m_name, threadName, kThreadMaxNameLength);
        m_name[kThreadMaxNameLength - 1] = '\0';
    }
}

Rtip_Word32 ThreadPosix::Construct()
{
	Rtip_Word32 result = 0;

    result = pthread_attr_init(&m_attr);
    if (result != 0)
    {
        return -1;
    }

    return 0;
}

ThreadPosix::~ThreadPosix()
{
    pthread_attr_destroy(&m_attr);
    delete m_pEvent;
    delete m_pCS;
}

bool ThreadPosix::Start(Rtip_UWord32& id)
{
    if (!m_runFunction)
    {
        return false;
    }

    Rtip_Word32 result = pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED);
    // Set the stack size to 1M.
    result |= pthread_attr_setstacksize(&m_attr, 1024*1024);

#ifdef RTIP_THREAD_RR
    const Rtip_Word32 policy = SCHED_RR;
#else
    const Rtip_Word32 policy = SCHED_FIFO;
#endif

    m_pEvent->Reset();

    result |= pthread_create(&m_thread, &m_attr, &StartThread, this);
    if (result != 0)
    {
        return false;
    }

    // Wait up to 10 seconds for the OS to call the callback function. Prevents
    // race condition if Stop() is called too quickly after start.
    if (kEventSignaled != m_pEvent->Wait(RTIP_EVENT_10_SEC))
    {
        // Timed out. Something went wrong.
        m_runFunction = NULL;
        return false;
    }

    id = static_cast<Rtip_UWord32>(m_thread);

    sched_param param;

    const Rtip_Word32 minPrio = sched_get_priority_min(policy);
    const Rtip_Word32 maxPrio = sched_get_priority_max(policy);
    if ((minPrio == EINVAL) || (maxPrio == EINVAL))
    {
        return false;
    }

    switch (m_prio)
    {
    case kLowPriority:
        param.sched_priority = minPrio + 1;
        break;
    case kNormalPriority:
        param.sched_priority = (minPrio + maxPrio) / 2;
        break;
    case kHighPriority:
        param.sched_priority = maxPrio - 3;
        break;
    case kHighestPriority:
        param.sched_priority = maxPrio - 2;
        break;
    case kRealtimePriority:
        param.sched_priority = maxPrio - 1;
        break;
    }
    result = pthread_setschedparam(m_thread, policy, &param);
    if (result == EINVAL)
    {
        return false;
    }

    return true;
}

void ThreadPosix::SetNotAlive()
{
    CriticalSectionScoped cs(m_pCS);
    m_alive = false;
}

bool ThreadPosix::Stop()
{
    bool dead = false;
    {
        CriticalSectionScoped cs(m_pCS);
        m_alive = false;
        dead = m_dead;
    }

    // Wait up to 10 seconds for the thread to terminate
    for (Rtip_Word32 i = 0; i < 1000 && !dead; i++)
    {
        timespec t;
        t.tv_sec = 0;
        t.tv_nsec = 10*1000*1000;
        nanosleep(&t, NULL);
        {
            CriticalSectionScoped cs(m_pCS);
            dead = m_dead;
        }
    }

    if (dead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ThreadPosix::Run()
{
    {
        CriticalSectionScoped cs(m_pCS);
        m_alive = true;
        m_dead  = false;
    }

    // The event the Start() is waiting for.
    m_pEvent->Set();

    if (m_setThreadName)
    {
        // LOGI("[%s : %s] Thread with name (%s) started.", __FILE__, __FUNCTION__, m_name);
        printf("[%s : %s] Thread with name (%s) started.\n", __FILE__, __FUNCTION__, m_name);
        prctl(PR_SET_NAME, m_name);
    }
    else
    {
    	// LOGI("[%s : %s] Thread without name started.", __FILE__, __FUNCTION__);
        printf("[%s : %s] Thread without name started.\n", __FILE__, __FUNCTION__);
    }

#ifdef AUD_ALG_BIND_CPU0
    // set pthread affinity as required
    if (m_isBind)
    {
        cpu_set_t mask;

        CPU_ZERO(&mask);
        CPU_SET(m_cpu_id, &mask);
        
        if (syscall(__NR_sched_setaffinity, gettid(), sizeof (mask), &mask) !=  0)
        {
            LOGD(" XXXXXXXXXXXXXXXXXXXXXXXXXX - failed to bind cpu.\n");
            // printf(" XXXXXXXXXXXXXXXXXXXXXXXXXX - failed to bind cpu.\n");
        }
    }
#endif

    bool alive = true;
    do
    {
        if (m_runFunction)
        {
            if (!m_runFunction(m_obj))
            {
                alive = false;
            }
        }
        else
        {
            alive = false;
        }
        {
            CriticalSectionScoped cs(m_pCS);
            if (!alive)
            {
              m_alive = false;
            }
            alive = m_alive;
        }
    }
    while (alive);

    if (m_setThreadName)
    {
        // LOGI("[%s : %s] Thread with name (%s) stopped.", __FILE__, __FUNCTION__, m_name);
        printf("[%s : %s] Thread with name (%s) stopped. \n", __FILE__, __FUNCTION__, m_name);
    }
    else
    {
    	// LOGI("[%s : %s] Thread without name stopped.", __FILE__, __FUNCTION__);
    	printf("[%s : %s] Thread without name stopped. \n", __FILE__, __FUNCTION__);
    }
    {
        CriticalSectionScoped cs(m_pCS);
        m_dead = true;
    }
}
