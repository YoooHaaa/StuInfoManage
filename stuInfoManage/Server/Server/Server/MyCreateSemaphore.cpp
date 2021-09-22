#include "MyCreateSemaphore.h"


CMySemaphore::CMySemaphore(LONG lInitialCount,
    LONG lMaximumCount,
    LPCTSTR lpName,
    LPSECURITY_ATTRIBUTES lpSemaphoreAttributes)
{
    m_sem = ::CreateSemaphore(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
}



CMySemaphore::~CMySemaphore()
{
    ::CloseHandle(m_sem);
}


int CMySemaphore::WaitForSingleObject(DWORD dwMilliseconds)
{
    return ::WaitForSingleObject(m_sem, dwMilliseconds);
}

int CMySemaphore::ReleaseSemaphore(LONG lReleaseCount)
{

    return ::ReleaseSemaphore(m_sem, lReleaseCount, NULL);
}
