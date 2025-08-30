#pragma once

#include "Tag.hpp"

class Int64ArrayTag : public Tag
{
public:
    Int64ArrayTag();
	Int64ArrayTag(const TagMemoryChunk& data);

public:
    Tag::Type getId() const override { return TAG_TYPE_INT64_ARRAY; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    TagMemoryChunk m_data;
};
