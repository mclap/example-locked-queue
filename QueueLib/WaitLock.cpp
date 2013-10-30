#include "WaitLock.h"

#if defined(_WIN32)
#include <windows.h>
#define LOCK_WINDOWS

#elif defined(linux)
#include <pthread.h>
#include <time.h> /* for clock_gettime(2) */
#define LOCK_POSIX

#else
#error Platform is not supported

#endif

/* TODO: add error checking */
class WaitLock::PrivateImpl
{
public:
	PrivateImpl()
	{
#if defined(LOCK_WINDOWS)
		InitializeCriticalSection(&m_lock);
		InitializeConditionVariable(&m_condition);
#elif defined(LOCK_POSIX)
		pthread_mutex_init(&m_lock, 0);
		pthread_cond_init(&m_condition, 0);
#endif
	}

	~PrivateImpl()
	{
#if defined(LOCK_WINDOWS)
		DeleteCriticalSection(&m_lock);
#elif defined(LOCK_POSIX)
		pthread_mutex_destroy(&m_lock);
		pthread_cond_destroy(&m_condition);
#endif
	}

	void Lock()
	{
#if defined(LOCK_WINDOWS)
		EnterCriticalSection(&m_lock);
#elif defined(LOCK_POSIX)
		pthread_mutex_lock(&m_lock);
#endif
	}

	void Unlock()
	{
#if defined(LOCK_WINDOWS)
		LeaveCriticalSection(&m_lock);
#elif defined(LOCK_POSIX)
		pthread_mutex_unlock(&m_lock);
#endif
	}

	bool UnlockAndWait(unsigned milliseconds) throw()
	{
#if defined(LOCK_WINDOWS)
		return (TRUE == SleepConditionVariableCS(&m_condition, &m_lock, milliseconds));
#elif defined(LOCK_POSIX)
		timespec future;
		clock_gettime(CLOCK_MONOTONIC, &future);
		future.tv_nsec += milliseconds * 1000;

		return (0 == pthread_cond_timedwait(&m_condition, &m_lock, &future));
#endif
	}

	bool UnlockAndWait() throw()
	{
#if defined(LOCK_WINDOWS)
		return (TRUE == UnlockAndWait(INFINITE));
#elif defined(LOCK_POSIX)
		return (0 == pthread_cond_wait(&m_condition, &m_lock));
#endif
	}


	void Signal()
	{
#if defined(LOCK_WINDOWS)
		WakeConditionVariable(&m_condition);
#elif defined(LOCK_POSIX)
		pthread_cond_signal(&m_condition);
#endif
	}

	void Broadcast()
	{
#if defined(LOCK_WINDOWS)
		WakeAllConditionVariable(&m_condition);
#elif defined(LOCK_POSIX)
		pthread_cond_broadcast(&m_condition);
#endif
	}

private:
#if defined(LOCK_WINDOWS)
	CRITICAL_SECTION m_lock;
	CONDITION_VARIABLE m_condition;
#elif defined(LOCK_POSIX)
	pthread_mutex_t m_lock;
	pthread_cond_t m_condition;
#endif
};


WaitLock::WaitLock() throw()
{
	m_pimpl = new PrivateImpl();
}

WaitLock::~WaitLock() throw()
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
	m_pimpl->UnlockAndWait();
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
