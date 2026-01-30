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

	// When we've got a proper creative inventory, use this method for aux tiles/items
	//for (int i = 0; i < 16; i++) // <-- This is an example for all wool colors in order

	// Original list of items.
	addCreativeItem(Tile::cobblestone->m_ID);
	addCreativeItem(Tile::brick->m_ID);
	addCreativeItem(Tile::sandStone->m_ID);
	addCreativeItem(Tile::wood->m_ID);
	addCreativeItem(Tile::treeTrunk->m_ID, 0);
	addCreativeItem(Tile::goldBlock->m_ID);
	addCreativeItem(Tile::ironBlock->m_ID);
	addCreativeItem(Tile::diamondBlock->m_ID);
	addCreativeItem(Tile::brick->m_ID);
	addCreativeItem(Tile::leaves->m_ID, 0);
	addCreativeItem(Tile::cloth->m_ID, 14);
	addCreativeItem(Tile::cloth->m_ID, 13);
	addCreativeItem(Tile::cloth->m_ID, 12);
	addCreativeItem(Tile::cloth->m_ID, 11);
	addCreativeItem(Tile::cloth->m_ID, 10);
	addCreativeItem(Tile::cloth->m_ID, 9);
	addCreativeItem(Tile::cloth->m_ID, 8);
	addCreativeItem(Tile::glass->m_ID);
	addCreativeItem(Tile::cloth->m_ID, 7);
	addCreativeItem(Tile::cloth->m_ID, 6);
	addCreativeItem(Tile::cloth->m_ID, 5);
	addCreativeItem(Tile::cloth->m_ID, 4);
	addCreativeItem(Tile::cloth->m_ID, 3);
	addCreativeItem(Tile::stairsWood->m_ID);
	addCreativeItem(Tile::stairsStone->m_ID);
	addCreativeItem(Tile::stoneSlabHalf->m_ID, 0);
	addCreativeItem(Tile::sand->m_ID);
	addCreativeItem(Tile::ladder->m_ID);
	addCreativeItem(Tile::torch->m_ID);
	addCreativeItem(Tile::flower->m_ID);
	addCreativeItem(Tile::rose->m_ID);
	addCreativeItem(Tile::mushroom1->m_ID);
	addCreativeItem(Tile::mushroom2->m_ID);
	addCreativeItem(Tile::reeds->m_ID);
	addCreativeItem(Tile::obsidian->m_ID);
	addCreativeItem(Tile::dirt->m_ID);

	// New items that weren't in the inventory before.
	addCreativeItem(Tile::grass->m_ID);
	addCreativeItem(Tile::tnt->m_ID);
	addCreativeItem(Tile::gravel->m_ID);
	addCreativeItem(Tile::cloth->m_ID, 15);
	addCreativeItem(Tile::mossStone->m_ID);
	addCreativeItem(Tile::bookshelf->m_ID);
	addCreativeItem(Tile::lapisBlock->m_ID);
	addCreativeItem(Tile::sponge->m_ID);
	addCreativeItem(Tile::sapling->m_ID);
	addCreativeItem(Tile::cryingObsidian->m_ID);
	addCreativeItem(Tile::rocketLauncher->m_ID);
	addCreativeItem(Tile::redstoneOre->m_ID);

	// test stuff
	addCreativeItem(Tile::water->m_ID);
	addCreativeItem(Tile::lava->m_ID);
	addCreativeItem(Tile::fire->m_ID);

	// items
	addCreativeItem(Item::camera->m_itemID);
	addCreativeItem(Item::woodDoor->m_itemID);
	addCreativeItem(Item::ironDoor->m_itemID);
	addCreativeItem(Item::rocket->m_itemID);

	// more stuff
	addCreativeItem(Tile::cloth->m_ID, 0);
	addCreativeItem(Tile::cloth->m_ID, 1);
	addCreativeItem(Tile::cloth->m_ID, 2);
	addCreativeItem(Tile::stoneSlabHalf->m_ID, 1);
	addCreativeItem(Tile::stoneSlabHalf->m_ID, 2);
	addCreativeItem(Tile::stoneSlabHalf->m_ID, 3);
	addCreativeItem(Tile::treeTrunk->m_ID, 1);
	addCreativeItem(Tile::treeTrunk->m_ID, 2);
	addCreativeItem(Tile::cactus->m_ID);
	addCreativeItem(Tile::tallGrass->m_ID);
	addCreativeItem(Tile::deadBush->m_ID);
	addCreativeItem(Tile::pumpkin->m_ID);
	addCreativeItem(Tile::pumpkinLantern->m_ID);
	addCreativeItem(Tile::netherrack->m_ID);
	addCreativeItem(Tile::soulSand->m_ID);
	addCreativeItem(Tile::glowstone->m_ID);
	addCreativeItem(Tile::web->m_ID);

}

void IngameBlockSelectionScreen::addCreativeItem(int itemID, int auxValue)
{
	m_items.push_back(ItemStack(itemID, 1, auxValue));
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
	return (int(m_items.size()) + 8) / 9;
}

bool IngameBlockSelectionScreen::isAllowed(int slot)
{
	return slot >= 0 && slot < int(m_items.size());
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

	for (size_t i = 0; i < m_items.size(); i++)
	{
		ItemStack& item = m_items[i];
		if (!item.isEmpty() && item.getId() == pInv->getSelectedItemId())
		{
			m_selectedSlot = i;
			break;
		}
	}

	if (!isAllowed(m_selectedSlot))
		m_selectedSlot = 0;
}

void IngameBlockSelectionScreen::renderSlot(int index, int x, int y, float f)
{
	if (index >= int(m_items.size())) return;

	ItemStack& item = m_items[index];
	if (item.isEmpty())
		return;

	ItemRenderer::singleton().renderGuiItem(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, item, x, y, true);
	ItemRenderer::singleton().renderGuiItemOverlay(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, item, x, y);
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

	ItemStack& selected = m_items[m_selectedSlot];

	pInv->pickItem(selected.getId(), selected.getAuxValue(), C_MAX_HOTBAR_ITEMS);

	m_pMinecraft->m_pSoundEngine->playUI("random.click");
	m_pMinecraft->setScreen(nullptr);
}
