#pragma once
#include <cassert>
#include <iterator>

// Circular buffer with a fixed capacity
template<typename T, size_t Size>
class FixedCircularBuffer
{
private:
	// These are here just to make const casts cleaner
	typedef FixedCircularBuffer* this_type;
	typedef const FixedCircularBuffer* const_this_type;

public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

private:
	char m_data[sizeof(value_type) * Size]; // Avoid unnecessary initialisations
	pointer m_first;
	pointer m_last;
	pointer m_end;
	size_type m_size;

private:
	pointer data()
	{
		return reinterpret_cast<pointer>(m_data);
	}

	const_pointer data() const
	{
		return reinterpret_cast<const_pointer>(m_data);
	}

private:
	const_pointer getPointerAfter(const_pointer ptr, size_type i) const
	{
		if (ptr + i >= &data()[Size])
			return &data()[0] + ((ptr + i) - &data()[Size]);
		else
			return ptr + i;
	}

	pointer getPointerAfter(pointer ptr, size_type i)
	{
		return const_cast<pointer>(
			const_cast<const_this_type>(this)->getPointerAfter(ptr, i)
		);
	}

	void incrementPointer(pointer& ptr)
	{
		if (ptr == &data()[Size - 1])
			ptr = &data()[0];
		else
			ptr++;
	}

	void decrementPointer(pointer& ptr)
	{
		if (ptr == &data()[0])
			ptr = &data()[Size - 1];
		else
			ptr--;
	}

public:
	size_type capacity() const
	{
		return Size;
	}

	size_type size() const
	{
		return m_size;
	}

	bool empty() const
	{
		return size() == 0;
	}

	bool full() const
	{
		return size() == capacity();
	}

	void push_back(const_reference val)
	{
		if (full())
			pop_front();

		new (m_end) T(val);

		m_last = m_end;
		incrementPointer(m_end);
		++m_size;
	}

	void pop_back()
	{
		assert(!empty());

		m_last->~value_type();
		decrementPointer(m_last);
		decrementPointer(m_end);
		--m_size;
	}

	void pop_front()
	{
		assert(!empty());

		m_first->~value_type();
		incrementPointer(m_first);
		--m_size;
	}

	reference front()
	{
		assert(!empty());
		return *m_first;
	}

	const_reference front() const
	{
		assert(!empty());
		return *m_first;
	}

	reference back()
	{
		assert(!empty());
		return *m_last;
	}

	const_reference back() const
	{
		assert(!empty());
		return *m_last;
	}

	reference operator[](size_type i)
	{
		assert(i >= 0 && i < size());
		return *getPointerAfter(m_first, i);
	}

	const_reference operator[](size_type i) const
	{
		assert(i >= 0 && i < size());
		return *getPointerAfter(m_first, i);
	}

	class iterator_base
	{
	public:
		//typedef std::random_access_iterator_tag iterator_category;
		typedef std::forward_iterator_tag iterator_category; // Backwards iteration is not currently supported
		typedef FixedCircularBuffer::value_type value_type;
		typedef FixedCircularBuffer::pointer pointer;
		typedef FixedCircularBuffer::const_pointer const_pointer;
		typedef FixedCircularBuffer::reference reference;
		typedef FixedCircularBuffer::const_reference const_reference;
		typedef FixedCircularBuffer::difference_type difference_type;

	protected:
		FixedCircularBuffer* buffer;
		pointer ptr;

	protected:
		iterator_base(FixedCircularBuffer* buffer, pointer ptr)
			: buffer(buffer)
			, ptr(ptr)
		{
		}

	public:
		bool operator==(const iterator_base& other) const
		{
			return buffer == other.buffer && ptr == other.ptr;
		}

		bool operator!=(const iterator_base& other) const
		{
			return buffer != other.buffer || ptr != other.ptr;
		}

		// ++it
		iterator_base& operator++()
		{
			assert(ptr);

			buffer->incrementPointer(ptr);

			if (ptr == buffer->m_end)
				ptr = NULL;

			return *this;
		}

		// it++
		iterator_base operator++(int)
		{
			iterator_base temp = *this;
			++(*this);
			return temp;
		}
	};

	class const_iterator : public iterator_base
	{
	public:
		const_iterator(const FixedCircularBuffer* buffer, value_type* ptr)
			: iterator_base(const_cast<this_type>(buffer), ptr)
		{
		}

		const_reference operator*() const
		{
			assert(this->ptr);
			return *this->ptr;
		}

		const_pointer operator->() const
		{
			assert(this->ptr);
			return this->ptr;
		}
	};

	class iterator : public const_iterator
	{
	public:
		iterator(FixedCircularBuffer* buffer, value_type* ptr)
			: const_iterator(buffer, ptr)
		{
		}

		reference operator*()
		{
			assert(this->ptr);
			return *this->ptr;
		}

		pointer operator->()
		{
			assert(this->ptr);
			return this->ptr;
		}
	};

	friend class iterator_base;
	friend class iterator;
	friend class const_iterator;

	iterator begin()
	{
		return iterator(this, empty() ? NULL : m_first);
	}

	iterator end()
	{
		return iterator(this, NULL);
	}

	const_iterator begin() const
	{
		return const_iterator(this, empty() ? NULL : m_first);
	}

	const_iterator end() const
	{
		return const_iterator(this, NULL);
	}

	void clear()
	{
		for (iterator it = begin(); it != end(); it++)
			(*it).~value_type();

		m_first = &data()[0];
		m_last = &data()[0];
		m_end = &data()[0];
		m_size = 0;
	}

	FixedCircularBuffer()
		: m_first(&data()[0])
		, m_last(&data()[0])
		, m_end(&data()[0])
		, m_size(0)
	{
	}

	~FixedCircularBuffer()
	{
		for (iterator it = begin(); it != end(); it++)
			(*it).~value_type();
	}
};
