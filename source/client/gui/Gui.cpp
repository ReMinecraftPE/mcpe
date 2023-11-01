/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "client/app/Minecraft.hpp"
#include "client/gui/screens/IngameBlockSelectionScreen.hpp"
#include "client/gui/screens/ChatScreen.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif

#ifdef ENH_USE_GUI_SCALE_2
float Gui::InvGuiScale = 1.0f / 2.0f;
#else
float Gui::InvGuiScale = 1.0f / 3.0f;
#endif

Gui::Gui(Minecraft* pMinecraft)
{
	field_8 = 0;
	field_C = "";
	field_24 = 0;
	field_28 = 0;
	field_2C = 0;
	field_9FC = 0;
	field_A00 = "";
	field_A18 = 0;
	field_A1C = false;
	field_A20 = 1.0f;
	field_A3C = true;
	m_bRenderMessages = true;

	m_pMinecraft = pMinecraft;

	xglGenBuffers(1, &m_renderChunk.field_0);
}

void Gui::addMessage(const std::string& s)
{
	// if the message contains a new line, add each line separately:
	if (s.find("\n") != std::string::npos)
	{
		std::stringstream ss(s);
		std::string line;
		while (std::getline(ss, line))
			addMessage(line);

		return;
	}

	std::string str = s;

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
	t.vertexUV(0.0f, a4,   -90.0f, 0.0f, 1.0f);
	t.vertexUV(a3,   a4,   -90.0f, 1.0f, 1.0f);
	t.vertexUV(a3,   0.0f, -90.0f, 1.0f, 0.0f);
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

	bool isTouchscreen = m->isTouchscreen();

	field_4 = -90.0f;

#ifndef ENH_TRANSPARENT_HOTBAR
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
#else
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
#endif

	m->m_pTextures->loadAndBindTexture("gui/gui.png");

	Inventory* pInventory = m->m_pLocalPlayer->m_pInventory;

	field_4 = -90.0f;

	int width  = Minecraft::width  * InvGuiScale,
		height = Minecraft::height * InvGuiScale;

#ifdef ENH_TRANSPARENT_HOTBAR
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

	int nSlots = getNumSlots();
	int hotbarWidth = 2 + nSlots * 20;

	// hotbar
	int cenX = width / 2;
	blit(cenX - hotbarWidth / 2, height - 22, 0, 0, hotbarWidth, 22, 0, 0);

	// selection mark
	blit(cenX - 1 - hotbarWidth / 2 + 20 * pInventory->m_SelectedHotbarSlot, height - 23, 0, 22, 24, 22, 0, 0);

	m->m_pTextures->loadAndBindTexture("gui/icons.png");

	if (!isTouchscreen)
	{
#ifndef ENH_TRANSPARENT_HOTBAR
		glEnable(GL_BLEND);
#endif
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
		blit(cenX - 8, height / 2 - 8, 0, 0, 16, 16, 0, 0);
#ifndef ENH_TRANSPARENT_HOTBAR
		glDisable(GL_BLEND);
#endif
	}
	else
	{
		// if needed, draw feedback
		
		// NOTE: real Minecraft PE takes it directly from the gamemode as "current progress" and
		// "last progress". Well guess what? The game mode in question updates our field_8 with
		// the pre-interpolated break progress! Isn't that awesome?!
		float breakProgress = field_8;

		// don't know about this if-structure, it feels like it'd be like
		// if (field_C >= 0.0f && breakProgress <= 0.0f)
		//     that;
		// else
		//     this;
		if (breakProgress > 0.0f || m_pMinecraft->m_pInputHolder->m_feedbackAlpha < 0.0f)
		{
			if (breakProgress > 0.0f)
			{
				float xPos = m_pMinecraft->m_pInputHolder->m_feedbackX;
				float yPos = m_pMinecraft->m_pInputHolder->m_feedbackY;

				m_pMinecraft->m_pTextures->loadAndBindTexture("gui/feedback_outer.png");
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				blit(InvGuiScale * xPos - 44.0f, InvGuiScale * yPos - 44.0f, 0, 0, 88, 88, 256, 256);

				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
				m_pMinecraft->m_pTextures->loadAndBindTexture("gui/feedback_fill.png");

				// note: scale starts from 4.0f
				float halfWidth = (40.0f * breakProgress + 48.0f) / 2.0f;

				blit(InvGuiScale * xPos - halfWidth, InvGuiScale * yPos - halfWidth, 0, 0, halfWidth * 2, halfWidth * 2, 256, 256);

				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glDisable(GL_BLEND);
			}
		}
		else
		{
			float xPos = m_pMinecraft->m_pInputHolder->m_feedbackX;
			float yPos = m_pMinecraft->m_pInputHolder->m_feedbackY;

			m_pMinecraft->m_pTextures->loadAndBindTexture("gui/feedback_outer.png");
			glColor4f(1.0f, 1.0f, 1.0f, Mth::Min(1.0f, m_pMinecraft->m_pInputHolder->m_feedbackAlpha));
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			blit(InvGuiScale * xPos - 44.0f, InvGuiScale * yPos - 44.0f, 0, 0, 88, 88, 256, 256);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glDisable(GL_BLEND);
		}
	}

#ifdef ENH_TRANSPARENT_HOTBAR
	glDisable(GL_BLEND);
#endif

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
			int breathFull  = int(ceilf((float(breathRaw - 2) * 10.0f) / 300.0f));
			int breathMeter = int(ceilf((float(breathRaw)     * 10.0f) / 300.0f)) - breathFull;

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

	int diff = m->isTouchscreen();

	int slotX = cenX - hotbarWidth / 2 + 3;
	for (int i = 0; i < nSlots - diff; i++)
	{
		renderSlot(i, slotX, height - 19, f);

		slotX += 20;
	}
	
	slotX = cenX - hotbarWidth / 2 + 3;
	for (int i = 0; i < nSlots - diff; i++)
	{
		renderSlotOverlay(i, slotX, height - 19, f);

		slotX += 20;
	}

#undef DIFF

	field_A3C = false;

	// blit the "more items" button
	if (m->isTouchscreen())
	{
		m->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		blit(cenX + hotbarWidth / 2 - 19, height - 19, 208, 208, 16, 16, 0, 0);
	}

	// render messages
	if (m_bRenderMessages)
	{
		renderMessages(false);
	}
}

