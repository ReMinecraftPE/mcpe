#include "Semaphore.hpp"

Semaphore::Semaphore()
	: m_count(0)
{
}

void Semaphore::notify()
{
	m_mutex.lock();

	m_count++;
	m_condition.notify_one();

	m_mutex.unlock();
}

void Semaphore::wait()
{
	std::unique_lock<std::mutex> lock(m_mutex);

	do
	{
		m_condition.wait(lock);
	} while (m_count.load(std::memory_order_acquire) == 0);

	m_count--;
}
