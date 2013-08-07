#include "LockedQueue.h"

LockedQueue::LockedQueue()
	: m_running(false)
{
}

LockedQueue::~LockedQueue() throw()
{
	Stop();
}

void LockedQueue::Start() throw()
{
	Guard guard(&m_lock);
	m_running = true;
}

void LockedQueue::Stop() throw()
{
	Guard guard(&m_lock);
	m_running = false;
	m_lock.Broadcast();
}

void LockedQueue::Push(Item *ptr) throw()
{
	Guard guard(&m_lock);
	m_items.push_back(std::auto_ptr<Item>(ptr));
}

bool LockedQueue::Fetch(std::auto_ptr<Item>& out) throw()
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
