#pragma once

/**
 * Implementation condition+mutex idiom from POSIX using Win32 API for Windows Vista and higher
 * @author Pavel Plesov <pavel.plesov@gmail.com>
 * @see http://msdn.microsoft.com/en-us/library/windows/desktop/ms682052(v=vs.85).aspx
 */
class WaitLock
{
public:
	WaitLock() throw();
	~WaitLock() throw();

	void Lock() throw();
	void Unlock() throw();

	bool UnlockAndWait(unsigned milliseconds) throw();
	void UnlockAndWait() throw();

	/** Wake up the single waiter */
	void Signal() throw();

	/** Wake up all of the waiters */
	void Broadcast() throw();

private:
	class PrivateData;

	PrivateData *m_pimpl;

private:
	/* prohibit copying */
	WaitLock(const WaitLock& v);
	WaitLock& operator=(const WaitLock& v);
};

/** Acquire lock on initialization. Release on deinitialization */
class Guard
{
public:
	Guard(WaitLock *lock) throw();
	~Guard() throw();

private:
	WaitLock *m_lock;

private:
	/* prohibit copying */
	Guard(const Guard& v);
	Guard& operator=(const Guard& v);
};
