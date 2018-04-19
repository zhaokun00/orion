// Critical section definition
// Jianqiang Wei
//////////////////////////////////////////////////////////////////////////

#ifndef _CRITICALSECTIONPOSIX_H_
#define _CRITICALSECTIONPOSIX_H_

#include <pthread.h>

class CriticalSectionPosix
{
public:
    static CriticalSectionPosix* Create();

    virtual ~CriticalSectionPosix();

    void Enter();
    void Leave();

private:
    CriticalSectionPosix();

private:
    pthread_mutex_t m_mutex;
};

// Extension of the critical section. Prevents Enter/Leave mismatches and
// provides more compact critical section syntax.
class CriticalSectionScoped
{
public:
    explicit CriticalSectionScoped(CriticalSectionPosix& critsec)
        : m_ptrCritSec(&critsec)
    {
        m_ptrCritSec->Enter();
    }

    explicit CriticalSectionScoped(CriticalSectionPosix* critsec)
        : m_ptrCritSec(critsec)
    {
      m_ptrCritSec->Enter();
    }

    ~CriticalSectionScoped()
    {
        if (m_ptrCritSec)
        {
            Leave();
        }
    }

private:
    void Leave()
    {
        m_ptrCritSec->Leave();
        m_ptrCritSec = 0;
    }

    CriticalSectionPosix* m_ptrCritSec;
};

#endif // _CRITICALSECTIONPOSIX_H_
