/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "IngameBlockSelectionScreen.hpp"
#include "PauseScreen.hpp"
#include "ChatScreen.hpp"
//#include "CraftingScreen.hpp"
//#include "ArmorScreen.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"
#include "renderer/ShaderConstants.hpp"

std::string g_sNotAvailableInDemoVersion = "Not available in the demo version";

IngameBlockSelectionScreen::IngameBlockSelectionScreen() :
	m_btnPause(0, "\xF0"), // 3 lined bars
	m_btnChat(1, "\x01\x27"), // face and comma
	m_btnCraft(2, "Craft"),
	m_btnArmor(3, "Armor")
{
	m_bRenderPointer = true;
	m_selectedSlot = 0;
}

Inventory* IngameBlockSelectionScreen::getInventory()
{
	return m_pMinecraft->m_pLocalPlayer->m_pInventory;
}

int IngameBlockSelectionScreen::getBottomY()
{
	// -1 for some reason, -2 to make it align between top of screen and top of hotbar instead
	return (m_height - 22 * (getSlotsHeight() - 2)) / 2;
}

int IngameBlockSelectionScreen::getSelectedSlot(int x, int y)
{
	int slotsHeight = getSlotsHeight();
	int bottom = m_height - getBottomY();
	int top = bottom - slotsHeight * 22;
	int left = m_width / 2 - 87;

	if (y < top)
		return -1;
	if (x < left)
		return -1;

	int idx = (x - left) / 20;
	if (idx > 8)
		return -1;

	return idx + 9 * slotsHeight - 9 * ((y - top) / 22);
}

int IngameBlockSelectionScreen::getSlotPosX(int x)
{
	return m_width / 2 - 88 + 20 * x;
}

int IngameBlockSelectionScreen::getSlotPosY(int y)
{
	return m_height - getBottomY() - 22 * y;
}

int IngameBlockSelectionScreen::getSlotsHeight()
{
	return (getInventory()->getContainerSize() + 8) / 9;
}

bool IngameBlockSelectionScreen::isAllowed(int slot)
{
	return slot >= 0 && slot < getInventory()->getContainerSize();
}

bool IngameBlockSelectionScreen::isInsideSelectionArea(int x, int y)
{
	int slotsWidth = 9 * 20 + 2;
	int left = m_width / 2 - slotsWidth / 2;
	int right = left + slotsWidth;

	int top = getSlotPosY(getSlotsHeight() - 1);
	int bottom = getSlotPosY(0) + 22;

	return x >= left && x < right && y >= top && y < bottom;
}

void IngameBlockSelectionScreen::init()
{
	m_btnPause.m_width = 25;
	m_btnPause.m_xPos = m_width - m_btnPause.m_width / 1.05;
	m_btnPause.m_yPos = 0;
	if (m_pMinecraft->isTouchscreen())
	{
		_addElement(m_btnPause);
	}

	m_btnChat.m_width = 25;
	m_btnChat.m_xPos = 0;
	m_btnChat.m_yPos = 0;
	if (m_pMinecraft->isTouchscreen())
	{
		_addElement(m_btnChat);
	}
	
	/*m_btnCraft.m_width = 40;
	m_btnCraft.m_xPos = 0;
	m_btnCraft.m_yPos = 0;
	_addElement(m_btnCraft);*/

	/*m_btnArmor.m_width = 40;
	m_btnArmor.m_xPos = m_btnCraft.m_width;
	m_btnArmor.m_yPos = 0;
	_addElement(m_btnArmor);*/

	Inventory* pInv = getInventory();

	//int nItems = pInv->getContainerSize();

	//for (int i = 0; i < nItems; i++)
	//{
	//	ItemInstance& item = pInv->getItem(i);
	//	if (!item.isEmpty() && item.getId() == pInv->getSelectedItemId())
	//	{
	//		m_selectedSlot = i;
	//		break;
	//	}
	//}

	//if (!isAllowed(m_selectedSlot))
	//	m_selectedSlot = 0;

	m_selectedSlot = pInv->m_selectedSlot;
}

