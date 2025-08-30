#pragma once

#include <string>

#include "Tag.hpp"

class StringTag : public Tag
{
public:
    StringTag();
    StringTag(const std::string& value);

public:
    Tag::Type getId() const override { return TAG_TYPE_STRING; }

public:
    void write(IDataOutput& dos) const override;
    void load(IDataInput& dis) override;

	std::string toString() const override;
	Tag* copy() const override;
	
	bool operator==(const Tag& other) const override;

public:
    std::string m_data;
};
