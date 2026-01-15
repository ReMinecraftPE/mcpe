#pragma once

#include <algorithm>

namespace mce
{
	// Mojang used the ComPtr class from Microsoft::WRL
	template <typename T>
	class ComInterface
	{
	private:
		T* m_pObject;

	private:
		void _init(T* pObject)
		{
			m_pObject = pObject;
			if (m_pObject)
				m_pObject->AddRef();
		}

	public:
		ComInterface(T* pObject = NULL)
		{
			_init(pObject);
		}
		ComInterface(const ComInterface& other)
		{
			_init(other.m_pObject);
		}
		~ComInterface()
		{
			release();
		}

	public:
		void _move(ComInterface& other)
		{
			std::swap(this->m_pObject, other.m_pObject);
		}
		void release()
		{
			T* pObject = m_pObject;
			if (pObject)
			{
				m_pObject = NULL;
				pObject->Release();
			}
		}

		const T* getPtr() const
		{
			return m_pObject;
		}

	public:
		void operator=(const ComInterface& other)
		{
			_init(other.m_pObject);
		}
		void operator=(T* value)
		{
			release();
			_init(value);
		}
		T* operator->()
		{
			return m_pObject;
		}
		T** operator*()
		{
			return &m_pObject;
		}
		operator bool() const
		{
			return m_pObject != NULL;
		}
	};
}
