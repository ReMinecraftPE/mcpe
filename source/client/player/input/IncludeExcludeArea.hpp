#pragma once

#include <vector>
#include "IArea.hpp"

class IncludeExcludeArea : public IArea
{
public:
	IncludeExcludeArea();
	~IncludeExcludeArea();
	void include(IArea*);
	void exclude(IArea*);
	void clear();

	bool isInside(float x, float y) override;

private:
	std::vector<IArea*> m_include;
	std::vector<IArea*> m_exclude;
};

