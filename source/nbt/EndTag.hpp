#pragma once

#include "Tag.hpp"

class EndTag : public Tag
{
public:
    EndTag();

public:
    Tag::Type getId() const override { return TAG_TYPE_END; }

	void write(IDataOutput& dos) const override {};
	void load(IDataInput& dis) override {};

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;
};