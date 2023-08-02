#pragma once

#include "Screen.hpp"

#ifdef ENH_IMPROVED_SAVING

class SavingWorldScreen : public Screen
{
public:
	SavingWorldScreen(bool bCopyMap);
	
	void render(int mouseX, int mouseY, float f) override;
	void tick() override;

public:
	bool m_bCopyMapAtEnd;
	int m_timer = 0;
};

#endif
