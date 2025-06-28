/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// @TODO: Add keyboard based control

#include "ScrolledSelectionList.hpp"

#define C_ITEM_WIDTH C_SCROLLED_LIST_ITEM_WIDTH

ScrolledSelectionList::ScrolledSelectionList(Minecraft* minecraft, int a3, int a4, int a5, int a6, int a7) :
	m_pMinecraft(minecraft),
	field_C(float(a5)),
	field_10(float(a6)),
	m_itemHeight(a7),
	field_18(a3),
	field_1C(a4),
	field_20(float(a3)),
	field_24(0.0f),
	field_2C(-2),
	field_30(0.0f),
	field_34(0.0f),
	field_38(0.0f),
	field_3C(-1),
	field_40(0),
	m_bRenderSelection(true),
	field_45(false),
	field_48(0)
{	
}

void ScrolledSelectionList::setRenderSelection(bool b)
{
	m_bRenderSelection = b;
}

int ScrolledSelectionList::getMaxPosition()
{
	return field_48 + m_itemHeight * getNumberOfItems();
}

void ScrolledSelectionList::renderHeader(int a, int b, Tesselator& t)
{
}

void ScrolledSelectionList::renderDecorations(int a, int b)
{
}

void ScrolledSelectionList::clickedHeader(int x, int y)
{
}

int ScrolledSelectionList::getItemAtPosition(int x, int y)
{
	if (x < field_18 / 2 - C_ITEM_WIDTH / 2)
		return -1;

	if (x > field_18 / 2 + C_ITEM_WIDTH / 2)
		return -1;

	return getItemAtYPositionRaw(transformY(y));
}

void ScrolledSelectionList::capYPosition()
{
	float maxY = float(getMaxPosition()) - (float(field_10 - field_C) - 4.0f);
	if (maxY < 0.0f)
		maxY *= 0.5f;

	if (field_34 < 0.0f)
		field_34 = 0.0f;
	if (field_34 > maxY)
		field_34 = maxY;
}

void ScrolledSelectionList::onClickItem(int index, int mouseX, int mouseY)
{
	selectItem(index, false);
}

void ScrolledSelectionList::renderScrollBackground()
{
	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x202020);
	t.vertexUV(field_24, field_10, 0.0f, field_24 / 32.0f, (field_10 + float(int(field_34))) / 32.0f);
	t.vertexUV(field_20, field_10, 0.0f, field_20 / 32.0f, (field_10 + float(int(field_34))) / 32.0f);
	t.vertexUV(field_20, field_C,  0.0f, field_20 / 32.0f, (field_C  + float(int(field_34))) / 32.0f);
	t.vertexUV(field_24, field_C,  0.0f, field_24 / 32.0f, (field_C  + float(int(field_34))) / 32.0f);
	t.draw();
}

void ScrolledSelectionList::checkInput(int mouseX, int mouseY)
{
	if (Mouse::isButtonDown(BUTTON_LEFT))
	{
		if (float(mouseY) >= field_C && float(mouseY) <= field_10 && abs(mouseY - field_28) > 5)
		{
			int field_2C_old = field_2C;

			if (field_2C == -1)
			{
				field_2C = 1;
			}
			else if (field_2C == 1)
			{
				field_3C = mouseY;
				field_40 = getTimeMs();
			}
			else if (field_2C == 0)
			{
				float diff = float(mouseY) - field_30;
				field_34 -= diff;
				field_38 += diff;
			}

			if (field_2C_old >= 0)
				field_2C = 0;

			field_28 = -1;
		}
	}
	else
	{
		if (field_2C >= 0)
		{
			if (fabsf(field_38) < 2.0f)
				field_38 = 0.0f;

			if (getTimeMs() - field_40 < 300)
			{
				if (transformY(mouseY) / m_itemHeight >= 0 && m_itemHeight > abs(field_3C - mouseY))
				{
					onClickItem(transformY(mouseY) / m_itemHeight, mouseX, mouseY);
					field_38 = 0.0f;
				}
			}
		}

		field_2C = -1;
		field_34 -= field_38;
	}
}

