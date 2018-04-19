// Thread definition
// Jianqiang Wei
//////////////////////////////////////////////////////////////////////////

#ifndef _THREADPOSIX_H_
#define _THREADPOSIX_H_

#include <sys/syscall.h>
#include <pthread.h>

#include "typedefs_audwrapper.h"

#define RTIP_THREAD_RR

class CriticalSectionPosix;
class EventPosix;

// Object that will be passed by the spawned thread when it enters the callback
// function.
#define ThreadObj void*
// Callback function that the spawned thread will enter once spawned.
// A return value of false is interpreted as that the function has no
// more work to do and that the thread can be released.
typedef bool (*ThreadRunFunction)(ThreadObj);

enum ThreadPriority
{
    kLowPriority = 1,
    kNormalPriority = 2,
    kHighPriority = 3,
    kHighestPriority = 4,
    kRealtimePriority = 5
};

enum {kThreadMaxNameLength = 64};

class ThreadPosix
{
public:
    static ThreadPosix* Create(ThreadRunFunction func = 0, ThreadObj obj = 0,
                             ThreadPriority prio = kNormalPriority, const char* threadName = 0);

    ThreadPosix(ThreadRunFunction func, ThreadObj obj, ThreadPriority prio,
              const char* threadName);

    virtual ~ThreadPosix();

    void SetNotAlive();
    bool Start(Rtip_UWord32& id);
    bool Stop();

    void Run();

#ifdef AUD_ALG_BIND_CPU0
	// set which cpu id to bind
	void setCpuAffinity(int cpu_id) 
	{
		m_isBind = true;
		m_cpu_id = cpu_id;
	}
#endif

private:
    Rtip_Word32 Construct();

private:
    // processing function
    ThreadRunFunction   m_runFunction;
    ThreadObj           m_obj;

    // internal state
    CriticalSectionPosix* m_pCS;  // Protects _alive and _dead
    bool m_alive;
    bool m_dead;
    ThreadPriority m_prio;
    EventPosix* m_pEvent;

#ifdef AUD_ALG_BIND_CPU0
	// bind with specified CPU ?
	bool m_isBind;
	int m_cpu_id;
#endif
	
    // zero-terminated thread name string
    char m_name[kThreadMaxNameLength];
    bool m_setThreadName;

    // handle to thread
    pthread_attr_t m_attr;
    pthread_t m_thread;
};

#endif // _THREADPOSIX_H_
