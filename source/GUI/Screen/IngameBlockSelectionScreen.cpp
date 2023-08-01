/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "IngameBlockSelectionScreen.hpp"
#include "ItemRenderer.hpp"

#define C_SLOTS_HEIGHT ((C_MAX_INVENTORY_ITEMS + 8) / 9)

std::string g_sNotAvailableInDemoVersion = "Not available in the demo version";

int IngameBlockSelectionScreen::getSelectedSlot(int x, int y)
{
	int bottom = m_height - 151;
	int left = m_width / 2 - 87;

	if (y < bottom)
		return -1;
	if (x < left)
		return -1;

	int idx = (x - left) / 20;
	if (idx > 8)
		return -1;

	return idx + 36 - 9 * ((y - bottom) / 22);
}

int IngameBlockSelectionScreen::getSlotPosX(int x)
{
	return m_width / 2 - 88 + 20 * x;
}

int IngameBlockSelectionScreen::getSlotPosY(int y)
{
	return m_height - 63 - 22 * y;
}

bool IngameBlockSelectionScreen::isAllowed(int slot)
{
	if (slot < 0 || slot > C_MAX_INVENTORY_ITEMS-1)
		return false;

#ifdef DEMO
	return slot > 17;
#endif
	return true;
}

void IngameBlockSelectionScreen::init()
{
	Inventory* pInv = m_pMinecraft->m_pLocalPlayer->m_pInventory;

	for (int i = 9; i < C_MAX_HOTBAR_ITEMS + C_MAX_INVENTORY_ITEMS; i++)
	{
		if (pInv->getSelectionSlotItemId(i) == pInv->getSelectedItemId())
		{
			m_selectedSlot = i - 9;
			break;
		}
	}

	if (!isAllowed(m_selectedSlot))
		m_selectedSlot = 27;
}

void IngameBlockSelectionScreen::renderSlot(int index, int x, int y, float f)
{
	int item = m_pMinecraft->m_pLocalPlayer->m_pInventory->getSelectionSlotItemId(index);
	if (item < 0)
		return;

	ItemInstance inst(item, 2, 0);
	ItemRenderer::renderGuiItem(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, &inst, x, y, item);
}

void IngameBlockSelectionScreen::renderSlots()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui.png");

	for (int y = 0; y != -22 * C_SLOTS_HEIGHT; y -= 22)
		blit(m_width / 2 - 182 / 2, m_height - 66 + y, 0, 0, 182, 22, 0, 0);

	if (m_selectedSlot >= 0)
		blit(m_width / 2 - 92 + 20 * (m_selectedSlot % 9), m_height - 67 - 22 * (m_selectedSlot / 9), 0, 22, 24, 22, 0, 0);

	for (int y = 0, index = 9; y < C_SLOTS_HEIGHT; y++)
	{
		int posY = getSlotPosY(y);
		for (int x = 0; x < 9; x++)
		{
			int posX = getSlotPosX(x);
			renderSlot(index++, posX, posY, 0.0f);
		}
	}
}

void IngameBlockSelectionScreen::renderDemoOverlay()
{
	fill(getSlotPosX(0) - 3, getSlotPosY(1) - 3, getSlotPosX(9), getSlotPosY(-1), 0xA0000000);
	
	int x = (getSlotPosX(4) + getSlotPosX(5)) / 2;
	int y = (getSlotPosY(0) + getSlotPosY(1)) / 2 + 5;

	drawCenteredString(m_pMinecraft->m_pFont, g_sNotAvailableInDemoVersion, x, y, 0xFFFFFFFF);
}

void IngameBlockSelectionScreen::render(int x, int y, float f)
{
	Screen::render(x, y, f);
	glDisable(GL_DEPTH_TEST);

	fill(0, 0, m_width, m_height, 0x80000000);

	glEnable(GL_BLEND);
	renderSlots();

#ifdef DEMO
	renderDemoOverlay();
#endif

	glEnable(GL_DEPTH_TEST);
}

void IngameBlockSelectionScreen::mouseClicked(int x, int y, int type)
{
	// not a left click
	if (type != 1)
		return;

	int slot = getSelectedSlot(x, y);
	if (isAllowed(slot))
		m_selectedSlot = slot;
}

void IngameBlockSelectionScreen::mouseReleased(int x, int y, int type)
{
	// not a left click
	if (type != 1)
		return;

	int slot = getSelectedSlot(x, y);
	if (isAllowed(slot) && slot == m_selectedSlot)
		selectSlotAndClose();
}

void IngameBlockSelectionScreen::selectSlotAndClose()
{
	Inventory* pInv = m_pMinecraft->m_pLocalPlayer->m_pInventory;
	int item = pInv->getSelectionSlotItemId(m_selectedSlot + 9);
	int idx = 0;

	// @TODO: Fix gotos
#ifdef ENH_ENABLE_9TH_SLOT
#define MAX_ITEMS (C_MAX_HOTBAR_ITEMS - 1)
#else
#define MAX_ITEMS (C_MAX_HOTBAR_ITEMS - 2)
#endif

	if (item == pInv->getSelectionSlotItemId(0))
	{
	label_4:
		if (!idx)
			goto label_5;
	}
	else while (++idx != MAX_ITEMS)
	{
		if (item == pInv->getSelectionSlotItemId(idx))
			goto label_4;
	}

	while (true)
	{
		int item = pInv->getSelectionSlotItemId(idx - 1);
		pInv->setSelectionSlotItemId(idx, item);

		if (idx == 1)
			break;

		--idx;
	}
label_5:
	pInv->setSelectionSlotItemId(0, item);
	pInv->selectSlot(0);

	m_pMinecraft->m_pSoundEngine->play("random.click");

	m_pMinecraft->setScreen(nullptr);
}
