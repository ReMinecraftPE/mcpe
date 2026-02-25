#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <utility>
#include <stdint.h>

template<typename TKey, typename TValue>
struct HashMapEntry
{
    std::pair<TKey, TValue> pair;
    bool bOccupied;
    bool bDeleted;
    
    HashMapEntry() : bOccupied(false), bDeleted(false) {}
};

template<typename TKey>
struct HashFunction
{
    size_t operator()(const TKey& key) const;
};

template<>
struct HashFunction<int8_t>
{
    size_t operator()(const int8_t& key) const { return key; }
};

template<>
struct HashFunction<uint8_t>
{
    size_t operator()(const uint8_t& key) const { return key; }
};

template<>
struct HashFunction<int16_t>
{
    size_t operator()(const int16_t& key) const { return key; }
};

template<>
struct HashFunction<uint16_t>
{
    size_t operator()(const uint16_t& key) const { return key; }
};

template<>
struct HashFunction<int32_t>
{
    size_t operator()(const int32_t& key) const { return key; }
};

template<>
struct HashFunction<uint32_t>
{
    size_t operator()(const uint32_t& key) const { return key; }
};

template<>
struct HashFunction<int64_t>
{
    size_t operator()(const int64_t& key) const { return key; }
};

template<>
struct HashFunction<uint64_t>
{
    size_t operator()(const uint64_t& key) const { return key; }
};

template<>
struct HashFunction<std::string>
{
    size_t operator()(const std::string& key) const
    {
        size_t hash = 5381;
        for (size_t i = 0; i < key.length(); i++)
            hash = ((hash << 5) + hash) + key[i];
        return hash;
    }
};

template<>
struct HashFunction<const char*>
{
    size_t operator()(const char* key) const
    {
        size_t hash = 5381;
        while (*key)
            hash = ((hash << 5) + hash) + size_t(*key++);
        return hash;
    }
};

template<typename TKey, typename TValue, typename THash = HashFunction<TKey> >
class HashMap
{
private:
    size_t _findSlot(const TKey& key) const
    {
        size_t hash = m_HashFunc(key);
        size_t slot = hash % capacity();
        size_t originalSlot = slot;

        while (m_entries[slot].bOccupied)
        {
            if (!m_entries[slot].bDeleted && _keysEqual(m_entries[slot].pair.first, key))
                return slot;

            slot = (slot + 1) % capacity();

            if (slot == originalSlot)
                break;
        }

        return capacity();
    }

    size_t _findSlotForInsert(const TKey& key) const
    {
        size_t hash = m_HashFunc(key);
        size_t slot = hash % capacity();
        size_t firstDeleted = capacity();
        size_t originalSlot = slot;

        while (m_entries[slot].bOccupied)
        {
            if (m_entries[slot].bDeleted)
            {
                if (firstDeleted == capacity())
                    firstDeleted = slot;
            }
            else if (_keysEqual(m_entries[slot].pair.first, key))
            {
                return slot;
            }

            slot = (slot + 1) % capacity();

            if (slot == originalSlot)
                break;
        }

        if (firstDeleted != capacity())
            return firstDeleted;

        return slot;
    }

    void _resize(size_t newCapacity)
    {
        std::vector<HashMapEntry<TKey, TValue> > oldEntries = m_entries;

        m_entries.clear();
        m_entries.resize(newCapacity);
        m_size = 0;

        for (size_t i = 0; i < oldEntries.size(); i++)
        {
            if (oldEntries[i].bOccupied && !oldEntries[i].bDeleted)
                insert(oldEntries[i].pair.first, oldEntries[i].pair.second);
        }
    }

    bool _keysEqual(const TKey& a, const TKey& b) const { return a == b; }

public:
    HashMap() : m_size(0)
    {
        _resize(DEFAULT_CAPACITY);
    }

    HashMap(size_t initialCapacity) :
        m_size(0)
    {
        _resize(initialCapacity);
    }
    
    bool insert(const TKey& key, const TValue& value)
    {
        if ((m_size * 100) / capacity() >= MAX_LOAD_FACTOR_PERCENT)
            _resize(capacity() * 2);

        size_t slot = _findSlotForInsert(key);

        if (m_entries[slot].bOccupied && !m_entries[slot].bDeleted)
        {
            m_entries[slot].pair.second = value;
            return false;
        }

        m_entries[slot].pair.first = key;
        m_entries[slot].pair.second = value;
        m_entries[slot].bOccupied = true;
        m_entries[slot].bDeleted = false;
        m_size++;

        return true;
    }

    bool remove(const TKey& key)
    {
        return erase(find(key)) != end();
    }
    
    TValue* get(const TKey& key)
    {
        if (m_size == 0)
            return NULL;

        size_t slot = _findSlot(key);

        if (slot == capacity())
            return NULL;

        return &m_entries[slot].pair.second;
    }

