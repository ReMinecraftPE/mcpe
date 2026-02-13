/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "OptionList.hpp"
#include "client/gui/components/Button.hpp"
#include "client/options/Options.hpp"

#define C_OPTION_ITEM_HEIGHT (22)

OptionList::OptionList(Minecraft* pMinecraft, int width, int height, int something, int something2) :
	ScrolledSelectionList(pMinecraft, width, height, something, something2, C_OPTION_ITEM_HEIGHT)
{
	m_selectedItem = -1;
	m_bRenderScrollbar = false;
	m_bRenderSelection = false;
}

OptionList::~OptionList()
{
	clear();
}

int OptionList::getNumberOfItems()
{
	return int (m_items.size());
}

// b appears to be unused
void OptionList::selectItem(int index, bool b)
{
	if (index >= 0 && index < getNumberOfItems())
	{
		if (!m_items[index]->isVisible() || !m_items[index]->isEnabled())
			return;
	}

	m_selectedItem = index;
}

bool OptionList::isSelectedItem(int index)
{
	return m_selectedItem == index;
}

void OptionList::renderItem(int index, int x, int y, int height, const MenuPointer& pointer, Tesselator& t)
{
	GuiElement* pItem = m_items[index];

	pItem->m_xPos = x;
	pItem->m_yPos = y;
	pItem->m_width = C_SCROLLED_LIST_ITEM_WIDTH;
	pItem->m_height = height;

	pItem->render(m_pMinecraft, pointer);
}

void OptionList::renderBackground(float f)
{
	if (!m_pMinecraft->isLevelGenerated())
		m_pMinecraft->m_pScreen->renderMenuBackground(f);
}

void OptionList::renderHoleBackground(float a, float b, int c, int d)
{
	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.color(0x202020, 0xC0);
	t.vertexUV(0.0f, b, 0.0f, 0.0f, b / 32.0f);
	t.vertexUV(float(m_width), b, 0.0f, m_width / 32.0f, b / 32.0f);
	t.vertexUV(float(m_width), a, 0.0f, m_width / 32.0f, a / 32.0f);
	t.vertexUV(0.0f, a, 0.0f, 0.0f, a / 32.0f);
	t.draw(m_materials.ui_fill_gradient);
}

void OptionList::onReleaseItem(int index, const MenuPointer& pointer)
{
	if (index >= getNumberOfItems())
		index = -1;

	selectItem(index, false);

	if (index >= 0)
	{
		GuiElement* pItem = m_items[index];

		if (pItem->getType() != GuiElement::TYPE_BUTTON) return;

		((Button*)pItem)->released(pointer);
	}
}

void OptionList::renderScrollBackground()
{
	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.color(0x202020, 0x90);
	t.vertexUV(m_x1, m_y1, 0.0f, m_x1 / 32.0f, (m_y1 + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x0, m_y1, 0.0f, m_x0 / 32.0f, (m_y1 + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x0, m_y0,  0.0f, m_x0 / 32.0f, (m_y0  + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x1, m_y0,  0.0f, m_x1 / 32.0f, (m_y0  + float(int(m_scrollAmount))) / 32.0f);
	t.draw(m_materials.ui_fill_gradient);
}

void OptionList::onClickItem(int index, const MenuPointer& pointer, int relMouseX, int relPointerY, bool doubleClick)
{
	if (index >= getNumberOfItems())
		index = -1;

	selectItem(index, false);

	if (index >= 0)
	{
		GuiElement* pItem = m_items[index];

		if (pItem->getType() != GuiElement::TYPE_BUTTON) return;
		
		Button* button = ((Button*)pItem);

		if (!button->isHovered(m_pMinecraft, pointer)) return;

		button->pressed(m_pMinecraft, pointer);
	}
}

void OptionList::clear()
{
	for (GuiElementList::iterator iter = m_items.begin();
		iter != m_items.end();
		++iter)
	{
		delete (*iter);
	}

	m_items.clear();
}

void OptionList::initDefaultMenu()
{
	initVideoMenu();
}

#define HEADER(text) do { m_items.push_back(new OptionHeader(text)); currentIndex++; } while (0)
#define OPTION(name) do { pOptions->name.addGuiElement(m_items, UI_POCKET); currentIndex++; } while (0)

void OptionList::initGameplayMenu()
{
	Options* pOptions = m_pMinecraft->getOptions();
	int currentIndex = -1;

	HEADER("Game");
	OPTION(m_difficulty);
	OPTION(m_thirdPerson);
	OPTION(m_serverVisibleDefault);

	HEADER("Audio");
	OPTION(m_musicVolume);
	OPTION(m_masterVolume);

#ifndef FEATURE_NETWORKING
	m_items[currentIndex]->setEnabled(false);
#endif
	(void)currentIndex; // compiler will warn about an unused variable sometimes if this isn't here
}

void OptionList::initControlsMenu()
{
	Options* pOptions = m_pMinecraft->getOptions();
	int currentIndex = -1;
	int idxSplit = -1, idxController = -1;

	HEADER("Controls");
	OPTION(m_sensitivity);
	OPTION(m_invertMouse);
	OPTION(m_splitControls); idxSplit = currentIndex;
	//OPTION(m_swapJumpSneak);
	//OPTION(m_buttonSize);
	OPTION(m_autoJump);
	OPTION(m_bUseController); idxController = currentIndex;

	/*HEADER("Feedback");
	OPTION(m_vibrate);*/

	HEADER("Experimental");
	OPTION(m_flightHax);

	if (!m_pMinecraft->isTouchscreen())
		m_items[idxSplit]->setEnabled(false);
	m_items[idxController]->setEnabled(false);
}

void OptionList::initVideoMenu()
{
	Options* pOptions = m_pMinecraft->getOptions();
	int currentIndex = -1;
	int idxPano = -1;

	HEADER("Graphics");
	//OPTION(m_brightness);
	OPTION(m_viewDistance);
	//OPTION(m_antiAliasing);
	//OPTION(m_guiScale);
	//OPTION(m_fov);
	OPTION(m_ambientOcclusion);
	OPTION(m_fancyGraphics);
	OPTION(m_viewBobbing);
	OPTION(m_anaglyphs);
	OPTION(m_blockOutlines);
	OPTION(m_fancyGrass);
	OPTION(m_biomeColors);
	OPTION(m_dynamicHand);
  OPTION(m_uiTheme);
	OPTION(m_logoType);
  
	HEADER("Experimental");
	OPTION(m_hideGui);
	OPTION(m_debugText);
#ifdef ENH_MENU_BACKGROUND
	OPTION(m_menuPanorama); idxPano = currentIndex;
#endif

#ifdef ENH_MENU_BACKGROUND
	if (!Screen::isMenuPanoramaAvailable())
		m_items[idxPano]->setEnabled(false);
#endif
}

OptionHeader::OptionHeader(const std::string& text)
	: GuiElement()
	, m_text(text)
{
}

void OptionHeader::render(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	pMinecraft->m_pFont->draw(
		m_text,
		m_xPos + 2,
		m_yPos + (C_OPTION_ITEM_HEIGHT - 8) / 2 - 2,
		Color::WHITE);
}
