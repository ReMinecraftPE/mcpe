#pragma once

#include <stdint.h>

#include "Tag.hpp"

class Int8Tag : public Tag
{
public:
    Int8Tag();
    Int8Tag(int8_t val);

public:
    Tag::Type getId() const override { return TAG_TYPE_INT8; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    int8_t m_data;
};