void ScrolledSelectionList::render(int mouseX, int mouseY, float f)
{
	renderBackground(f);

	int nItems = getNumberOfItems();
	Tesselator& t = Tesselator::instance;

	checkInput(mouseX, mouseY);

	field_30 = float(mouseY);
	field_38 *= 0.75f;
	capYPosition();

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	renderScrollBackground();

	int itemX = field_18 / 2 - (C_ITEM_WIDTH - 4) / 2;
	int scrollY = int(field_C + 4 - float(int(field_34)));

	if (field_45)
		renderHeader(itemX, scrollY, t);

	// Note, X/Y are the lower left's X/Y coordinates, not the upper left's.
	int lowerY = Minecraft::height - int(field_10 / Gui::InvGuiScale);
	int upperY = Minecraft::height - int(field_C  / Gui::InvGuiScale);
	glScissor(0, lowerY, Minecraft::width, upperY - lowerY);
	glEnable(GL_SCISSOR_TEST);

	for (int i = 0; i < nItems; i++)
	{
		float itemY = float(field_48 + scrollY + i * m_itemHeight);
		if (field_10 < itemY)
			continue;

		float lowerY = itemY + m_itemHeight - 4;
		if (lowerY < field_C)
			continue;

		if (m_bRenderSelection && isSelectedItem(i))
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);
			t.begin();
			t.color(0x808080);
			t.vertexUV(float(field_18) / 2.0f - C_ITEM_WIDTH / 2.0f, lowerY + 2.0f, 0.0f, 0.0f, 1.0f);
			t.vertexUV(float(field_18) / 2.0f + C_ITEM_WIDTH / 2.0f, lowerY + 2.0f, 0.0f, 1.0f, 1.0f);
			t.vertexUV(float(field_18) / 2.0f + C_ITEM_WIDTH / 2.0f, itemY  - 2.0f, 0.0f, 1.0f, 0.0f);
			t.vertexUV(float(field_18) / 2.0f - C_ITEM_WIDTH / 2.0f, itemY  - 2.0f, 0.0f, 0.0f, 0.0f);
			t.color(0x000000);
			t.vertexUV(float(field_18) / 2.0f - C_ITEM_WIDTH / 2.0f + 1, lowerY + 1.0f, 0.0f, 0.0f, 1.0f);
			t.vertexUV(float(field_18) / 2.0f + C_ITEM_WIDTH / 2.0f - 1, lowerY + 1.0f, 0.0f, 1.0f, 1.0f);
			t.vertexUV(float(field_18) / 2.0f + C_ITEM_WIDTH / 2.0f - 1, itemY  - 1.0f, 0.0f, 1.0f, 0.0f);
			t.vertexUV(float(field_18) / 2.0f - C_ITEM_WIDTH / 2.0f + 1, itemY  - 1.0f, 0.0f, 0.0f, 0.0f);
			t.draw();
			glEnable(GL_TEXTURE_2D);
		}

		renderItem(i, itemX, int(itemY), int(m_itemHeight - 4.0f), t);
	}
	
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_DEPTH_TEST);

	renderHoleBackground(0.0f, field_C, 255, 255);
	renderHoleBackground(field_10, float(field_1C), 255, 255);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA_TEST);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);

	t.begin();
	t.color(0, 0);
	t.vertexUV(field_24, field_C + 4.0f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(field_20, field_C + 4.0f, 0.0f, 1.0f, 1.0f);
	t.color(0, 255);
	t.vertexUV(field_20, field_C, 0.0f, 1.0f, 0.0f);
	t.vertexUV(field_24, field_C, 0.0f, 0.0f, 0.0f);
	t.draw();

	t.begin();
	t.color(0, 255);
	t.vertexUV(field_24, field_10, 0.0f, 0.0f, 1.0f);
	t.vertexUV(field_20, field_10, 0.0f, 1.0f, 1.0f);
	t.color(0, 0);
	t.vertexUV(field_20, field_10 - 4.0f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(field_24, field_10 - 4.0f, 0.0f, 0.0f, 0.0f);
	t.draw();

	renderDecorations(mouseX, mouseY);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

void ScrolledSelectionList::renderHoleBackground(float a, float b, int c, int d)
{
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x505050, d);
	t.vertexUV(0.0f,            b, 0.0f, 0.0f,             b / 32.0f);
	t.vertexUV(float(field_18), b, 0.0f, field_18 / 32.0f, b / 32.0f);
	t.color(0x505050, c);
	t.vertexUV(float(field_18), a, 0.0f, field_18 / 32.0f, a / 32.0f);
	t.vertexUV(0.0f,            a, 0.0f, 0.0f,             a / 32.0f);
	t.draw();
}

void ScrolledSelectionList::setRenderHeader(bool b, int i)
{
	field_45 = b;
	if (!b)
		i = 0;
	field_48 = i;
}

void ScrolledSelectionList::handleScroll(bool down)
{
	float diff = 5.0f * (down ? -1.0f : 1.0f);
	field_34 -= diff;
	field_38 += diff;
}
