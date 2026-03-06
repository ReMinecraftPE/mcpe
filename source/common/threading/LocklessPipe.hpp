#pragma once
#include <atomic>
#include <cassert>

// A circular, lockless queue
// This is thread-safe as long as there is one consumer and one producer
template<typename T, size_t MaxItemsSize = 512>
class LocklessPipe
{
private:
	// Starting node items size
	static const int START_SIZE = 16;

private:
	// this is required because of how limited older C++ templating was
	template<typename Arg1>
	struct ArgHolder1
	{
	private:
		Arg1& m_arg1;

	public:
		ArgHolder1(Arg1& arg1)
			: m_arg1(arg1)
		{
		}

		void create_at(void* block)
		{
			new (block) T(m_arg1);
		}
	};

	template<typename Arg1, typename Arg2>
	struct ArgHolder2
	{
	private:
		Arg1& m_arg1;
		Arg2& m_arg2;

	public:
		ArgHolder2(Arg1& arg1, Arg2& arg2)
			: m_arg1(arg1), m_arg2(arg2)
		{
		}

		void create_at(void* block)
		{
			new (block) T(m_arg1, m_arg2);
		}
	};

	template<typename Arg1, typename Arg2, typename Arg3>
	struct ArgHolder3
	{
	private:
		Arg1& m_arg1;
		Arg2& m_arg2;
		Arg3& m_arg3;

	public:
		ArgHolder3(Arg1& arg1, Arg2& arg2, Arg3& arg3)
			: m_arg1(arg1), m_arg2(arg2), m_arg3(arg3)
		{
		}

		void create_at(void* block)
		{
			new (block) T(m_arg1, m_arg2, m_arg3);
		}
	};

	class Node
	{
	public:
		std::atomic<size_t> m_readIndex;

		std::atomic<size_t> m_writeIndex;

		Node* m_nextNode;
		size_t m_sizeMask;
		char* m_items;

	public:
		Node(size_t numItems)
			: m_readIndex(0),
			  m_writeIndex(0),
			  m_nextNode(this),
			  m_sizeMask(numItems - 1),
			  m_items(new char[sizeof(T) * numItems])
		{
		}

		~Node()
		{
			size_t last = m_writeIndex.load(std::memory_order_relaxed);
			for (size_t i = m_readIndex.load(std::memory_order_relaxed); i < last; i++)
			{
				T& item = get_at(i);
				item.~T();
			}

			delete[] m_items;
		}

		T& get_at(size_t index) const
		{
			char& p = m_items[sizeof(T) * index];
			return reinterpret_cast<T&>(p);
		}

		template<typename ArgHolder>
		void copy_to(ArgHolder& argHolder, size_t index)
		{
			void* p = reinterpret_cast<void*>(&m_items[sizeof(T) * index]);
			argHolder.create_at(p);
		}

		size_t next_index(size_t index) const
		{
			return (index + 1) & m_sizeMask;
		}
	};

private:
	std::atomic<Node*> m_head;
	
	std::atomic<Node*> m_tail;
	size_t m_lastAssignedSize;

private:
	// disable copying
	LocklessPipe(const LocklessPipe&);
	LocklessPipe& operator=(const LocklessPipe&);

public:
	LocklessPipe()
		: m_lastAssignedSize(START_SIZE)
	{
		Node* node = new Node(START_SIZE);
		m_head.store(node, std::memory_order_relaxed);
		m_tail.store(node, std::memory_order_relaxed);
	}

	~LocklessPipe()
	{
		Node* head = m_head.load(std::memory_order_relaxed);
		Node* currentNode = head;
		do
		{
			for (size_t i = currentNode->m_readIndex; i != currentNode->m_writeIndex; i = currentNode->next_index(i))
			{
				T& item = currentNode->get_at(i);
				item.~T();
			}

			Node* nextNode = currentNode->m_nextNode;
			delete currentNode;
			currentNode = nextNode;
		} while (currentNode != head);
	}

public:
	template<typename Arg1>
	bool enqueue(Arg1& arg1)
	{
		ArgHolder1<Arg1> holder(arg1);
		return enqueue_internal(holder);
	}

