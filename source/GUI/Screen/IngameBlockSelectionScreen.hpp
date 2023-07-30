#pragma once

#include "Screen.hpp"

class IngameBlockSelectionScreen : public Screen
{
public:
	int getSelectedSlot(int x, int y);
	int getSlotPosX(int x);
	int getSlotPosY(int y);
	bool isAllowed(int slot);
	void renderSlots();
	void renderDemoOverlay();
	void renderSlot(int index, int x, int y, float f);
	void selectSlotAndClose();

	virtual void init() override;
	virtual void render(int x, int y, float f) override;
	virtual void mouseClicked(int x, int y, int type) override;
	virtual void mouseReleased(int x, int y, int type) override;

private:
	int m_selectedSlot = 0;
};

