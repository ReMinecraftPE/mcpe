#pragma once

#include <exception>
#include <algorithm>

#include "CThread.hpp"
#include "compat/LegacyCPP.hpp"

struct IFunctor
{
	virtual ~IFunctor() {}
	virtual void* call() = 0;
};

template <typename T>
struct Functor : IFunctor
{
	T func;
	Functor(const T& f) : func(f) {}
	void* call() { return func(); }
};

class AsyncTask
{
public:
	typedef void* (*Function)(void*);

public:
	enum Status
	{
		CREATED,
		RUNNING,
		COMPLETED,
		CANCELLED,
		FAULTED
	};

	class State
	{
	public:
		State(IFunctor* pFunctor)
		{
			m_bIsRunning = false;
			m_status = CREATED;
			m_pException = nullptr;
			m_pFunctor = pFunctor;
			m_pResult = nullptr;
		}

		~State()
		{
			delete m_pException;

			// @NOTE: THIS WILL NOT CALL THE DESTRUCTOR, SINCE IT'S A VOID PTR
			if (m_pResult)
				free(m_pResult);
		}

		volatile bool m_bIsRunning;
		Status m_status;
		const std::exception* m_pException;
		IFunctor* m_pFunctor;
		void* m_pResult;
	};

private:
	static void* tspawn_func(void* context)
	{
		State& state = *(State*)context;

		state.m_status = RUNNING;
		try
		{
			state.m_pResult = state.m_pFunctor->call();
			state.m_status = COMPLETED;
		}
		catch (const std::exception& exc)
		{
			state.m_pException = new std::exception(exc);
			state.m_status = FAULTED;
		}
		state.m_bIsRunning = false;

		delete state.m_pFunctor;

		return nullptr;
	}

public:
	AsyncTask()
	{
		m_pState = new State(nullptr);
		m_pThread = nullptr;
	}

	template <typename Callable>
	AsyncTask(const Callable& task)
	{
		m_pState = new State(new Functor<Callable>(task));
		m_pState->m_bIsRunning = true;
		m_pThread = new CThread(tspawn_func, m_pState);
	}

	MC_CTOR_MOVE(AsyncTask);

	~AsyncTask()
	{
		delete m_pThread;
		delete m_pState;
	}

private:
	inline const State& getState() const
	{
		return *m_pState;
	}

	inline State& getState()
	{
		return *m_pState;
	}

protected:
	void _move(AsyncTask& other)
	{
		std::swap(this->m_pState, other.m_pState);
		std::swap(this->m_pThread, other.m_pThread);
	}

public:
	inline bool isRunning() const
	{
		return getState().m_bIsRunning;
	}

	inline Status getStatus() const
	{
		// Avoid UB from reading a non-atomic (bool is usually atomic on CPU level by default)
		if (isRunning())
			return RUNNING;

		return getState().m_status;
	}

	inline const std::exception* getException() const
	{
		if (isRunning())
			return nullptr;

		return getState().m_pException;
	}

	inline void* getResult() const
	{
		if (isRunning())
			return nullptr;

		return getState().m_pResult;
	}

	const AsyncTask& join() const
	{
		if (!m_pThread)
			return *this;

		m_pThread->join();
		m_pState->m_bIsRunning = false;
		m_pState->m_status = COMPLETED;
		return *this;
	}

	void cancel()
	{
		State& state = getState();

		delete m_pThread;
		state.m_bIsRunning = false;
		state.m_status = CANCELLED;
	}

public:
	MC_FUNC_MOVE(AsyncTask);

private:
	State* m_pState;
	CThread* m_pThread;
};

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define MC_ASYNC_WRAPPER(...) __VA_ARGS__

#define MC_ASYNC_FUNC_1(name, T1, m1)       \
struct name##Async {                        \
    T1 m1;                                  \
    name##Async(T1 m1) : m1(m1) {}          \
    void* operator()() { return name(m1); } \
}

#define MC_ASYNC_FUNC_2(name, T1, m1, T2, m2)     \
struct name##Async {                              \
    T1 m1;                                        \
    T2 m2;                                        \
    name##Async(T1 m1, T2 m2) : m1(m1), m2(m2) {} \
    void* operator()() { return name(m1, m2); }   \
}

#define MC_ASYNC_FUNC_3(name, T1, m1, T2, m2, T3, m3)            \
struct name##Async {                                             \
    T1 m1;                                                       \
    T2 m2;                                                       \
    T3 m3;                                                       \
    name##Async(T1 m1, T2 m2, T3 m3) : m1(m1), m2(m2), m3(m3) {} \
    void* operator()() { return name(m1, m2, m3); }              \
}

#define MC_ASYNC_FUNC(n, ...) MC_ASYNC_WRAPPER(CONCAT(MC_ASYNC_FUNC_, n)(__VA_ARGS__))


#define MC_ASYNC_FUNC_VOID_1(name, T1, m1)               \
struct name##Async {                                     \
    T1 m1;                                               \
    name##Async(T1 m1) : m1(m1) {}                       \
    void* operator()() { name(m1); return nullptr; }     \
}

#define MC_ASYNC_FUNC_VOID_2(name, T1, m1, T2, m2)       \
struct name##Async {                                     \
    T1 m1;                                               \
    T2 m2;                                               \
    name##Async(T1 m1, T2 m2) : m1(m1), m2(m2) {}        \
    void* operator()() { name(m1, m2); return nullptr; } \
}

#define MC_ASYNC_FUNC_VOID_3(name, T1, m1, T2, m2, T3, m3)       \
struct name##Async {                                             \
    T1 m1;                                                       \
    T2 m2;                                                       \
    T3 m3;                                                       \
    name##Async(T1 m1, T2 m2, T3 m3) : m1(m1), m2(m2), m3(m3) {} \
    void* operator()() { name(m1, m2, m3); return nullptr; }     \
}

#define MC_ASYNC_FUNC_VOID(n, ...) MC_ASYNC_WRAPPER(CONCAT(MC_ASYNC_FUNC_VOID_, n)(__VA_ARGS__))