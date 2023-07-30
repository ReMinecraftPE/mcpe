/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "GuiComponent.hpp"
#include "Minecraft.hpp"
#include "Random.hpp"
#include "Utils.hpp"

class Minecraft; // in case we're included from Minecraft.hpp

struct GuiMessage
{
	std::string msg;
	int field_18;

	GuiMessage(const std::string& x, int a) : msg(x), field_18(a) {}
};

class Gui : public GuiComponent
{
public:
	Gui(Minecraft* pMinecraft);

	void addMessage(const std::string& str);
	void inventoryUpdated();
	void renderVignette(float, int, int);
	void setNowPlaying(const std::string& str);
	void render(float f, bool bHaveScreen, int mouseX, int mouseY);
	void tick();
	void renderSlot(int slot, int x, int y, float f);
	int  getSlotIdAt(int mx, int my);
	bool isInside(int mx, int my);
	void handleClick(int id, int mx, int my);
	void handleKeyPressed(int keyCode);

public:
	static float InvGuiScale;

public:
	float field_8 = 0;
	std::string field_C = "";
	std::vector<GuiMessage> m_guiMessages;
	int field_24 = 0;
	int field_28 = 0;
	int field_2C = 0;
	Random m_random;
	Minecraft* m_pMinecraft;
	int field_9FC = 0;
	std::string field_A00 = "";
	int field_A18 = 0;
	bool field_A1C = false;
	float field_A20 = 1.0f;
	RenderChunk m_renderChunk;
	bool field_A3C = true;
};

