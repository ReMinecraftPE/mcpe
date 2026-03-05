#include <Windows.h>
#include "BackgroundQueue.hpp"
#include "BackgroundQueuePool.hpp"

std::function<void()> BackgroundQueue::NOP;

static void SetThreadName(const std::string& name)
{
	// TODO!
}

BackgroundQueue::Job::Job()
	: m_runFunction(nullptr),
	  m_callbackFunction(nullptr),
	  m_priority(TOP_PRIORITY)
{
}

BackgroundQueue::Job::Job(const std::function<bool()>& run, const std::function<void()>& callback, int priority)
	: m_runFunction(run),
	  m_callbackFunction(callback),
	  m_priority(priority)
{
}

BackgroundQueue::Job::~Job()
{
	clear();
}

bool BackgroundQueue::Job::canRun() const
{
	return m_runFunction != nullptr;
}

bool BackgroundQueue::Job::run()
{
	return m_runFunction();
}

bool BackgroundQueue::Job::hasCallback() const
{
	return m_callbackFunction != nullptr;
}

const std::function<void()>& BackgroundQueue::Job::getCallback() const
{
	return m_callbackFunction;
}

int BackgroundQueue::Job::getPriority() const
{
	return m_priority;
}

void BackgroundQueue::Job::clear()
{
	m_runFunction = nullptr;
	m_callbackFunction = nullptr;
	m_priority = TOP_PRIORITY;
}

void BackgroundQueue::Job::_move(Job& other)
{
	m_runFunction = other.m_runFunction;
	other.m_runFunction = nullptr;

	m_callbackFunction = other.m_callbackFunction;
	other.m_callbackFunction = nullptr;

	m_priority = other.m_priority;
}

template<>
struct std::less<BackgroundQueue::Job>
{
	bool operator()(const BackgroundQueue::Job& left, const BackgroundQueue::Job& right) const
	{
		return left.getPriority() < right.getPriority();
	}
};

BackgroundQueue::BackgroundQueue(const std::string& name, bool async)
	: m_name(name),
	  m_async(async),
	  m_jobQueue(nullptr),
	  m_callbackQueue(nullptr),
	  m_semaphore(nullptr)
{
	_start();
}

BackgroundQueue::~BackgroundQueue()
{
	stop();

	delete m_jobQueue;
	delete m_callbackQueue;
	delete m_semaphore;
}

const std::string& BackgroundQueue::getName()
{
	return m_name;
}

void BackgroundQueue::resume()
{
	_setState(QS_RUNNING);
}

bool BackgroundQueue::_pauseQueueJobFunction()
{
	_setState(QS_PAUSING);
	return true;
}

void BackgroundQueue::pause()
{
	std::function<bool()> pauseFunc = std::bind(&BackgroundQueue::_pauseQueueJobFunction, this);
	queue(pauseFunc, NOP, 0);

	while (getState() != QS_PAUSED)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(15000000));
	}
}

bool BackgroundQueue::_stopQueueJobFunction()
{
	_setState(QS_STOPPED);
	return true;
}

void BackgroundQueue::stop()
{
	if (!m_async)
		return;

	if (getState() == QS_STOPPED)
		return;

	if (getState() == QS_PAUSED)
	{
		_setState(QS_STOPPED);
	}
	else
	{
		std::function<bool()> stopFunc = std::bind(&BackgroundQueue::_stopQueueJobFunction, this);
		queue(stopFunc, NOP, 0);

		// MATT: i added this in myself, it would instantly kill the thread otherwise
		while (getState() != QS_STOPPED)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(15000000));
		}
	}

	if (m_thread.native_handle() != nullptr)
		m_thread.join();

	_resetData();
}

static bool _SyncJobFunction(Semaphore* semaphore)
{
	semaphore->notify();
	return true;
}

void BackgroundQueue::sync()
{
	do
	{
		Semaphore semaphore;

		std::function<bool()> syncFunc = std::bind(_SyncJobFunction, &semaphore);
		queue(syncFunc, NOP, TOP_PRIORITY);

		semaphore.wait();
	} while (_processCallbacks());
}

void BackgroundQueue::flush()
{
	if (_workerThread())
	{
		_resetData();
	}
	else
	{
		pause();
		_resetData();
		resume();
	}
}

