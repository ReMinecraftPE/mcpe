#pragma once

class IArea
{
public:
	IArea() : field_4(true) {}
	virtual bool isInside(float, float) = 0;

protected:
	friend class IncludeExcludeArea;

	bool field_4;
};

