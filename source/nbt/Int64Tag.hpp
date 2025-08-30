#pragma once

#include <stdint.h>

#include "Tag.hpp"

class Int64Tag : public Tag
{
public:
    Int64Tag();
    Int64Tag(int64_t val);

public:
    Tag::Type getId() const override { return TAG_TYPE_INT64; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    int64_t m_data;
};