void Gui::tick()
{
	if (field_A18 > 0)
		field_A18--;

	field_9FC++;

	for (int i = 0; i < int(m_guiMessages.size()); i++)
	{
		GuiMessage& msg = m_guiMessages[i];
		msg.field_18++;
	}
}

void Gui::renderSlot(int slot, int x, int y, float f)
{
	Inventory* pInv = m_pMinecraft->m_pLocalPlayer->m_pInventory;

	ItemInstance* pInst = pInv->getQuickSlotItem(slot);
	if (!pInst)
		return;

	if (!pInst->m_itemID)
		return;

	ItemRenderer::renderGuiItem(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, pInst, x, y, true);
}

void Gui::renderSlotOverlay(int slot, int x, int y, float f)
{
	Inventory* pInv = m_pMinecraft->m_pLocalPlayer->m_pInventory;

	ItemInstance* pInst = pInv->getQuickSlotItem(slot);
	if (!pInst)
		return;

	if (!pInst->m_itemID)
		return;

	ItemRenderer::renderGuiItemOverlay(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, pInst, x, y);
}

int Gui::getSlotIdAt(int mouseX, int mouseY)
{
	int scaledY = int(InvGuiScale * mouseY);
	int scaledHeight = int(InvGuiScale * Minecraft::height);

	if (scaledY >= scaledHeight)
		return -1;
	if (scaledY < scaledHeight - 19)
		return -1;

	int hotbarOffset = getNumSlots() * 20 / 2 - 2;

	int slotX = (int(InvGuiScale * mouseX) - int(InvGuiScale * Minecraft::width) / 2 + hotbarOffset + 20) / 20;

	if (slotX >= 0)
		slotX--;

	if (slotX > getNumSlots())
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

	if (m_pMinecraft->isTouchscreen() && slot == getNumSlots() - 1)
		m_pMinecraft->setScreen(new IngameBlockSelectionScreen);
	else
		m_pMinecraft->m_pLocalPlayer->m_pInventory->selectSlot(slot);
}

void Gui::handleKeyPressed(int keyCode)
{
	if (m_pMinecraft->getOptions()->isKey(KM_INVENTORY, keyCode))
	{
		m_pMinecraft->setScreen(new IngameBlockSelectionScreen);
		return;
	}

	int maxItems = getNumSlots() - 1;
	if (m_pMinecraft->isTouchscreen())
		maxItems--;

	if (m_pMinecraft->getOptions()->isKey(KM_SLOT_R, keyCode))
	{
		int* slot = &m_pMinecraft->m_pLocalPlayer->m_pInventory->m_SelectedHotbarSlot;

		if (*slot <= maxItems)
			(*slot)++;

		return;
	}
	if (m_pMinecraft->getOptions()->isKey(KM_SLOT_L, keyCode))
	{
		int* slot = &m_pMinecraft->m_pLocalPlayer->m_pInventory->m_SelectedHotbarSlot;

		if (*slot > 0)
			(*slot)--;

		return;
	}

	if (m_pMinecraft->getOptions()->isKey(KM_CHAT_CMD, keyCode) || m_pMinecraft->getOptions()->isKey(KM_CHAT, keyCode))
	{
		if (m_pMinecraft->m_pScreen)
			return;

		m_pMinecraft->setScreen(new ChatScreen(m_pMinecraft->getOptions()->isKey(KM_CHAT_CMD, keyCode)));
		return;
	}
}

void Gui::renderMessages(bool bShowAll)
{
	//int width = Minecraft::width * InvGuiScale,
    int height = Minecraft::height * InvGuiScale;

	int topEdge = height - 49;
	
	for (int i = 0; i < int(m_guiMessages.size()); i++)
	{
		GuiMessage& msg = m_guiMessages[i];
		if (!bShowAll && msg.field_18 > 199)
			continue;

		int bkgdColor = 0x7F000000, textColor = 0xFFFFFFFF;

		float fade = 1.0f;
		
		if (!bShowAll)
		{
			fade = 10.0f * (1.0f - (float(msg.field_18) / 200.0f));
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
		}

		fill(2, topEdge, 322, topEdge + 9, bkgdColor);
		glEnable(GL_BLEND);
		m_pMinecraft->m_pFont->drawShadow(msg.msg, 2, topEdge + 1, textColor);

		topEdge -= 9;
	}

	glDisable(GL_BLEND);
}

int Gui::getNumSlots()
{
	if (m_pMinecraft->isTouchscreen())
		return 4;

	return 9;
}

RectangleArea Gui::getRectangleArea(bool b)
{
	float centerX = Minecraft::width / 2;
	float hotbarWidthHalf = (10 * getNumSlots() + 5) / InvGuiScale;
	return RectangleArea(
		b ? (centerX - hotbarWidthHalf) : 0,
		Minecraft::height - 24.0f / InvGuiScale,
		centerX + hotbarWidthHalf,
		Minecraft::height);
}
