#pragma once

#include <vector>
#include <stdint.h>

#include "Tag.hpp"

class CompoundTag;

class ListTag : public Tag
{
public:
    ListTag();
    ListTag(Tag::Type elementType);

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

	bool operator==(const Tag& other) const override;

private:
    std::vector<Tag*> m_list;
    Tag::Type m_type;
};
