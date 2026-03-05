#pragma once
#include <Windows.h>
#include <set>
#include <functional>
#include <mutex>
#include <cassert>

template<typename T>
class ThreadLocal
{
private:
	std::function<T* ()> m_creatorFunction;
	std::set<T*> m_pool;
	std::mutex m_creatorMutex;
	DWORD m_key;

private:
	// disable copy constructors
	ThreadLocal(const ThreadLocal&);
	ThreadLocal& operator=(const ThreadLocal&);

private:
	static T* _Create()
	{
		return new T();
	}

public:
	ThreadLocal()
		: m_creatorFunction(std::function<T* ()>(_Create)),
		  m_key(TlsAlloc())
	{
		assert(m_key);
	}

	ThreadLocal(const std::function<T* ()>& creator)
		: m_creatorFunction(creator),
		  m_key(TlsAlloc())
	{
		assert(m_key);
	}

	~ThreadLocal()
	{
		BOOL result = TlsFree(m_key);
		assert(result == TRUE);

		for (typename std::set<T*>::iterator iter = m_pool.begin(); iter != m_pool.end(); iter++)
			delete (*iter);
	}

private:
	T* _get() const
	{
		return reinterpret_cast<T*>(TlsGetValue(m_key));
	}

public:
	T& getLocal()
	{
		T* storedPtr = _get();
		if (storedPtr)
			return *storedPtr;

		std::lock_guard<std::mutex> lock(m_creatorMutex);

		T* ptr = m_creatorFunction();
		BOOL result = TlsSetValue(m_key, ptr);
		assert(result == TRUE);

		m_pool.insert(ptr);
		return *ptr;
	}

	T* getLocalPtr()
	{
		return &getLocal();
	}

	void resetLocal()
	{
		T* storedPtr = _get();
		if (!storedPtr)
			return;

		std::lock_guard<std::mutex> lock(m_creatorMutex);

		typename std::set<T*>::iterator iter = std::find(m_pool.begin(), m_pool.end(), storedPtr);
		if (iter != m_pool.end())
			m_pool.erase(iter);

		TlsSetValue(m_key, nullptr);
		delete storedPtr;
	}
};
