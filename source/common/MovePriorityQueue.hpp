#pragma once
#include <functional>
#include <vector>
#include <algorithm>

template<typename T, typename Comparator = std::less<T>>
class MovePriorityQueue
{
public:
	typedef typename std::vector<T>::const_iterator Iterator;

private:
	std::vector<T> m_items;

public:
	MovePriorityQueue()
	{
	}

public:
	template<typename Arg1>
	void push(const Arg1& arg1)
	{
		m_items.push_back(arg1);
		std::push_heap(m_items.begin(), m_items.end(), Comparator());
	}

	template<typename Arg1, typename Arg2>
	void push(const Arg1& arg1, const Arg2& arg2)
	{
		m_items.push_back(arg1, arg2);
		std::push_heap(m_items.begin(), m_items.end(), Comparator());
	}

	template<typename Arg1, typename Arg2, typename Arg3>
	void push(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
	{
		m_items.push_back(arg1, arg2, arg3);
		std::push_heap(m_items.begin(), m_items.end(), Comparator());
	}

	void popInto(T& item)
	{
		std::pop_heap(m_items.begin(), m_items.end(), Comparator());
		item = m_items.back();
		m_items.pop_back();
	}

	const T& top() const
	{
		return m_items.front();
	}

	void resort()
	{
		std::make_heap(m_items.begin(), m_items.end(), Comparator());
	}

	void clear()
	{
		m_items.clear();
	}

	size_t size() const
	{
		return m_items.size();
	}

	bool empty() const
	{
		return m_items.empty();
	}

	Iterator begin() const
	{
		return m_items.begin();
	}

	Iterator end() const
	{
		return m_items.end();
	}
};
