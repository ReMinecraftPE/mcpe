/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "OptionList.hpp"
#include "client/options/Options.hpp"
#include "client/renderer/PatchManager.hpp"
#include "client/renderer/FoliageColor.hpp"
#include "client/renderer/GrassColor.hpp"

#define C_OPTION_ITEM_HEIGHT (20)

#define C_ON_OFF_SWITCH_WIDTH (32)
#define C_ON_OFF_SWITCH_HEIGHT (18)

#define C_DISTANCE_SWITCH_WIDTH (60)
#define C_DISTANCE_SWITCH_HEIGHT (18)

BooleanOptionItem::BooleanOptionItem(bool* pValue, const std::string& text)
{
	m_text = text;
	m_pValue = pValue;
	m_bDisabled = false;
}

void BooleanOptionItem::onClick(OptionList* pList, int mouseX, int mouseY)
{
	if (m_bDisabled)
		return;

	int itemX = pList->field_18 / 2 - (C_SCROLLED_LIST_ITEM_WIDTH - 4) / 2;

	if (mouseX <= itemX + C_SCROLLED_LIST_ITEM_WIDTH - C_ON_OFF_SWITCH_WIDTH - 6)
		return;
	if (mouseX > itemX + C_SCROLLED_LIST_ITEM_WIDTH - 6)
		return;

	// Toggle the value
	toggleState(pList);
}

void BooleanOptionItem::render(OptionList* pList, int x, int y)
{
	pList->drawString(
		pList->m_pMinecraft->m_pFont,
		m_text,
		x + 22,
		y + (C_OPTION_ITEM_HEIGHT - 8) / 2 - 2,
		m_bDisabled ? 0x777777 : 0xCCCCCC);

	pList->drawOnOffSwitch(
		x + C_SCROLLED_LIST_ITEM_WIDTH - C_ON_OFF_SWITCH_WIDTH - 6,
		y + (C_OPTION_ITEM_HEIGHT - C_ON_OFF_SWITCH_HEIGHT) / 2 - 2,
		*m_pValue,
		m_bDisabled);
}

void BooleanOptionItem::toggleState(OptionList* pList)
{
	*m_pValue ^= 1;
}

RenderOptionItem::RenderOptionItem(bool* pValue, const std::string& text) :
	BooleanOptionItem(pValue, text)
{
}

void RenderOptionItem::toggleState(OptionList* pList)
{
	BooleanOptionItem::toggleState(pList);
	pList->m_pMinecraft->m_pLevelRenderer->allChanged();
}


AORenderOptionItem::AORenderOptionItem(bool* pValue, const std::string& text) :
	RenderOptionItem(pValue, text)
{
}

void AORenderOptionItem::toggleState(OptionList* pList)
{
	BooleanOptionItem::toggleState(pList);
	Minecraft::useAmbientOcclusion = *m_pValue;
	pList->m_pMinecraft->m_pLevelRenderer->allChanged();
}

HeaderOptionItem::HeaderOptionItem(const std::string& text)
{
	m_text = text;
}

void HeaderOptionItem::render(OptionList* pList, int x, int y)
{
	pList->drawString(
		pList->m_pMinecraft->m_pFont,
		m_text,
		x + 2,
		y + (C_OPTION_ITEM_HEIGHT - 8) / 2 - 2,
		0xFFFFFF);
}

DistanceOptionItem::DistanceOptionItem(int* pValue, const std::string& text)
{
	m_text = text;
	m_pValue = pValue;
}

void DistanceOptionItem::onClick(OptionList* pList, int mouseX, int mouseY)
{
	int itemX = pList->field_18 / 2 - (C_SCROLLED_LIST_ITEM_WIDTH - 4) / 2;

	if (mouseX <= itemX + C_SCROLLED_LIST_ITEM_WIDTH - C_DISTANCE_SWITCH_WIDTH - 6)
		return;
	if (mouseX > itemX + C_SCROLLED_LIST_ITEM_WIDTH - 6)
		return;

	int oldValue = *m_pValue;
	*m_pValue = (oldValue + 1) % RD_COUNT;
	
	// If the old render distance was bigger than the current one
	if (oldValue < *m_pValue)
		pList->m_pMinecraft->m_pLevelRenderer->allChanged();
}

