#pragma once

#include "GuiComponent.hpp"

class GuiElement : public GuiComponent
{
public:
	GuiElement();

public:
	virtual void setupPositions();
};

