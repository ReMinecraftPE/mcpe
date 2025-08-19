#pragma once

#include "Tag.hpp"

class Int32ArrayTag : public Tag
{
public:
    Int32ArrayTag();
	Int32ArrayTag(const TagMemoryChunk& data);

public:
    Tag::Type getId() const override { return TAG_TYPE_INT32_ARRAY; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    TagMemoryChunk m_data;
};
