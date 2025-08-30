#pragma once

#include <stdint.h>

#include "Tag.hpp"

class DoubleTag : public Tag
{
public:
    DoubleTag();
    DoubleTag(double val);
	
public:
    Tag::Type getId() const override { return TAG_TYPE_DOUBLE; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    double m_data;
};
