/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiComponent.hpp"
#include "client/app/Minecraft.hpp"

class Screen;

class Button : public GuiComponent
{
private:
	void _init();
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
	int m_width;
	int m_height;
	int m_xPos;
	int m_yPos;
	std::string m_text;
	int m_buttonId;
	bool m_bEnabled;
	bool m_bVisible;
	bool m_bHovered;
	float m_bOpacity;

#ifndef ORIGINAL_CODE
	int m_lastX;
	int m_lastY;
#endif
};

