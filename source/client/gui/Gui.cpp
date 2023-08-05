/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Minecraft.hpp"
#include "IngameBlockSelectionScreen.hpp"
#include "ChatScreen.hpp"
#include "ItemRenderer.hpp"
#include <Windows.h>
#include <unordered_map>
#include <iostream>
#include <corecrt_wstring.h>
#include <string>
#include <fstream>
#include <locale>   // For wstring_convert
#include <codecvt> 
#include <io.h>
#include <fcntl.h> 
#ifdef _WIN32
#pragma warning(disable : 4244)
#endif

#ifdef ENH_USE_GUI_SCALE_2
float Gui::InvGuiScale = 0.5f;
#else
float Gui::InvGuiScale = 0.333333f;
#endif






Gui::Gui(Minecraft* pMinecraft)
{
	m_pMinecraft = pMinecraft;

	xglGenBuffers(1, &m_renderChunk.field_0);
}

void Gui::addMessage(const std::string& s)
{
	
	std::string str = s;
	
	// Turkish characters for add turkish language support
	while (m_pMinecraft->m_pFont->width(str) > 320)
	{
		int i = 2;
		for (; i < int(str.size()); i++)
		{
			std::string sstr = str.substr(0, i);

			// this sucks
			if (m_pMinecraft->m_pFont->width(sstr) > 320)
				break;
		}

		std::string sstr = str.substr(0, i - 1);
		addMessage(sstr);
		str = str.substr(i - 1);
	}

	if (m_guiMessages.size() > 50)
	{
		m_guiMessages.erase(m_guiMessages.end());
	}
	
	m_guiMessages.insert(m_guiMessages.begin(), GuiMessage(str, 0));
}

void Gui::setNowPlaying(const std::string& str)
{
	field_A00 = "Now playing: " + str;
	field_A18 = 60;
	field_A1C = true;
}

