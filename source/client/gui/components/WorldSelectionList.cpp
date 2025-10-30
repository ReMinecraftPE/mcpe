/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "WorldSelectionList.hpp"

static float WorldSelectionList_Static1(float a, float b, float c, float d)
{
	float x1 = 2 * (a / b), x2;

	if (x1 < 1.0f)
		x2 = c + ((d - c) * 0.5f) * x1 * x1;
	else
		x2 = c + ((d - c) * -0.5f) * ((x1 - 1.0f) * (x1 - 3.0f) - 1.0f);

	return x2;
}

WorldSelectionList::WorldSelectionList(Minecraft* minecraft, int a, int b) :
	RolledSelectionList(minecraft, a, b, 0, a, 26, b - 32, 120),
	field_90(false),
	field_CC(-1),
	field_D0(0),
	field_D8(0)
{
	field_68 = b;
}

bool WorldSelectionList::capXPosition()
{
	if (RolledSelectionList::capXPosition())
	{
		field_D8 = 0;
		return true;
	}

	return false;
}

void WorldSelectionList::tick()
{
	RolledSelectionList::tick();
	field_D0++;
	if (Mouse::isButtonDown(BUTTON_LEFT) || !field_28)
		return;

	m_selectedIndex = -1;
	if (field_D8 == 1)
	{
		field_54 += 1.0f;
		if (field_54 != field_58)
		{
			tweenInited();
			return;
		}

		field_D8 = 0;
		field_38 = 0.0f;
		field_34 = field_30 = field_60;
		m_selectedIndex = getItemAtPosition(field_18 / 2, field_1C / 2);
		return;
	}

	float abs_field38 = Mth::abs(field_38);
	if (abs_field38 >= 5.0f)
	{
		field_38 *= 0.9f;
		return;
	}

	field_38 *= 0.8f;
	if (abs_field38 < 1.0f && field_28 < 0)
	{
		float x1 = float((field_18 - m_itemWidth) / 2) + field_30;
		int x2 = getItemAtXPositionRaw(int(x1 - 10.0f * field_38));
		float x3 = float(m_itemWidth * x2) - x1;

		if (x3 < -float(m_itemWidth / 2))
			x3 += float(m_itemWidth);

		if (Mth::abs(x3) > 1.0f || abs_field38 >= 0.1f)
		{
			field_5C = field_30;
			field_60 = field_30 + x3;
			field_54 = 0.0f;
			field_D8 = 1;
			field_58 = float(Mth::Min(7, int(float(0.25f * Mth::abs(x3))) + 1));

			tweenInited();
			return;
		}

		m_selectedIndex = getItemAtPosition(field_18 / 2, field_1C / 2);
	}
}

int WorldSelectionList::getNumberOfItems()
{
	return int(m_items.size());
}

void WorldSelectionList::selectItem(int index, bool b)
{
	if (m_selectedIndex >= 0 && m_selectedIndex == index && !field_90)
	{
		field_90 = 1;
		m_levelSummary = m_items[index];
	}
}

bool WorldSelectionList::isSelectedItem(int index)
{
	return m_selectedIndex == index;
}

float WorldSelectionList::getPos(float f)
{
	if (field_D8 != 1)
		return RolledSelectionList::getPos(f);

	return Mth::Lerp(WorldSelectionList_Static1(field_54,        field_58, field_5C, field_60),
	            WorldSelectionList_Static1(field_54 + 1.0f, field_58, field_5C, field_60),
	            f);
}

void WorldSelectionList::touched()
{
	field_D8 = false;
}

void WorldSelectionList::renderItem(int index, int xPos, int yPos, int width, Tesselator& t)
{
    int yPadding = -4;
    yPos += yPadding; // Move up
    
	int xCenter = xPos + m_itemWidth / 2;
	float mult = Mth::Max(1.1f - 0.0055f * float(abs(field_18 / 2 - xCenter)), 0.2f);
	if (mult > 1.0f)
		mult = 1.0f;

	int color1 = 0x010101 * int(mult * 255.0f);
	int color2 = 0x010101 * int(mult * 140.0f);

	std::vector<std::string> details = m_vvs[index];

    int x = xCenter + 5 - m_itemWidth / 2;
	// Draw name
	drawString(m_pMinecraft->m_pFont, details[0], x, yPos + 50 + yPadding, color1);
	// Draw other details
	for (unsigned int i = 1; i < details.size()-1; i++)
	{
		drawString(m_pMinecraft->m_pFont, details[i], x, yPos + (50 + yPadding + (10 * i)), color2);
	}
    // Draw storage version
    drawString(m_pMinecraft->m_pFont, details[details.size()-1], xCenter + 42, yPos + (50 + yPadding + (10 * 3)), color2);

	m_pMinecraft->m_pTextures->loadAndBindTexture(m_previewImages[index]);
	
	// @NOTE: useless assignment of color
	t.color(0.3f, 1.0f, 0.2f);

	t.begin();
	t.color(color1);
	float y = float(yPos) - 6.0f;
	t.vertexUV(float(xCenter - 32), y,         this->field_4, 0.0f, 0.0f);
	t.vertexUV(float(xCenter - 32), y + 48.0f, this->field_4, 0.0f, 1.0f);
	t.vertexUV(float(xCenter + 32), y + 48.0f, this->field_4, 1.0f, 1.0f);
	t.vertexUV(float(xCenter + 32), y,         this->field_4, 1.0f, 0.0f);
	t.draw();
}

void WorldSelectionList::renderBackground()
{
}

void WorldSelectionList::commit()
{
	for (int i = 0; i < int(m_items.size()); i++)
	{
		LevelSummary& item = m_items[i];

		// @NOTE: this string stream crap is unused.
		// Weirdly Java Edition Beta 1.3 did not have world previews, so its interesting to see PE try
		/*std::stringstream ss;
		ss << item.m_levelName << "/preview.png";*/

		m_previewImages.push_back("gui/default_world.png");

		std::vector<std::string> vs;
		vs.push_back(item.m_levelName);
		vs.push_back(m_pMinecraft->platform()->getDateString(item.m_lastPlayed));
		vs.push_back(item.m_fileName);
		vs.push_back(GameTypeConv::GameTypeToNonLocString(item.m_gameType));
        std::stringstream ss;
        ss << "V" << item.m_storageVersion;
        vs.push_back(ss.str());
		m_vvs.push_back(vs);
	}
}

void WorldSelectionList::stepLeft()
{
	if (m_selectedIndex <= 0)
		return;

	field_5C = field_30;
	field_D8 = 1;
	field_60 = field_5C - float(m_itemWidth);
	field_54 = 0.0f;
	field_58 = 8.0f;
	tweenInited();
}

void WorldSelectionList::stepRight()
{
	if (m_selectedIndex < 0 || m_selectedIndex >= getNumberOfItems() - 1)
		return;

	field_5C = field_30;
	field_D8 = 1;
	field_60 = field_5C + float(m_itemWidth);
	field_54 = 0.0f;
	field_58 = 8.0f;
	tweenInited();
}

void WorldSelectionList::tweenInited()
{
	field_38 =
		WorldSelectionList_Static1(field_54,        field_58, field_5C, field_60) -
		WorldSelectionList_Static1(field_54 + 1.0f, field_58, field_5C, field_60);
}
