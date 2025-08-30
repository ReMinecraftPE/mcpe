#pragma once

#include <stdint.h>

#include "Tag.hpp"

class Int16Tag : public Tag
{
public:
    Int16Tag();
    Int16Tag(int16_t val);

public:
    Tag::Type getId() const override { return TAG_TYPE_INT16; }
    
	void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    int16_t m_data;
};
