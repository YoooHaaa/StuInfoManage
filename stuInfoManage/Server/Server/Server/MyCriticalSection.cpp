#include "MyCriticalSection.h"


CMyCriticalSection::CMyCriticalSection()
{
    ::InitializeCriticalSection(&m_cs);
}


CMyCriticalSection::~CMyCriticalSection()
{
    ::DeleteCriticalSection(&m_cs);
}

int CMyCriticalSection::lock()
{
    ::EnterCriticalSection(&m_cs);
    return 0;
}

int CMyCriticalSection::unlock()
{
    ::LeaveCriticalSection(&m_cs);
    return 0;
}
