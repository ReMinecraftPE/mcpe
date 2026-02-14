#pragma once

#include <stdint.h>

class ByteMask
{
protected:
	uint8_t mask;

public:
	ByteMask()
		: mask(0)
	{
	}
public:
	void setEmpty()
	{
		mask = 0;
	}
	void setFull()
	{
		mask = 0xFF;
	}
	void add(const uint8_t bit)
	{
		mask |= bit;
	}
	bool contains(const uint8_t bit) const
	{
		return (mask & bit) != 0;
	}
	operator bool() const
	{
		return mask != 0;
	}
	uint8_t toByte() const
	{
		return mask;
	}
};
