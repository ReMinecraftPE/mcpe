#pragma once
#include <functional>
#include <deque>
#include <vector>
#include "compat/LegacyCPP.hpp"
#include "common/threading/Semaphore.hpp"
#include "common/threading/LocklessPipe.hpp"
#include "common/MovePriorityQueue.hpp"

class BackgroundQueue
{
public:
	static const int TOP_PRIORITY = INT32_MAX;

public:
	class Job
	{
	private:
		std::function<bool()> m_runFunction;
		std::function<void()> m_callbackFunction;
		int m_priority;

	public:
		Job();
		Job(const std::function<bool()>& run, const std::function<void()>& callback, int priority);
		MC_CTOR_MOVE(Job);

		~Job();

	public:
		bool canRun() const;
		bool run();

		bool hasCallback() const;
		const std::function<void()>& getCallback() const;

		int getPriority() const;

		void clear();

		MC_FUNC_MOVE(Job);

	private:
		void _move(Job& other);
	};

public:
	static std::function<void()> NOP;

public:
	enum State
	{
		QS_STOPPED,
		QS_RUNNING,
		QS_PAUSING,
		QS_PAUSED
	};

private:
	bool m_async;
	std::string m_name;
	std::thread m_thread;
	std::thread::id m_threadId;
	Job m_currentJob;
	std::atomic<State> m_state;
	Semaphore* m_semaphore;
	int m_coroutineProcessCounter;
	LocklessPipe<Job>* m_jobQueue;
	LocklessPipe<std::function<void()>>* m_callbackQueue;
	std::deque<Job> m_localJobQueue;
	MovePriorityQueue<Job> m_jobPriorityQueue;

private:
	// disable copying
	BackgroundQueue(const BackgroundQueue&);
	BackgroundQueue& operator=(const BackgroundQueue&);

public:
	BackgroundQueue(const std::string& name, bool async);
	~BackgroundQueue();

public:
	const std::string& getName();

	void resume();
	void pause();
	void stop();

	void sync();
	void flush();

	bool processNext();

	void queue(Job& job);
	void queue(const std::function<bool()>& runFunction, const std::function<void()>& callbackFunction, int priority);

	void queueCallback(const std::function<void()>& callbackFunction);

	State getState();

private:
	bool _workerThread() const;

	void _setState(State state);

	void _start();

	void _resetData();

	bool _tryPopLocal(Job& job);
	bool _tryPopReal(Job& job);
	void _tryPop();

	bool _processCallbacks();
	bool _processNextCallback();
	bool _processNextCoroutine();

	void _processingThreadLogic();

	bool _pauseQueueJobFunction();
	bool _stopQueueJobFunction();
};
