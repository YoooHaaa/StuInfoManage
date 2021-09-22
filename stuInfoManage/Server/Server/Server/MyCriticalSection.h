#pragma once
#include <windows.h>



class CMyCriticalSection
{
public:
    CMyCriticalSection();
    ~CMyCriticalSection();
    int lock();
    int unlock();
private:
    CRITICAL_SECTION m_cs;
};

