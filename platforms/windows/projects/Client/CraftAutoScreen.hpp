
#pragma once

#include "../Screen.hpp"

class CraftAutoScreen : public Screen
{
public:
	virtual void init() override;
	virtual void render(int x, int y, float f) override;
	virtual void buttonClicked(Button*) override;
	virtual void mouseClicked(int x, int y, int type) override;
	virtual void mouseReleased(int x, int y, int type) override;
	virtual void removed() override;

private:
};

