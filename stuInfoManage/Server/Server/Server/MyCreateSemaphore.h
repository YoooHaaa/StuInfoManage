#pragma once
#include <windows.h>



class CMySemaphore
{
public:
    CMySemaphore(
        LONG lInitialCount = 0,
        LONG lMaximumCount = 1,
        LPCTSTR lpName = NULL,
        LPSECURITY_ATTRIBUTES lpSemaphoreAttributes = NULL);
    ~CMySemaphore();
    int WaitForSingleObject(DWORD dwMilliseconds);
    int ReleaseSemaphore(LONG lReleaseCount);
private:
    HANDLE m_sem;
};