    const TValue* get(const TKey& key) const
    {
        if (m_size == 0)
            return NULL;

        size_t slot = _findSlot(key);

        if (slot == capacity())
            return NULL;

        return &m_entries[slot].pair.second;
    }
    
    TValue& operator[](const TKey& key)
    {
        TValue* pValue = get(key);
        if (pValue)
            return *pValue;

        insert(key, TValue());
        return *get(key);
    }
    
    void clear()
    {
        for (size_t i = 0; i < capacity(); ++i)
        {
            m_entries[i] = HashMapEntry<TKey, TValue>();
        }
        m_size = 0;
    }

    size_t size() const { return m_size; }
    size_t capacity() const { return m_entries.size(); }
    bool empty() const { return m_size == 0; }
    
    class iterator
    {
    private:
        void _findNextOccupied()
        {
            while (m_index < m_entries->size() && (!m_entries->at(m_index).bOccupied || m_entries->at(m_index).bDeleted))
                m_index++;
        }

    public:
        iterator(std::vector<HashMapEntry<TKey, TValue> >* entries, size_t index) :
            m_entries(entries),
            m_index(index)
        {
            if (m_index < m_entries->size() && (!m_entries->at(m_index).bOccupied || m_entries->at(m_index).bDeleted))
                _findNextOccupied();
        }

        iterator& operator++()
        {
            m_index++;
            _findNextOccupied();
            return *this;
        }

        iterator operator++(int)
        {
            iterator it = *this;
            ++(*this);
            return it;
        }

        bool operator==(const iterator& other) const { return m_entries == other.m_entries && m_index == other.m_index; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
        
        TKey& key() { return m_entries->at(m_index).pair.first; }
        TValue& value() { return m_entries->at(m_index).pair.second; }
        std::pair<TKey, TValue>* operator->() { return &m_entries->at(m_index).pair; }
        
    private:
        std::vector<HashMapEntry<TKey, TValue> >* m_entries;
        size_t m_index;
        
        friend class HashMap;
    };
    
    iterator find(const TKey& key)
    {
        if (m_size == 0)
            return end();

        size_t slot = _findSlot(key);

        if (slot == capacity())
            return end();

        return iterator(&m_entries, slot);
    }

    iterator begin() { return iterator(&m_entries, 0); }
    iterator end() { return iterator(&m_entries, capacity()); }

    iterator erase(iterator it)
    {
        if (it == end())
            return it;

        size_t slot = it.m_index;
        if (slot < capacity() &&
            m_entries[slot].bOccupied &&
            !m_entries[slot].bDeleted)
        {
            m_entries[slot].pair = std::pair<TKey, TValue>();
            m_entries[slot].bDeleted = true;
            m_size--;
        }

        return ++it;
    }

    iterator erase(iterator first, iterator last)
    {
        while (first != last)
            first = erase(first);

        return first;
    }

    class const_iterator
    {
    private:
        void _findNextOccupied()
        {
            while (m_index < m_entries->size() && (!m_entries->at(m_index).bOccupied || m_entries->at(m_index).bDeleted))
                m_index++;
        }

    public:
        const_iterator(const std::vector<HashMapEntry<TKey, TValue> >* entries, size_t index) :
            m_entries(entries),
            m_index(index)
        {
            if (m_index < entries->size() && (!m_entries->at(m_index).bOccupied || m_entries->at(m_index).bDeleted))
                _findNextOccupied();
        }

        const_iterator& operator++()
        {
            m_index++;
            _findNextOccupied();
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator it = *this;
            ++(*this);
            return it;
        }

        bool operator==(const const_iterator& other) const { return m_entries == other.m_entries && m_index == other.m_index; }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }

        const TKey& key() { return m_entries->at(m_index).pair.first; }
        const TValue& value() { return m_entries->at(m_index).pair.second; }
        const std::pair<TKey, TValue>* operator->() { return &m_entries->at(m_index); }

    private:
        const std::vector<HashMapEntry<TKey, TValue> >* m_entries;
        size_t m_index;

        friend class HashMap;
    };

    const_iterator find(const TKey& key) const
    {
        if (m_size == 0)
            return end();

        size_t slot = _findSlot(key);

        if (slot == capacity())
            return end();

        return const_iterator(&m_entries, slot);
    }

    const_iterator begin() const { return const_iterator(&m_entries, 0); }
    const_iterator end() const { return const_iterator(&m_entries, capacity()); }

private:
    std::vector<HashMapEntry<TKey, TValue> > m_entries;
    size_t m_size;
    THash m_HashFunc;
    
    static const size_t DEFAULT_CAPACITY = 16;
    static const size_t MAX_LOAD_FACTOR_PERCENT = 75;
};