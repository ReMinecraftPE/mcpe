#include <cassert>
#include "BackgroundQueuePool.hpp"
#include "BackgroundQueue.hpp"
#include "common/Utils.hpp"

BackgroundQueuePool* BackgroundQueuePool::instance = nullptr;

BackgroundQueuePool::RolePool::RolePool()
	: m_counter(0)
{
}

BackgroundQueuePool::RolePool::~RolePool()
{
	for (std::vector<BackgroundQueue*>::iterator iter = m_queues.begin(); iter != m_queues.end(); iter++)
	{
		delete (*iter);
	}
}

void BackgroundQueuePool::RolePool::add(BackgroundQueue& queue)
{
	m_queues.push_back(&queue);
}

BackgroundQueue& BackgroundQueuePool::RolePool::get()
{
	m_counter++;
	return *m_queues[m_counter % size()];
}

size_t BackgroundQueuePool::RolePool::size() const
{
	return m_queues.size();
}

const std::vector<BackgroundQueue*>& BackgroundQueuePool::RolePool::getQueues() const
{
	return m_queues;
}

BackgroundQueuePool::BackgroundQueuePool()
	: m_state(PS_NORMAL),
	  m_nextWorkerIndex(0)
{
}

BackgroundQueuePool::~BackgroundQueuePool()
{
	for (std::vector<BackgroundQueue*>::iterator iter = m_workers.begin(); iter != m_workers.end(); iter++)
	{
		BackgroundQueue* queue = *iter;
		queue->stop();
		delete queue;
	}
}

void BackgroundQueuePool::assign(QueueRole role, BackgroundQueue& queue)
{
	m_rolePools[role].add(queue);
}

BackgroundQueue& BackgroundQueuePool::getFor(int role)
{
	return m_rolePools[role].get();
}

BackgroundQueue& BackgroundQueuePool::getFor(QueueRole role)
{
	return m_rolePools[role].get();
}

BackgroundQueue& BackgroundQueuePool::getMain()
{
	return getFor(QR_MAIN);
}

const std::vector<BackgroundQueue*>& BackgroundQueuePool::getQueuesFor(QueueRole role) const
{
	return m_rolePools[role].getQueues();
}

size_t BackgroundQueuePool::count(QueueRole role) const
{
	return m_rolePools[role].size();
}

void BackgroundQueuePool::processCoroutines()
{
	switch (m_state)
	{
	case PS_NORMAL:
		_runCoroutines(DBL_MAX);
		break;

	case PS_THROTTLED:
		_runCoroutines(0.2);
		break;
	}
}

void BackgroundQueuePool::setMainThreadHasPriority(bool value)
{
	m_state = value ? PS_NORMAL : PS_THROTTLED;
}

bool BackgroundQueuePool::_runCoroutines(double timeCap)
{
	double startTime = getTimeS();

	size_t nonWorkCount = 0;
	while (true)
	{
		while (!m_workers[m_nextWorkerIndex]->processNext())
		{
			nonWorkCount++;
			m_nextWorkerIndex = (m_nextWorkerIndex + 1) % m_workers.size();

			if (nonWorkCount >= m_workers.size())
				return true;
		}

		double currentTime = getTimeS();
		if ((currentTime - startTime) >= timeCap)
			return false;
	}
}

void BackgroundQueuePool::_setLocal(BackgroundQueue& queue)
{
	m_threadQueues[std::this_thread::get_id()] = &queue;
}

BackgroundQueue& BackgroundQueuePool::_addWorker(const std::string& name, bool async)
{
	m_workers.push_back(new BackgroundQueue(name, async));
	return *m_workers.back();
}

void BackgroundQueuePool::_start(int hardwareConcurrency)
{
	assert(hardwareConcurrency > 0);

	BackgroundQueue& mainWorker = _addWorker("Main Thread", false);
	assign(QR_MAIN, mainWorker);

	if (hardwareConcurrency > 1)
	{
		for (int i = 1; i <= hardwareConcurrency; i++)
		{
			char name[16];
			sprintf_s(name, "Streaming %d", i);

			BackgroundQueue& streamingWorker = _addWorker(name, true);
			assign(QR_STREAMING, streamingWorker);
		}
	}
	else
	{
		// use the main worker for streaming
		assign(QR_STREAMING, mainWorker);
	}

	BackgroundQueue& ioWorker = _addWorker("IO Thread", true);
	assign(QR_IO, ioWorker);
}

BackgroundQueue* BackgroundQueuePool::getForThread(std::thread::id threadId)
{
	std::unordered_map<std::thread::id, BackgroundQueue*>::const_iterator iter = getInstance().m_threadQueues.find(threadId);
	if (iter != getInstance().m_threadQueues.end())
		return iter->second;

	return nullptr;
}

BackgroundQueue* BackgroundQueuePool::getLocal()
{
	return getForThread(std::this_thread::get_id());
}

void BackgroundQueuePool::start(int hardwareConcurrency)
{
	assert(!instance);
	instance = new BackgroundQueuePool();
	instance->_start(hardwareConcurrency);
}

void BackgroundQueuePool::stop()
{
	delete instance;
	instance = nullptr;
}

BackgroundQueuePool& BackgroundQueuePool::getInstance()
{
	assert(instance);
	return *instance;
}