	template<typename Arg1, typename Arg2>
	bool enqueue(Arg1& arg1, Arg2& arg2)
	{
		ArgHolder2<Arg1, Arg2> holder(arg1, arg2);
		return enqueue_internal(holder);
	}

	template<typename Arg1, typename Arg2, typename Arg3>
	bool enqueue(Arg1& arg1, Arg2& arg2, Arg3& arg3)
	{
		ArgHolder3<Arg1, Arg2, Arg3> holder(arg1, arg2, arg3);
		return enqueue_internal(holder);
	}

private:
	// force inline so the compiler doesnt actually create the argholder struct
	template<typename ArgHolder>
	__forceinline bool enqueue_internal(ArgHolder& argHolder)
	{
		Node* node = m_tail.load(std::memory_order_relaxed);
		size_t writeIndex = node->m_writeIndex.load(std::memory_order_relaxed);
		size_t nextWriteIndex = node->next_index(writeIndex);
		size_t readIndex = node->m_readIndex.load(std::memory_order_acquire);

		// check if the tail node has any capacity left
		// if the next write index is the same as the read index, that indicates that we no longer have any capacity left at this node
		if (nextWriteIndex == readIndex)
		{
			// check if the next node is at the head
			// we will likely run out of capacity if we enqueue in the same node as the consumer
			if (node->m_nextNode == m_head.load(std::memory_order_acquire))
			{
				// we need a new node
				// increase the amount of items stored in this node
				// with a max determined by "MaxItemsSize"
				size_t newNumItems = m_lastAssignedSize;
				if (MaxItemsSize > newNumItems)
					newNumItems *= 2;

				Node* newNode = new Node(newNumItems);
				if (!newNode)
					return false;

				newNode->m_nextNode = node->m_nextNode;
				node->m_nextNode = newNode;
				m_tail.store(newNode, std::memory_order_relaxed);
				m_lastAssignedSize = newNumItems;

				// now add the item
				newNode->copy_to(argHolder, 0);
				newNode->m_writeIndex.store(1, std::memory_order_release);

				return true;
			}
			else
			{
				// the consumer is not here, so we're free to write here
				// advance to the next node
				node = node->m_nextNode;
				writeIndex = node->m_writeIndex.load(std::memory_order_relaxed);
				nextWriteIndex = node->next_index(writeIndex);
				m_tail.store(node, std::memory_order_relaxed);
			}
		}

		node->copy_to(argHolder, writeIndex);
		node->m_writeIndex.store(nextWriteIndex, std::memory_order_release);

		return true;
	}

public:
	template<typename Type>
	bool try_dequeue(Type& item)
	{
		Node* node = m_head.load(std::memory_order_relaxed);
		size_t readIndex = node->m_readIndex.load(std::memory_order_relaxed);
		size_t writeIndex = node->m_writeIndex.load(std::memory_order_acquire);

		// check if the head node is empty
		if (readIndex == writeIndex)
		{
			// are we the final node? (tail)
			if (node == node->m_nextNode)
				return false;

			// advance head to the next node
			node = node->m_nextNode;
			m_head.store(node, std::memory_order_release);

			readIndex = node->m_readIndex.load(std::memory_order_relaxed);
			writeIndex = node->m_writeIndex.load(std::memory_order_acquire);

			// is this node too empty?
			if (readIndex == writeIndex)
				return false; // give up, there aren't any further items
		}

		T& poppedItem = node->get_at(readIndex);
		item = Type(poppedItem); // copy over

		poppedItem.~T(); // we no longer have a need for this

		size_t nextReadIndex = node->next_index(readIndex);
		node->m_readIndex.store(nextReadIndex, std::memory_order_release);

		return true;
	}
};
