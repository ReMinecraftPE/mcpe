/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RolledSelectionList.hpp"

static float g_RolledSelectionListUnk, g_RolledSelectionListUnk2;

RolledSelectionList::RolledSelectionList(Minecraft* minecraft, int a, int b, int c, int d, int e, int f, int g)
{
	m_pMinecraft = minecraft;
	m_itemWidth = g;
	field_18 = a;
	field_C = float(c);
	field_10 = float(d);
	field_1C = b;
	field_20 = float(e);
	field_24 = float(f);
	field_28 = -2;
	field_2C = 0.0f;
	field_38 = 0.0f;
	field_3C = -1;
	m_bRenderSelection = true;
	m_bComponentSelected = false;
	field_44 = 0;
	field_48 = false;
	field_4C = 0;
	field_50 = 1;
	g_RolledSelectionListUnk = field_30 = field_34 = float(g - a) / 2.0f;
}

int RolledSelectionList::getItemAtXPositionRaw(int x)
{
	if (x < 0)
		return -1;

	// @NOTE: redundant check
	int idx = x / m_itemWidth;
	if (idx < 0)
		return -1;

	if (idx >= getNumberOfItems())
		return -1;

	return idx;
}

int RolledSelectionList::getItemAtPosition(int x, int y)
{
	if (float(y) < field_20)
		return -1;
	if (float(y) > field_24)
		return -1;

	return getItemAtXPositionRaw(transformX(x));
}

bool RolledSelectionList::capXPosition()
{
	float f1 = float(m_itemWidth - field_18) / 2.0f;
	int i1 = getNumberOfItems();

	if (field_30 < f1)
	{
		field_30 = f1;
		field_38 = 0.0f;
		return true;
	}

	f1 += float(m_itemWidth * (i1 - 1));
	if (field_30 > f1)
	{
		field_30 = f1;
		field_38 = 0.0f;
		return true;
	}

	return false;
}

void RolledSelectionList::tick()
{
	float diff = g_RolledSelectionListUnk - field_34;
	g_RolledSelectionListUnk = field_34;
	g_RolledSelectionListUnk2 = diff;
	field_34 = field_30 - field_38;
}

void RolledSelectionList::render(int mouseX, int mouseY, float f)
{
	renderBackground();

	int nItems = getNumberOfItems();

	// @TODO: fix gotos.
	if (!Mouse::isButtonDown(BUTTON_LEFT))
	{
		if (field_28 < 0)
		{
		_crap:
			field_28 = -1;
			field_30 = getPos(f);
			goto _done;
		}

		if (g_RolledSelectionListUnk2 < 0.0f)
			field_38 = Mth::Max(-20.0f, g_RolledSelectionListUnk2);
		else
			field_38 = Mth::Min(20.0f, g_RolledSelectionListUnk2);

		if (fabsf(field_38) < 2.0f)
		{
			field_38 = 0.0f;

		_continue:
			if (getTimeMs() - field_4C < 300)
			{
				int idx = transformX(mouseX) / m_itemWidth;
				if (idx >= 0)
				{
					if (field_50 == idx && abs(field_3C - mouseX) <= 9)
						selectItem(field_50, 0);
				}
				goto _crap;
			}
			goto _crap;
		}

		if (fabsf(field_38) > 10.0f)
		{
			goto _crap;
		}

		goto _continue;
	}
	else
	{
		touched();

		if (float(mouseY) >= field_20 && float(mouseY) <= field_24)
		{
			if (field_28 == -1)
			{
				field_4C = getTimeMs();
				field_3C = mouseX;
				field_50 = getItemAtPosition(mouseX, field_1C / 2);
			}
			else if (field_28 >= 0)
			{
				field_34 = field_30 = field_30 - (float(mouseX) - field_2C);
			}

			field_28 = 0;
		}
	}

_done:
	field_2C = float(mouseX);
	
	capXPosition();
	
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x202020);
	t.vertexUV(field_C,  field_24, 0.0f, (field_C  + float(int(field_30))) / 32.0f, field_24 / 32.0f);
	t.vertexUV(field_10, field_24, 0.0f, (field_10 + float(int(field_30))) / 32.0f, field_24 / 32.0f);
	t.vertexUV(field_10, field_20, 0.0f, (field_10 + float(int(field_30))) / 32.0f, field_20 / 32.0f);
	t.vertexUV(field_C,  field_20, 0.0f, (field_C  + float(int(field_30))) / 32.0f, field_20 / 32.0f);
	t.draw();

	if (!getNumberOfItems())
		field_30 = 0.0f;

	if (field_48)
		renderHeader(int(field_C + 4.0f - float(int(field_30))), field_1C / 2 - 40, t);

	for (int i = 0; i < nItems; i++)
	{
		float itemX = float(field_44 + float(int(field_C + 4.0f - float(field_30))) + m_itemWidth * i);
		if (field_10 < itemX) continue;

		float width = m_itemWidth - 4.0f;
		if (itemX + width < field_C) continue;

		if (m_bRenderSelection && isSelectedItem(i))
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);

			t.begin();
			t.color(m_bComponentSelected ? 0x7F89BF : 0x808080);
			
			float right = itemX + width;
			float up = float(field_1C) / 2.0f - 48.0f - 4.0f;
			float dn = float(field_1C) / 2.0f + 48.0f - 4.0f;

			t.vertexUV(itemX - 2, up, 0.0f, 0.0f, 0.0f);
			t.vertexUV(itemX - 2, dn, 0.0f, 1.0f, 0.0f);
			t.vertexUV(right + 2, dn, 0.0f, 1.0f, 1.0f);
			t.vertexUV(right + 2, up, 0.0f, 0.0f, 1.0f);
			t.color(0x000000);
			t.vertexUV(itemX - 1, up + 1.0f, 0.0f, 0.0f, 0.0f);
			t.vertexUV(itemX - 1, dn - 1.0f, 0.0f, 1.0f, 0.0f);
			t.vertexUV(right + 1, dn - 1.0f, 0.0f, 1.0f, 1.0f);
			t.vertexUV(right + 1, up + 1.0f, 0.0f, 0.0f, 1.0f);
			t.draw();

			glEnable(GL_TEXTURE_2D);
		}

		renderItem(i, int(itemX), field_1C / 2 - 40, int(width), t);
	}

	glDisable(GL_DEPTH_TEST);

	renderHoleBackground(0.0f, field_20, 255, 255);
	renderHoleBackground(field_24, float(field_1C), 255, 255);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA_TEST);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	
	// @BUG: The X and Y coordinates have been swapped. This causes the gradient to not render
	// in the right place.
