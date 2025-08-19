#pragma once

#include <stdint.h>

#include "Tag.hpp"

class Int32Tag : public Tag
{
public:
    Int32Tag();
    Int32Tag(int32_t val);

public:
    Tag::Type getId() const override { return TAG_TYPE_INT32; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    int32_t m_data;
};
