#pragma once

#include <list>
#include <memory>
#include <exception>

#include "WaitLock.h"

/**
 * Locked non-prioritized queue implementation
 * @author Pavel Plesov <pavel.plesov@gmail.com>
 */
template<typename Item>
class LockedQueue
{
public:
	LockedQueue() : m_running(false) {}
	virtual ~LockedQueue() throw() { Stop(); }

	void Start() throw()
	{
		Guard guard(&m_lock);
		m_running = true;
	}

	void Stop() throw()
	{
		Guard guard(&m_lock);
		m_running = false;
		m_lock.Broadcast();
	}

	/** Add single item to the end of queue */
	void Push(const Item& elm) throw()
	{
		Guard guard(&m_lock);
		m_items.push_back(elm);
		/* Wake up single waiter */
		m_lock.Signal();
	}

	/** Extract single item from the head of queue.
	 * Will block in case of empty queue.
	 * @return true new element is successfuly extracted
	 * @return false no futher processing is possible
	 */
	bool Fetch(Item& out) throw()
	{
		Guard guard(&m_lock);

		/* Termination is requested */
		if (!m_running)
			return false;

		while (m_running && m_items.empty())
			m_lock.UnlockAndWait();

		if (!m_items.empty())
		{
			out = m_items.front();
			m_items.pop_front();
			return true;
		}

		/* Termination is requested */
		return false;
	}

protected:
	bool m_running;
	std::list<Item> m_items;
	WaitLock m_lock;
};
