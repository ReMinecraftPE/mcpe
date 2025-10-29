#pragma once

#include <map>

#include "Tag.hpp"
#include "Int8ArrayTag.hpp"
#include "Int32ArrayTag.hpp"
#include "Int64ArrayTag.hpp"
#include "ListTag.hpp"
#include "Int8Tag.hpp"
#include "Int32Tag.hpp"
#include "Int64Tag.hpp"
#include "Int16Tag.hpp"
#include "StringTag.hpp"
#include "FloatTag.hpp"
#include "DoubleTag.hpp"

class CompoundTag : public Tag
{
public:
    typedef std::map<std::string, Tag*> NamedTagMap;

public:
    CompoundTag();

public:
    Tag::Type getId() const override { return TAG_TYPE_COMPOUND; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;
    
    void put(const std::string& name, Tag* tag);
    void putInt8(const std::string& name, int8_t value);
    void putInt16(const std::string& name, int16_t value);
    void putInt32(const std::string& name, int32_t value);
    void putInt64(const std::string& name, int64_t value);
    void putFloat(const std::string& name, float value);
    void putDouble(const std::string& name, double value);
    void putString(const std::string& name, const std::string& value);
    void putInt8Array(const std::string& name, const TagMemoryChunk& mem);
    void putInt32Array(const std::string& name, const TagMemoryChunk& mem);
    void putInt64Array(const std::string& name, const TagMemoryChunk& mem);
	void putCompound(const std::string& name, CompoundTag* value);
    void putBoolean(const std::string& name, bool value);

	//void append(const CompoundTag& tag); // not in 0.12.1
    bool contains(const std::string& name) const;
    bool contains(const std::string& name, Tag::Type type) const;

    const Tag* get(const std::string& name) const;
    Tag* get(const std::string& name);
	int8_t getInt8(const std::string& name) const;
    int16_t getInt16(const std::string& name) const;
    int32_t getInt32(const std::string& name) const;
    int64_t getInt64(const std::string& name) const;
    float getFloat(const std::string& name) const;
    double getDouble(const std::string& name) const;
    const std::string& getString(const std::string& name) const;
    const TagMemoryChunk* getInt8Array(const std::string& name) const;
    const TagMemoryChunk* getInt32Array(const std::string& name) const;
    const TagMemoryChunk* getInt64Array(const std::string& name) const;
    CompoundTag* getCompound(const std::string& name);
    const CompoundTag* getCompound(const std::string& name) const;
    const ListTag* getList(const std::string& name) const;
    bool getBoolean(const std::string& name) const;

	std::string toString() const override;
	CompoundTag* copy() const override;
	CompoundTag clone() const;
    CompoundTag* uniqueClone() const;
	bool remove(const std::string& name);
    void deleteChildren() override;
    void leak() { m_bLeak = true; }

	bool isEmpty() const { return m_tags.empty(); }
    NamedTagMap& rawView() { return m_tags; }
    const NamedTagMap& rawView() const { return m_tags; }
	
	bool operator==(const Tag& other) const override;
	bool operator!=(const Tag& other) const { return !(*this == other); }

private:
    NamedTagMap m_tags;
    bool m_bLeak;
};