#pragma once

#include "Screen.hpp"

class ChatScreen : public Screen
{
public:
	void buttonClicked(Button*) override;
	void init() override;
	void render(int mouseX, int mouseY, float f) override;
};

