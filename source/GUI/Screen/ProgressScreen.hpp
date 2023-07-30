#pragma once

#include "Screen.hpp"

class ProgressScreen : public Screen
{
public:
	void render(int, int, float) override;
	bool isInGameScreen() override;
};

