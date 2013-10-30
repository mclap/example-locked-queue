#include "WaitLock.h"

#include <windows.h>

/* TODO: add error checking */
class WaitLock::PrivateData
{
public:
	PrivateData()
	{
		InitializeCriticalSection(&m_lock);
		InitializeConditionVariable(&m_condition);
	}

	~PrivateData()
	{
		DeleteCriticalSection(&m_lock);
		InitializeConditionVariable(&m_condition);
	}

	void Lock()
	{
		EnterCriticalSection(&m_lock);
	}

	void Unlock()
	{
		LeaveCriticalSection(&m_lock);
	}

	bool UnlockAndWait(unsigned milliseconds) throw()
	{
		return SleepConditionVariableCS(&m_condition, &m_lock, milliseconds);
	}

	void Signal()
	{
		WakeConditionVariable(&m_condition);
	}

	void Broadcast()
	{
		WakeAllConditionVariable(&m_condition);
	}

private:
	CRITICAL_SECTION m_lock;
	CONDITION_VARIABLE m_condition;
};


WaitLock::WaitLock()
{
	m_pimpl = new PrivateData();
}

WaitLock::~WaitLock()
{
	delete m_pimpl;
}

void WaitLock::Lock() throw()
{
	m_pimpl->Lock();
}

void WaitLock::Unlock() throw()
{
	m_pimpl->Unlock();
}

bool WaitLock::UnlockAndWait(unsigned milliseconds) throw()
{
	return m_pimpl->UnlockAndWait(milliseconds);
}

void WaitLock::UnlockAndWait() throw()
{
	m_pimpl->UnlockAndWait(INFINITE);
}

void WaitLock::Signal() throw()
{
	m_pimpl->Signal();
}

void WaitLock::Broadcast() throw()
{
	m_pimpl->Broadcast();
}

Guard::Guard(WaitLock *lock) throw()
	: m_lock(lock)
{
	m_lock->Lock();
}

Guard::~Guard() throw()
{
	m_lock->Unlock();
}
