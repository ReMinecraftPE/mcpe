#pragma once

#include "Tag.hpp"

class FloatTag : public Tag
{
public:
    FloatTag();
    FloatTag(float val);

public:
    Tag::Type getId() const override { return TAG_TYPE_FLOAT; }

    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    float m_data;
};
