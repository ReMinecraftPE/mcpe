#pragma once
#include <condition_variable>
#include <mutex>

class Semaphore
{
private:
	std::mutex m_mutex;
	std::condition_variable m_condition;
	std::atomic<size_t> m_count;

public:
	// NOTE: boolean ctor removed because it did nothing
	Semaphore();
	void notify();
	void wait();
};
