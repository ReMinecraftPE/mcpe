#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <thread>
#include <deque>
#include "common/threading/BackgroundQueue.hpp"

class BackgroundQueuePool
{
	friend class BackgroundQueue;

private:
	enum State
	{
		PS_NORMAL,
		PS_THROTTLED
	};

public:
	enum QueueRole
	{
		QR_STREAMING,
		QR_IO,
		QR_MAIN,
		QR_COUNT
	};

protected:
	class RolePool
	{
	private:
		size_t m_counter;
		std::vector<BackgroundQueue*> m_queues;

	public:
		RolePool();
		~RolePool();

	public:
		void add(BackgroundQueue& queue);
		BackgroundQueue& get();
		size_t size() const;
		const std::vector<BackgroundQueue*>& getQueues() const;
	};

protected:
	RolePool m_rolePools[QR_COUNT];
	size_t m_nextWorkerIndex;
	std::unordered_map<std::thread::id, BackgroundQueue*> m_threadQueues;
	State m_state;
	std::vector<BackgroundQueue*> m_workers;

private:
	// disable copying
	BackgroundQueuePool(const BackgroundQueuePool&);
	BackgroundQueuePool& operator=(const BackgroundQueuePool&);

protected:
	BackgroundQueuePool();
public:
	~BackgroundQueuePool();

public:
	void assign(QueueRole role, BackgroundQueue& queue);

	BackgroundQueue& getFor(int role);
	BackgroundQueue& getFor(QueueRole role);
	BackgroundQueue& getMain();
	const std::vector<BackgroundQueue*>& getQueuesFor(QueueRole role) const;
	size_t count(QueueRole role) const;

	void processCoroutines();
	
	void setMainThreadHasPriority(bool value);

protected:
	bool _runCoroutines(double timeCap);

	void _setLocal(BackgroundQueue& queue);

	BackgroundQueue& _addWorker(const std::string& name, bool async);
	void _start(int hardwareConcurrency);

public:
	static BackgroundQueue* getForThread(std::thread::id threadId);
	static BackgroundQueue* getLocal();

	static void start(int hardwareConcurrency);
	static void stop();

	static BackgroundQueue& GetFor(QueueRole role)
	{
		return getInstance().getFor(role);
	}

	static BackgroundQueuePool& getInstance();

private:
	static BackgroundQueuePool* instance;
};