void Gui::renderVignette(float a2, int a3, int a4)
{
	a2 = 1.0f - a2;
	if (a2 > 1.0f)
		a2 = 1.0f;
	if (a2 < 0.0f)
		a2 = 0.0f;

	field_A20 += ((a2 - field_A20) * 0.01f);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glColor4f(field_A20, field_A20, field_A20, 1.0f);

	//! @BUG: No misc/vignette.png to be found in the original.
	//! This function is unused anyways
	m_pMinecraft->m_pTextures->loadAndBindTexture("misc/vignette.png");

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.vertexUV(0.0f, a4, -90.0f, 0.0f, 1.0f);
	t.vertexUV(a3, a4, -90.0f, 1.0f, 1.0f);
	t.vertexUV(a3, 0.0f, -90.0f, 1.0f, 0.0f);
	t.vertexUV(0.0f, 0.0f, -90.0f, 0.0f, 0.0f);
	t.draw();

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Gui::inventoryUpdated()
{
	field_A3C = true;
}

void Gui::render(float f, bool bHaveScreen, int mouseX, int mouseY)
{
	Minecraft* m = m_pMinecraft;

	m->m_pGameRenderer->setupGuiScreen();

	if (!m->m_pLevel || !m->m_pLocalPlayer)
		return;

	field_4 = -90.0f;

#ifndef ENH_TRANSPARENT_HOTBAR
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
#else
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
#endif

	m->m_pTextures->loadAndBindTexture("gui/gui.png");

	Inventory* pInventory = m->m_pLocalPlayer->m_pInventory;

	field_4 = -90.0f;

	int width = Minecraft::width * InvGuiScale,
		height = Minecraft::height * InvGuiScale;

#ifdef ENH_TRANSPARENT_HOTBAR
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

	// hotbar
	int cenX = width / 2;
	blit(cenX - 182 / 2, height - 22, 0, 0, 182, 22, 0, 0);

	// selection mark
	blit(cenX - 92 + 20 * pInventory->m_SelectedHotbarSlot, height - 23, 0, 22, 24, 22, 0, 0);

	m->m_pTextures->loadAndBindTexture("gui/icons.png");

#ifndef ENH_TRANSPARENT_HOTBAR
	glEnable(GL_BLEND);
#endif

	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);

	// crosshair
	blit(cenX - 8, height / 2 - 8, 0, 0, 16, 16, 0, 0);

	glDisable(GL_BLEND);

	if (m_pMinecraft->m_pGameMode->canHurtPlayer())
	{
		LocalPlayer* pLP = m_pMinecraft->m_pLocalPlayer;

		// why??
		m_random.init_genrand(312871 * field_9FC);

		int emptyHeartX = 16;
		bool b1 = false;
		if (pLP->field_B8 < 10)
		{
			b1 = pLP->field_B8 / 3 % 2;
			emptyHeartX += 9 * b1;
		}

		// @NOTE: At the default scale, this would go off screen.

		int heartX = cenX - 191; // why?
		int heartYStart = height - 10;

		//@NOTE: Alpha-style health UI. I'll probably remove this on release.
#ifndef ORIGINAL_CODE
		heartX = cenX - 91;
		heartYStart = height - 32;
#endif

		int playerHealth = pLP->m_health;

		for (int healthNo = 1; healthNo <= C_MAX_MOB_HEALTH; healthNo += 2)
		{
			int heartY = heartYStart;

			if (playerHealth <= 4 && m_random.genrand_int32() % 2)
				heartY++;

			blit(heartX, heartY, emptyHeartX, 0, 9, 9, 0, 0);

			if (b1)
			{
				if (healthNo < pLP->field_100)
					blit(heartX, heartY, 70, 0, 9, 9, 0, 0);
				else if (healthNo == pLP->field_100)
					blit(heartX, heartY, 79, 0, 9, 9, 0, 0);
			}

			if (healthNo < playerHealth)
				blit(heartX, heartY, 52, 0, 9, 9, 0, 0);
			else if (healthNo == playerHealth)
				blit(heartX, heartY, 61, 0, 9, 9, 0, 0);

			heartX += 8;
		}

		if (m->m_pLocalPlayer->isUnderLiquid(Material::water))
		{
			int breathRaw = m->m_pLocalPlayer->field_BC;
			int breathFull = int(ceilf((float(breathRaw - 2) * 10.0f) / 300.0f));
			int breathMeter = int(ceilf((float(breathRaw) * 10.0f) / 300.0f)) - breathFull;

			int bubbleX = cenX - 191;
			int bubbleY = height - 19;

#ifndef ORIGINAL_CODE
			bubbleX = cenX - 91;
			bubbleY = height - 41;
#endif

			//@NOTE: Not sure this works as it should

			for (int bubbleNo = 0; bubbleNo < breathFull + breathMeter; bubbleNo++)
			{
				if (bubbleNo < breathFull)
					blit(bubbleX, bubbleY, 16, 18, 9, 9, 0, 0);
				else
					blit(bubbleX, bubbleY, 25, 18, 9, 9, 0, 0);

				bubbleX += 8;
			}
		}
	}

	m->m_pTextures->loadAndBindTexture("gui/gui_blocks.png");

	Tesselator& t = Tesselator::instance;
	t.begin();

	// if we aren't trying to build the real deal, don't do this. It only does worse
#ifdef ORIGINAL_CODE
	t.voidBeginAndEndCalls(true); // to ensure that Gui::renderSlot doesn't end() our tesselator right away?
#endif

	int slotX = cenX - 88;
#ifdef ENH_ENABLE_9TH_SLOT
#define HOTBAR_DIFF 0
#else
#define HOTBAR_DIFF 1
#endif
	for (int i = 0; i < C_MAX_HOTBAR_ITEMS - HOTBAR_DIFF; i++)
	{
		renderSlot(i, slotX, height - 19, f);

		slotX += 20;
	}
#undef HOTBAR_DIFF

	field_A3C = false;

#ifdef ORIGINAL_CODE
	t.voidBeginAndEndCalls(false);
#endif

	t.draw();

	// blit the "more items" button
#ifndef ENH_ENABLE_9TH_SLOT
	m->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
	blit(cenX + 72, height - 19, 208, 208, 16, 16, 0, 0);
#endif

	// render messages

	int topEdge = height - 49;

	for (auto& msg : m_guiMessages)
	{
		if (msg.field_18 > 199)
			continue;

		int bkgdColor = 0x7F000000, textColor = 0xFFFFFFFF;

		float fade = 10.0f * (1.0f - (float(msg.field_18) / 200.0f));
		if (fade <= 0.0f)
			continue;

		if (fade < 1.0f)
		{
			int x = int(fade * fade * 255.0f);
			if (x == 0)
				continue;

			bkgdColor = (x / 2) << 24;
			textColor = (x << 24) + 0xFFFFFF;
		}

		fill(2, topEdge, 322, topEdge + 9, bkgdColor);
		glEnable(GL_BLEND);
		m->m_pFont->drawShadow(msg.msg, 2, topEdge + 1, textColor);

		topEdge -= 9;
	}

	glDisable(GL_BLEND);
}