#ifdef ORIGINAL_CODE
	t.begin();
	t.color(0, 0);
	t.vertexUV(m_culledEntities, m_rotX + 4.0f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(field_24, m_rotX + 4.0f, 0.0f, 1.0f, 1.0f);
	t.color(0, 255);
	t.vertexUV(field_24, m_rotX, 0.0f, 1.0f, 0.0f);
	t.vertexUV(m_culledEntities, m_rotX, 0.0f, 0.0f, 0.0f);
	t.draw();

	t.begin();
	t.color(0, 255);
	t.vertexUV(m_culledEntities, m_rotY, 0.0f, 0.0f, 1.0f);
	t.vertexUV(field_24, m_rotY, 0.0f, 1.0f, 1.0f);
	t.color(0, 0);
	t.vertexUV(field_24, m_rotY - 4.0f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(m_culledEntities, m_rotY - 4.0f, 0.0f, 0.0f, 0.0f);
	t.draw();
#else
	t.begin();
	t.color(0, 0);
	t.vertexUV(field_C + 4.0f, field_20, 0.0f, 0.0f, 1.0f);
	t.vertexUV(field_C + 4.0f, field_24, 0.0f, 1.0f, 1.0f);
	t.color(0, 255);
	t.vertexUV(field_C, field_24, 0.0f, 1.0f, 0.0f);
	t.vertexUV(field_C, field_20, 0.0f, 0.0f, 0.0f);
	t.draw();

	t.begin();
	t.color(0, 255);
	t.vertexUV(field_10, field_20, 0.0f, 0.0f, 1.0f);
	t.vertexUV(field_10, field_24, 0.0f, 1.0f, 1.0f);
	t.color(0, 0);
	t.vertexUV(field_10 - 4.0f, field_24, 0.0f, 1.0f, 0.0f);
	t.vertexUV(field_10 - 4.0f, field_20, 0.0f, 0.0f, 0.0f);
	t.draw();
#endif
	
	renderDecorations(mouseX, mouseY);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

void RolledSelectionList::renderHoleBackground(float y1, float y2, int a, int b)
{
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x505050, b);
	t.vertexUV(0.0f, y2, 0.0f, 0.0f, y2 / 32.0f);
	t.vertexUV(float(field_18), y2, 0.0f, float(field_18) / 32.0f, y2 / 32.0f);
	t.color(0x505050, a);
	t.vertexUV(float(field_18), y1, 0.0f, float(field_18) / 32.0f, y1 / 32.0f);
	t.vertexUV(0.0f, y1, 0.0f, 0.0f, y1 / 32.0f);
	t.draw();
}

void RolledSelectionList::setRenderSelection(bool b)
{
	m_bRenderSelection = b;
}

void RolledSelectionList::setComponentSelected(bool b)
{
	m_bComponentSelected = b;
}

int RolledSelectionList::getMaxPosition()
{
	return field_44 + m_itemWidth * getNumberOfItems();
}

float RolledSelectionList::getPos(float f)
{
	return field_34 - field_38 * f;
}

void RolledSelectionList::touched()
{

}

void RolledSelectionList::renderHeader(int a, int b, Tesselator& t)
{

}

void RolledSelectionList::renderDecorations(int x, int y)
{
}

void RolledSelectionList::clickedHeader(int x, int y)
{

}

void RolledSelectionList::handleScroll(bool down)
{
	float diff = 5.0f * (down ? -1.0f : 1.0f);
	field_34 = field_30 = field_30 + diff;
	field_28 = 0;
}