void IngameBlockSelectionScreen::renderSlot(int index, int x, int y, float f)
{
	ItemInstance* item = getInventory()->tryGetItem(index);
	if (!item || item->isEmpty())
		return;

	ItemRenderer::singleton().renderGuiItem(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, *item, x, y, true);
	ItemRenderer::singleton().renderGuiItemOverlay(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, *item, x, y);
}

void IngameBlockSelectionScreen::renderSlots()
{
	currentShaderColor = Color::WHITE;
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui.png");

	for (int y = 0; y != -22 * getSlotsHeight(); y -= 22)
		blit(m_width / 2 - 182 / 2, m_height - 3 - getBottomY() + y, 0, 0, 182, 22, 0, 0, &m_materials.ui_textured_and_glcolor);

	if (m_selectedSlot >= 0)
		blit(m_width / 2 - 92 + 20 * (m_selectedSlot % 9), m_height - 4 - getBottomY() - 22 * (m_selectedSlot / 9), 0, 22, 24, 22, 0, 0, &m_materials.ui_textured_and_glcolor);

	for (int y = 0, index = 0; y < getSlotsHeight(); y++)
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

	drawCenteredString(*m_pMinecraft->m_pFont, g_sNotAvailableInDemoVersion, x, y, 0xFFFFFFFF);
}

void IngameBlockSelectionScreen::render(float f)
{
	fill(0, 0, m_width, m_height, 0x80000000);

	renderSlots();

#ifdef DEMO
	renderDemoOverlay();
#endif

	Screen::render(f);
}

void IngameBlockSelectionScreen::_buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnPause.m_buttonId)
		m_pMinecraft->setScreen(new PauseScreen);

	if (pButton->m_buttonId == m_btnChat.m_buttonId)
        m_pMinecraft->setScreen(new ChatScreen(true));

		/*if (pButton->m_buttonId == m_btnCraft.m_buttonId)
		m_pMinecraft->setScreen(new CraftingScreen(m_pMinecraft->m_pLocalPlayer));*/

	/*if (pButton->m_buttonId == m_btnArmor.m_buttonId)
		m_pMinecraft->setScreen(new ArmorScreen(m_pMinecraft->m_pLocalPlayer));*/
}

void IngameBlockSelectionScreen::pointerPressed(int x, int y, MouseButtonType btn)
{
	Screen::pointerPressed(x, y, btn);

	if (btn != MOUSE_BUTTON_LEFT)
		return;

	m_bReleased = true;
	m_bClickedOnSlot = isInsideSelectionArea(x, y);

	int slot = getSelectedSlot(x, y);
	if (isAllowed(slot))
		m_selectedSlot = slot;
}

void IngameBlockSelectionScreen::pointerReleased(int x, int y, MouseButtonType btn)
{
	Screen::pointerReleased(x, y, btn);
	
	if (btn != MOUSE_BUTTON_LEFT)
		return;

	for (unsigned int i = 0; i < m_elements.size(); i++)
	{
		GuiElement* element = _getInternalElement(i);
		if (element->getType() != GuiElement::TYPE_BUTTON)
			continue;

		Button* btn = (Button*)element;
		if (btn->clicked(m_pMinecraft, x, y))
			return;
	}
	
	if (isInsideSelectionArea(x, y))
	{
		int slot = getSelectedSlot(x, y);
		if (isAllowed(slot) && slot == m_selectedSlot)
			selectSlotAndClose();
		return;
	}

	if (m_bReleased && !m_bClickedOnSlot)
	{
		m_pMinecraft->setScreen(nullptr);
	}

	int slot = getSelectedSlot(x, y);
	if (isAllowed(slot) && slot == m_selectedSlot)
		selectSlotAndClose();
}

void IngameBlockSelectionScreen::removed()
{
	m_pMinecraft->m_pGui->inventoryUpdated();
}

void IngameBlockSelectionScreen::selectSlotAndClose()
{
	Inventory* pInv = getInventory();
	
	pInv->swapItems(m_selectedSlot, pInv->m_selectedSlot);

	m_pMinecraft->m_pSoundEngine->playUI("random.click");
	m_pMinecraft->setScreen(nullptr);
}