void Gui::tick()
{
	for (auto& msg : m_guiMessages)
	{
		msg.field_18++;
	}
}

void Gui::renderSlot(int slot, int x, int y, float f)
{
	int itemID = m_pMinecraft->m_pLocalPlayer->m_pInventory->getSelectionSlotItemId(slot);
	if (itemID < 0)
		return;

	ItemInstance inst(Item::items[itemID], 1, 0);
	ItemRenderer::renderGuiItem(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, &inst, x, y, true);
}

int Gui::getSlotIdAt(int mouseX, int mouseY)
{
	int scaledY = int(InvGuiScale * mouseY);
	int scaledHeight = int(InvGuiScale * Minecraft::height);

	if (scaledY >= scaledHeight)
		return -1;
	if (scaledY < scaledHeight - 19)
		return -1;

	int slotX = (int(InvGuiScale * mouseX) - int(InvGuiScale * Minecraft::width) / 2 + 88 + 20) / 20;

	//@NOTE: Why not just -88?
	if (slotX >= 0)
		slotX--;

	if (slotX > 8)
		slotX = -1;

	return slotX;
}

bool Gui::isInside(int mouseX, int mouseY)
{
	return getSlotIdAt(mouseX, mouseY) != -1;
}

void Gui::handleClick(int clickID, int mouseX, int mouseY)
{
	if (clickID != 1)
		return;

	int slot = getSlotIdAt(mouseX, mouseY);
	if (slot == -1)
		return;

#ifndef ENH_ENABLE_9TH_SLOT
	if (slot == 8)
	{
		m_pMinecraft->setScreen(new IngameBlockSelectionScreen);
	}
	else
#endif
	{
		m_pMinecraft->m_pLocalPlayer->m_pInventory->selectSlot(slot);
	}
}

void Gui::handleKeyPressed(int keyCode)
{
	
	switch (keyCode)
	{
	case AKEYCODE_BUTTON_Y:
	{
		m_pMinecraft->setScreen(new IngameBlockSelectionScreen);
		break;
	}

	case AKEYCODE_BUTTON_T:
	{
		m_pMinecraft->setScreen(new ChatScreen);
		break;
	
	}
	case AKEYCODE_BACK:
	{
		int* slot = &m_pMinecraft->m_pLocalPlayer->m_pInventory->m_SelectedHotbarSlot;

#ifdef ENH_ENABLE_9TH_SLOT
#define MAX_ITEMS (C_MAX_HOTBAR_ITEMS - 2)
#else
#define MAX_ITEMS (C_MAX_HOTBAR_ITEMS - 3)
#endif
		//@HUH: for whatever reason, it ignores the 7th item
		if (*slot <= MAX_ITEMS)
			(*slot)++;

		break;
	}

	case AKEYCODE_BUTTON_X:
	{
		int* slot = &m_pMinecraft->m_pLocalPlayer->m_pInventory->m_SelectedHotbarSlot;

		if (*slot > 0)
			(*slot)--;

		break;
	}
	}
}
