/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiElement.hpp"
#include "client/app/Minecraft.hpp"

class Screen;

class Button : public GuiElement
{
private:
	void _init();
public:
	Button(int x, int y, int width, int height, const std::string&);
	Button(int x, int y, const std::string&);
	Button(const std::string&);

public:
	Type getType() const override { return TYPE_BUTTON; }

public:
	virtual int  getYImage(bool bHovered);
	virtual void renderBg(Minecraft*, const MenuPointer& pointer);
	void render(Minecraft* pMinecraft, const MenuPointer& pointer) override;

public:
	Color m_color;

#ifndef ORIGINAL_CODE
	int m_lastX;
	int m_lastY;
#endif
};