bool BackgroundQueue::processNext()
{
	if (_processNextCallback())
		return true;

	if (!m_async)
	{
		if (_processNextCoroutine())
			return true;
	}

	return false;
}

void BackgroundQueue::queue(Job& job)
{
	if (_workerThread())
		m_localJobQueue.push_back(job);
	else
		m_jobQueue->enqueue(job);

	m_semaphore->notify();
}

void BackgroundQueue::queue(const std::function<bool()>& runFunction, const std::function<void()>& callbackFunction, int priority)
{
	if (_workerThread())
		m_localJobQueue.push_back(Job(runFunction, callbackFunction, priority));
	else
		m_jobQueue->enqueue(runFunction, callbackFunction, priority);

	m_semaphore->notify();
}

void BackgroundQueue::queueCallback(const std::function<void()>& callbackFunction)
{
	m_callbackQueue->enqueue(callbackFunction);
}

BackgroundQueue::State BackgroundQueue::getState()
{
	return m_state.load(std::memory_order_acquire);
}

void BackgroundQueue::_setState(State state)
{
	m_state.store(state, std::memory_order_release);
}

bool BackgroundQueue::_workerThread() const
{
	return std::this_thread::get_id() == m_threadId;
}

void BackgroundQueue::_start()
{
	_resetData();
	_setState(QS_RUNNING);

	if (m_async)
	{
		std::thread thread(&BackgroundQueue::_processingThreadLogic, this);
		m_thread.swap(thread);
	}
	else
	{
		SetThreadName(m_name);
		m_threadId = std::this_thread::get_id();
		BackgroundQueuePool::getInstance()._setLocal(*this);
	}
}

void BackgroundQueue::_resetData()
{
	m_localJobQueue.clear();
	m_jobPriorityQueue.clear();

	delete m_callbackQueue;
	m_callbackQueue = new LocklessPipe<std::function<void()>>();

	delete m_jobQueue;
	m_jobQueue = new LocklessPipe<Job>();

	delete m_semaphore;
	m_semaphore = new Semaphore();

	m_currentJob.clear();
}

bool BackgroundQueue::_tryPopLocal(Job& job)
{
	if (m_localJobQueue.empty())
		return false;

	job = m_localJobQueue.front();
	m_localJobQueue.pop_front();

	return true;
}

bool BackgroundQueue::_tryPopReal(Job& job)
{
	if (m_async)
	{
		if (m_jobQueue->try_dequeue(job))
			return true;
	}

	return _tryPopLocal(job);
}

void BackgroundQueue::_tryPop()
{
	if (!m_async)
	{
		_tryPopReal(m_currentJob);
		return;
	}

	Job job;
	while (_tryPopReal(job))
	{
		m_jobPriorityQueue.push(job);
	}

	if (m_jobPriorityQueue.empty())
	{
		m_semaphore->wait();
	}
	else
	{
		m_jobPriorityQueue.popInto(m_currentJob);
	}
}

bool BackgroundQueue::_processCallbacks()
{
	if (getState() == QS_PAUSED || getState() == QS_PAUSING)
		return false;

	bool processed = false;
	while (_processNextCallback())
		processed = true;

	return processed;
}

void BackgroundQueue::_processingThreadLogic()
{
	SetThreadName(m_name);
	m_threadId = std::this_thread::get_id();
	BackgroundQueuePool::getInstance()._setLocal(*this);

	while (getState() != QS_STOPPED)
	{
		if (getState() == QS_RUNNING)
		{
			_processNextCoroutine();
		}
		else if (getState() == QS_PAUSING)
		{
			_setState(QS_PAUSED);
		}
		else if (getState() == QS_PAUSED)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
}

bool BackgroundQueue::_processNextCallback()
{
	std::function<void()> callback;
	if (m_callbackQueue->try_dequeue(callback))
	{
		callback();
		return true;
	}

	return false;
}

bool BackgroundQueue::_processNextCoroutine()
{
	if (!m_currentJob.canRun())
	{
		_tryPop();
		if (!m_currentJob.canRun())
			return false;
	}

	if (m_currentJob.run())
	{
		if (m_currentJob.hasCallback())
			m_callbackQueue->enqueue(m_currentJob.getCallback());
	}
	else
	{
		queue(m_currentJob);
	}

	m_currentJob.clear();
	m_coroutineProcessCounter++;
	return true;
}
