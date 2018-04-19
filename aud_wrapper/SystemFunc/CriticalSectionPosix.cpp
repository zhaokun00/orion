// Critical section definition
// Jianqiang Wei
//////////////////////////////////////////////////////////////////////////

#include "CriticalSectionPosix.h"

CriticalSectionPosix* CriticalSectionPosix::Create()
{
	CriticalSectionPosix* ptr = new CriticalSectionPosix;
    if (!ptr)
    {
        return NULL;
    }

    return ptr;
}

CriticalSectionPosix::CriticalSectionPosix()
{
    pthread_mutexattr_t attr;
    (void) pthread_mutexattr_init(&attr);
    (void) pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    (void) pthread_mutex_init(&m_mutex, &attr);
}

CriticalSectionPosix::~CriticalSectionPosix()
{
    (void) pthread_mutex_destroy(&m_mutex);
}

void
CriticalSectionPosix::Enter()
{
    (void) pthread_mutex_lock(&m_mutex);
}

void
CriticalSectionPosix::Leave()
{
    (void) pthread_mutex_unlock(&m_mutex);
}
