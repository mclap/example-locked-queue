#pragma once

#include <list>
#include <memory>
#include <exception>

#include "WaitLock.h"

/**
 * Locked non-prioritized queue implementation
 * @author Pavel Plesov <pavel.plesov@gmail.com>
 */
class LockedQueue
{
public:
	class Item
	{
	public:
		Item() {};
		virtual ~Item() {};
	};

public:
	LockedQueue();
	~LockedQueue() throw();

	void Start() throw();
	void Stop() throw();

	/** Add single item to the end of queue */
	void Push(Item *ptr) throw();

	/** Extract single item from the head of queue.
	 * Will block in case of empty queue.
	 * @return true new element is successfuly extracted
	 * @return false no futher processing is possible
	 */
	bool Fetch(std::auto_ptr<Item>& out) throw();

protected:
	bool m_running;
	std::list<std::auto_ptr<Item> > m_items;
	WaitLock m_lock;

};