void DistanceOptionItem::render(OptionList* pList, int x, int y)
{
	pList->drawString(
		pList->m_pMinecraft->m_pFont,
		m_text,
		x + 22,
		y + (C_OPTION_ITEM_HEIGHT - 8) / 2 - 2,
		0xCCCCCC);

	const char* distanceText = "???";
	switch (*m_pValue)
	{
		case RD_EXTREME: distanceText = "EXTREME"; break;
		case RD_FAR:     distanceText = "FAR";     break;
		case RD_NORMAL:  distanceText = "NORMAL";  break;
		case RD_SHORT:   distanceText = "SHORT";   break;
	}

	std::string distanceTextStr(distanceText);

	x += C_SCROLLED_LIST_ITEM_WIDTH - C_DISTANCE_SWITCH_WIDTH - 6;
	y += (C_OPTION_ITEM_HEIGHT - C_DISTANCE_SWITCH_HEIGHT) / 2 - 2;

	pList->fill(x + 0, y + 0, x + C_DISTANCE_SWITCH_WIDTH - 0, y + C_DISTANCE_SWITCH_HEIGHT - 0, 0xFF444444);
	pList->fill(x + 1, y + 1, x + C_DISTANCE_SWITCH_WIDTH - 1, y + C_DISTANCE_SWITCH_HEIGHT - 1, 0xFF111111);
	pList->drawCenteredString(
		pList->m_pMinecraft->m_pFont,
		distanceTextStr,
		x + C_DISTANCE_SWITCH_WIDTH / 2,
		y + (C_DISTANCE_SWITCH_HEIGHT - 8) / 2,
		0xFFFFFF
	);
}

OptionList::OptionList(Minecraft* pMinecraft, int width, int height, int something, int something2) :
	ScrolledSelectionList(pMinecraft, width, height, something, something2, C_OPTION_ITEM_HEIGHT)
{
	m_selectedItem = -1;
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
		if (!m_items[index]->maySelect())
			return;
	}

	m_selectedItem = index;
}

bool OptionList::isSelectedItem(int index)
{
	return m_selectedItem == index;
}

void OptionList::drawOnOffSwitch(int x, int y, bool state, bool disabled)
{
	// Obs: The color setting that affected disabled items' texts will also affect this.

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui_custom.png");
	
	blit(x, y, 0, state ? 0 : 18, C_ON_OFF_SWITCH_WIDTH, C_ON_OFF_SWITCH_HEIGHT, C_ON_OFF_SWITCH_WIDTH, C_ON_OFF_SWITCH_HEIGHT);

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui.png");// bind the old texture back
}

void OptionList::renderItem(int index, int x, int y, int height, Tesselator& t)
{
	Font* f = m_pMinecraft->m_pFont;
	OptionItem* pItem = m_items[index];

	pItem->render(this, x, y);
}

void OptionList::renderBackground(float f)
{
	if (!m_pMinecraft->isLevelGenerated())
		m_pMinecraft->m_pScreen->renderMenuBackground(f);
}

void OptionList::renderHoleBackground(float a, float b, int c, int d)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x202020, 0xC0);
	t.vertexUV(0.0f, b, 0.0f, 0.0f, b / 32.0f);
	t.vertexUV(float(field_18), b, 0.0f, field_18 / 32.0f, b / 32.0f);
	t.vertexUV(float(field_18), a, 0.0f, field_18 / 32.0f, a / 32.0f);
	t.vertexUV(0.0f, a, 0.0f, 0.0f, a / 32.0f);
	t.draw();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void OptionList::renderScrollBackground()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x202020, 0x90);
	t.vertexUV(field_24, field_10, 0.0f, field_24 / 32.0f, (field_10 + float(int(field_34))) / 32.0f);
	t.vertexUV(field_20, field_10, 0.0f, field_20 / 32.0f, (field_10 + float(int(field_34))) / 32.0f);
	t.vertexUV(field_20, field_C,  0.0f, field_20 / 32.0f, (field_C  + float(int(field_34))) / 32.0f);
	t.vertexUV(field_24, field_C,  0.0f, field_24 / 32.0f, (field_C  + float(int(field_34))) / 32.0f);
	t.draw();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void OptionList::onClickItem(int index, int mouseX, int mouseY)
{
	if (index >= getNumberOfItems())
		index = -1;

	selectItem(index, false);

	if (index >= 0)
		m_items[index]->onClick(this, mouseX, mouseY);
}

