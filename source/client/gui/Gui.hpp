/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "GuiComponent.hpp"
#include "client/app/Minecraft.hpp"
#include "common/Random.hpp"
#include "common/Utils.hpp"

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
	void renderSlotOverlay(int slot, int x, int y, float f);
	int  getSlotIdAt(int mx, int my);
	bool isInside(int mx, int my);
	void handleClick(int id, int mx, int my);
	void handleKeyPressed(int keyCode);
	void renderMessages(bool bShowAll);

public:
	static float InvGuiScale;

public:
	float field_8;
	std::string field_C;
	std::vector<GuiMessage> m_guiMessages;
	int field_24;
	int field_28;
	int field_2C;
	Random m_random;
	Minecraft* m_pMinecraft;
	int field_9FC;
	std::string field_A00;
	int field_A18;
	bool field_A1C;
	float field_A20;
	RenderChunk m_renderChunk;
	bool field_A3C;
	bool m_bRenderMessages;
};

