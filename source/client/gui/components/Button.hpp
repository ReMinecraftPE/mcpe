/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiComponent.hpp"
#include "Minecraft.hpp"

class Screen;

class Button : public GuiComponent
{
public:
	Button(int, int x, int y, int width, int height, const std::string&);
	Button(int, int x, int y, const std::string&);
	Button(int, const std::string&);

	// I can't possibly explain why Minecraft is referenced here
	bool clicked(Minecraft*, int xPos, int yPos);
	int  getYImage(bool bHovered);
	void released(int xPos, int yPos);
	void renderBg(Minecraft*, int, int);
	void render(Minecraft*, int xPos, int yPos);

public:
	int m_width = 0;
	int m_height = 0;
	int m_xPos = 0;
	int m_yPos = 0;
	std::string m_text = "";
	int m_buttonId;
	bool m_bEnabled = true;
	bool m_bVisible = true;
	bool field_36 = false;

#ifndef ORIGINAL_CODE
	int m_lastX = 0;
	int m_lastY = 0;
#endif
};