void OptionList::clear()
{
	for (std::vector<OptionItem*>::iterator iter = m_items.begin();
		iter != m_items.end();
		++iter)
	{
		delete (*iter);
	}

	m_items.clear();
}

void OptionList::initDefaultMenu()
{
	Options* pOptions = m_pMinecraft->getOptions();

	int currentIndex = -1;

#define HEADER(text) do { m_items.push_back(new HeaderOptionItem(text)); currentIndex++; } while (0)
#define OPTION(type, name, text) do { m_items.push_back(new type ## OptionItem(&pOptions->name, text)); currentIndex++; } while (0)

	int idxLM = -1, idxGrass = -1, idxBiome = -1, idxSplit = -1, idxController = -1, idxPano = -1;

	HEADER("Video");
	{
		OPTION(Distance, m_iViewDistance,         "Render Distance");
		OPTION(Boolean,  m_bThirdPerson,          "Third Person View");
		OPTION(AORender, m_bAmbientOcclusion,     "Smooth Lighting");
		OPTION(Render,   m_bFancyGraphics,        "Fancy Graphics");
		OPTION(Boolean,  m_bViewBobbing,          "View Bobbing");
		OPTION(Boolean,  m_bAnaglyphs,            "3D Anaglyph");
		OPTION(Boolean,  m_bBlockOutlines,        "Block Outlines");
		OPTION(Render,   m_bFancyGrass,           "Fancy Grass");   idxGrass = currentIndex; // renders colored grass side overlay
		OPTION(Render,   m_bBiomeColors,          "Biome Colors");  idxBiome = currentIndex; // colors the grass based on the current biome
		OPTION(Boolean,  m_bDontRenderGui,        "Hide GUI");
		OPTION(Boolean,  m_bDynamicHand,          "Dynamic Hand Movement");
	}

	HEADER("Controls");
	{
		OPTION(Boolean,  m_bAutoJump,             "Auto Jump");
		OPTION(Boolean,  m_bInvertMouse,          "Invert Y-axis");
		OPTION(Boolean,  m_bSplitControls,        "Split Controls"); idxSplit = currentIndex;
		OPTION(Boolean,  m_bUseController,        "Use Controller"); idxController = currentIndex;
		OPTION(Boolean,  m_bFlyCheat,             "Flight Hax");
	}

	HEADER("Multiplayer");
	{
		OPTION(Boolean, m_bServerVisibleDefault,  "Local Server Multiplayer"); idxLM = currentIndex;
	}

	HEADER("Miscellaneous");
	{
		OPTION(Boolean, m_bDebugText,			  "Debug Text");
		OPTION(Boolean, m_bOldTitleLogo,		  "Old Title Logo");
#ifdef ENH_MENU_BACKGROUND
		OPTION(Boolean, m_bMenuPanorama,		  "Menu Panorama"); idxPano = currentIndex; // renders a spinning panorama on the main menu
#endif
	}

#ifdef __EMSCRIPTEN
	m_items[idxLM]->setDisabled(true);
#endif

	if (!GetPatchManager()->IsGrassSidesTinted())
		m_items[idxGrass]->setDisabled(true);

	if (!GrassColor::isAvailable() || !FoliageColor::isAvailable())
		m_items[idxBiome]->setDisabled(true);

#ifdef ENH_MENU_BACKGROUND
	if (!Screen::isMenuPanoramaAvailable())
		m_items[idxPano]->setDisabled(true);
#endif

	if (!m_pMinecraft->isTouchscreen())
		m_items[idxSplit]->setDisabled(true);

	// Currently just used as a status indicator + prevents need to re-compile to get input mapped for controller
	//if (!m_pMinecraft->platform()->hasGamepad())
		m_items[idxController]->setDisabled(true);
}
