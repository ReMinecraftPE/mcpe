#pragma once

#include <vector>
#include <stdint.h>

#include "Tag.hpp"

class CompoundTag;
class ListTag;

class ListTagFloatAdder
{
public:
	ListTagFloatAdder() : m_tag(nullptr) {}
	ListTagFloatAdder(ListTag* tag) : m_tag(tag) {}
	ListTagFloatAdder(float f);

public:
	ListTagFloatAdder& operator()(float f);
	operator ListTag* () { return m_tag; }
	operator const ListTag* () const { return m_tag; }

private:
	ListTag* m_tag;
};

class ListTagInt32Adder
{
public:
	ListTagInt32Adder() : m_tag(nullptr) {}
	ListTagInt32Adder(ListTag* tag) : m_tag(tag) {}
	ListTagInt32Adder(int32_t i);

public:
	ListTagInt32Adder& operator()(int32_t i);
	operator ListTag* () { return m_tag; }
	operator const ListTag* () const { return m_tag; }

private:
	ListTag* m_tag;
};

class ListTag : public Tag
{
public:
    ListTag();
    ListTag(Tag::Type elementType);
	~ListTag();

public:
    Tag::Type getId() const override { return TAG_TYPE_LIST; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
    void add(Tag* tag);
	float getFloat(unsigned int index) const;
	int32_t getInt32(unsigned int index) const;
	const std::string& getString(unsigned int index) const;
	const CompoundTag* getCompound(unsigned int index) const;
	Tag* copy() const override;
	ListTag* copyList() const;
	void deleteChildren();

	const std::vector<Tag*>& rawView() const { return m_list; }

	bool operator==(const Tag& other) const override;
	operator ListTagFloatAdder() { return ListTagFloatAdder(this); }
	operator ListTagInt32Adder() { return ListTagInt32Adder(this); }

private:
    std::vector<Tag*> m_list;
    Tag::Type m_type;
};