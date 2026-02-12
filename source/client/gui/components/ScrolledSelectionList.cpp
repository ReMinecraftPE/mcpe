/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// @TODO: Add keyboard based control

#include "ScrolledSelectionList.hpp"
#include "renderer/EnableScissorTest.hpp"
#include "renderer/RenderContextImmediate.hpp"

#define C_ITEM_WIDTH C_SCROLLED_LIST_ITEM_WIDTH

ScrolledSelectionList::ScrolledSelectionList(Minecraft* minecraft, int a3, int a4, int a5, int a6, int a7) :
	m_pMinecraft(minecraft),
	m_y0(float(a5)),
	m_y1(float(a6)),
	m_itemHeight(a7),
	m_width(a3),
	field_1C(a4),
	m_x0(float(a3)),
	m_x1(0.0f),
	m_scrollAmount(0.0f),
	m_accumulatedScroll(0.0f),
	m_scrollBarGrabOffset(1.0f),
	m_mouseYWhenPressed(-2.0f),
	m_lastClickedIndex(-1),
	m_lastClickTime(0),
	m_bRenderSelection(true),
	m_bRenderHeader(false),
	m_bRenderScrollbar(true),
	m_headerHeight(0)
{	
}

void ScrolledSelectionList::setRenderSelection(bool b)
{
	m_bRenderSelection = b;
}

int ScrolledSelectionList::getMaxPosition()
{
	return m_headerHeight + m_itemHeight * getNumberOfItems();
}

void ScrolledSelectionList::renderHeader(int a, int b, Tesselator& t)
{
}

void ScrolledSelectionList::renderDecorations(const MenuPointer& pointer)
{
}

void ScrolledSelectionList::clickedHeader(int x, int y)
{
}

int ScrolledSelectionList::getItemAtPosition(int x, int y)
{
	if (x < m_width / 2 - C_ITEM_WIDTH / 2)
		return -1;

	if (x > m_width / 2 + C_ITEM_WIDTH / 2)
		return -1;

	return getItemAtYPositionRaw(transformY(y));
}

void ScrolledSelectionList::capYPosition()
{
	float maxY = float(getMaxPosition()) - (float(m_y1 - m_y0) - 4.0f);
	if (maxY < 0.0f)
		maxY *= 0.5f;

	if (m_scrollAmount < 0.0f)
		m_scrollAmount = 0.0f;
	if (m_scrollAmount > maxY)
		m_scrollAmount = maxY;
}

void ScrolledSelectionList::onClickItem(int index, const MenuPointer& pointer, int relMouseX, int relPointerY, bool doubleClick)
{
	selectItem(index, doubleClick);
}

void ScrolledSelectionList::onReleaseItem(int index, const MenuPointer& pointer)
{
}

void ScrolledSelectionList::mouseClicked(const MenuPointer& pointer, int relMouseX, int relPointerY)
{
}

void ScrolledSelectionList::renderScrollBackground()
{
	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.color(0x202020);
	t.vertexUV(m_x1, m_y1, 0.0f, m_x1 / 32.0f, (m_y1 + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x0, m_y1, 0.0f, m_x0 / 32.0f, (m_y1 + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x0, m_y0,  0.0f, m_x0 / 32.0f, (m_y0  + float(int(m_scrollAmount))) / 32.0f);
	t.vertexUV(m_x1, m_y0,  0.0f, m_x1 / 32.0f, (m_y0  + float(int(m_scrollAmount))) / 32.0f);
	t.draw(m_materials.ui_texture_and_color);
}

void ScrolledSelectionList::checkInput(const MenuPointer& pointer)
{
	if (pointer.isPressed)
	{
		if (m_mouseYWhenPressed == -1.0f)
		{
			bool isValidClickArea = true;

			if (pointer.x >= m_y0 && pointer.y <= m_y1)
			{
				int scrollLeft = m_width / 2 + 124;
				int scrollRight = scrollLeft + 6;
				int listLeft = m_width / 2 - 110;
				int listRight = m_width / 2 + 110;
				int relativeY = transformY(pointer.y);
				int hoveredItemIndex = relativeY / m_itemHeight;

				if (pointer.x >= listLeft && pointer.x <= listRight &&
					hoveredItemIndex >= 0 && relativeY >= 0 && hoveredItemIndex < getNumberOfItems())
				{
					bool isDoubleClick = (hoveredItemIndex == m_lastClickedIndex &&
						getTimeMs() - m_lastClickTime < 250);

					onClickItem(hoveredItemIndex, pointer, pointer.x - listLeft, pointer.y - m_y0 + (int)m_scrollAmount - 4, isDoubleClick);
					m_lastClickedIndex = hoveredItemIndex;
					m_lastClickTime = getTimeMs();
				}
				else if (pointer.x >= listLeft && pointer.x <= listRight && relativeY < 0)
				{
					mouseClicked(pointer, pointer.x - listLeft, pointer.y - m_y0 + (int)m_scrollAmount - 4);
					isValidClickArea = false;
				}

				if (m_bRenderScrollbar && pointer.x >= scrollLeft && pointer.x <= scrollRight)
				{
					m_scrollBarGrabOffset = -1.0f;

					int totalContentHeight = getMaxPosition() - (m_y1 - m_y0 - 4);
					if (totalContentHeight < 1) totalContentHeight = 1;

					int thumbHeight = (int)((float)(m_y1 - m_y0) * (m_y1 - m_y0) / (float)getMaxPosition());
					if (thumbHeight < 32) thumbHeight = 32;
					if (thumbHeight > m_y1 - m_y0 - 8) thumbHeight = m_y1 - m_y0 - 8;

					m_scrollBarGrabOffset /= float(m_y1 - m_y0 - thumbHeight) / totalContentHeight;
				}
				else
				{
					m_scrollBarGrabOffset = 1.0f;
				}

				if (isValidClickArea)
				{
					m_mouseYWhenPressed = pointer.y;
				}
				else
				{
					m_mouseYWhenPressed = -2.0f;
				}
			}
			else
			{
				m_mouseYWhenPressed = -2.0f;
			}
		}
		else if (m_mouseYWhenPressed >= 0.0f)
		{
			m_scrollAmount -= (pointer.y - m_mouseYWhenPressed) * m_scrollBarGrabOffset;
			m_mouseYWhenPressed = pointer.y;
		}
	}
	else
	{
		if (m_lastClickedIndex >= 0)
		{
			onReleaseItem(m_lastClickedIndex, pointer);
			m_lastClickedIndex = -1;
		}

		m_mouseYWhenPressed = -1.0f;
	}
}

void ScrolledSelectionList::render(const MenuPointer& pointer, float f)
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	renderBackground(f);

	int nItems = getNumberOfItems();
	Tesselator& t = Tesselator::instance;

	checkInput(pointer);

	if (m_accumulatedScroll)
	{

		float part = m_accumulatedScroll / 10;

		m_scrollAmount += part;

		m_accumulatedScroll = part < 0 ? Mth::Min(m_accumulatedScroll - part, 0.0f) : Mth::Max(m_accumulatedScroll - part, 0.0f);
	}

	capYPosition();

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");

	renderScrollBackground();

	int itemX = m_width / 2 - (C_ITEM_WIDTH - 4) / 2;
	int scrollY = int(m_y0 + 4 - float(int(m_scrollAmount)));

	if (m_bRenderHeader)
		renderHeader(itemX, scrollY, t);

	// Note, X/Y are the lower left's X/Y coordinates, not the upper left's.
	int lowerY = Minecraft::height - int(m_y1 / Gui::GuiScale);
	int upperY = Minecraft::height - int(m_y0  / Gui::GuiScale);

	{
		mce::EnableScissorTest scissor(0, lowerY, Minecraft::width, upperY - lowerY);

		for (int i = 0; i < nItems; i++)
		{
			float itemY = float(m_headerHeight + scrollY + i * m_itemHeight);
			if (m_y1 < itemY)
				continue;

			float lowerY = itemY + m_itemHeight - 4;
			if (lowerY < m_y0)
				continue;

			if (m_bRenderSelection && isSelectedItem(i))
			{
				t.begin(8);
				t.color(0x808080);
				t.vertexUV(float(m_width) / 2.0f - C_ITEM_WIDTH / 2.0f, lowerY + 2.0f, 0.0f, 0.0f, 1.0f);
				t.vertexUV(float(m_width) / 2.0f + C_ITEM_WIDTH / 2.0f, lowerY + 2.0f, 0.0f, 1.0f, 1.0f);
				t.vertexUV(float(m_width) / 2.0f + C_ITEM_WIDTH / 2.0f, itemY - 2.0f, 0.0f, 1.0f, 0.0f);
				t.vertexUV(float(m_width) / 2.0f - C_ITEM_WIDTH / 2.0f, itemY - 2.0f, 0.0f, 0.0f, 0.0f);
				t.color(0x000000);
				t.vertexUV(float(m_width) / 2.0f - C_ITEM_WIDTH / 2.0f + 1, lowerY + 1.0f, 0.0f, 0.0f, 1.0f);
				t.vertexUV(float(m_width) / 2.0f + C_ITEM_WIDTH / 2.0f - 1, lowerY + 1.0f, 0.0f, 1.0f, 1.0f);
				t.vertexUV(float(m_width) / 2.0f + C_ITEM_WIDTH / 2.0f - 1, itemY - 1.0f, 0.0f, 1.0f, 0.0f);
				t.vertexUV(float(m_width) / 2.0f - C_ITEM_WIDTH / 2.0f + 1, itemY - 1.0f, 0.0f, 0.0f, 0.0f);
				t.draw(m_materials.ui_fill_gradient);
			}

			renderItem(i, itemX, int(itemY), int(m_itemHeight - 4.0f), pointer, t);
		}
	}

	renderHoleBackground(0.0f, m_y0, 255, 255);
	renderHoleBackground(m_y1, float(field_1C), 255, 255);

	renderContext.setShadeMode(mce::SHADE_MODE_SMOOTH);

	t.begin(4);
	t.color(0x000000, 0);
	t.vertexUV(m_x1, m_y0 + 4.0f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(m_x0, m_y0 + 4.0f, 0.0f, 1.0f, 1.0f);
	t.color(0x000000, 255);
	t.vertexUV(m_x0, m_y0, 0.0f, 1.0f, 0.0f);
	t.vertexUV(m_x1, m_y0, 0.0f, 0.0f, 0.0f);
	t.draw(m_materials.ui_fill_gradient);

	t.begin(4);
	t.color(0x000000, 255);
	t.vertexUV(m_x1, m_y1, 0.0f, 0.0f, 1.0f);
	t.vertexUV(m_x0, m_y1, 0.0f, 1.0f, 1.0f);
	t.color(0x000000, 0);
	t.vertexUV(m_x0, m_y1 - 4.0f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(m_x1, m_y1 - 4.0f, 0.0f, 0.0f, 0.0f);
	t.draw(m_materials.ui_fill_gradient);

	int yDiff = getMaxPosition() - (m_y1 - m_y0 - 4);
	if (m_bRenderScrollbar && yDiff > 0)
	{
		int barHeight = (m_y1 - m_y0) * (m_y1 - m_y0) / getMaxPosition();
		if (barHeight < 32)
			barHeight = 32;

		if (barHeight > m_y1 - m_y0 - 8)
			barHeight = m_y1 - m_y0 - 8;

		int barTop = (int)m_scrollAmount * (m_y1 - m_y0 - barHeight) / yDiff + m_y0;
		if (barTop < m_y0)
			barTop = m_y0;

		int barLeft = m_width / 2 + 124;
		int barRight = barLeft + 6;

		t.begin(4);
		t.color(0x000000, 255);
		t.vertexUV(barLeft, m_y1, 0.0f, 0.0f, 1.0f);
		t.vertexUV(barRight, m_y1, 0.0f, 1.0f, 1.0f);
		t.vertexUV(barRight, m_y0, 0.0f, 1.0f, 0.0f);
		t.vertexUV(barLeft, m_y0, 0.0f, 0.0f, 0.0f);
		t.draw(m_materials.ui_fill_gradient);
		t.begin(4);
		t.color(0x808080, 255);
		t.vertexUV(barLeft, (barTop + barHeight), 0.0f, 0.0f, 1.0f);
		t.vertexUV(barRight, (barTop + barHeight), 0.0f, 1.0f, 1.0f);
		t.vertexUV(barRight, barTop, 0.0f, 1.0f, 0.0f);
		t.vertexUV(barLeft, barTop, 0.0f, 0.0f, 0.0f);
		t.draw(m_materials.ui_fill_gradient);
		t.begin(4);
		t.color(0xC0C0C0, 255);
		t.vertexUV(barLeft, (barTop + barHeight - 1), 0.0f, 0.0f, 1.0f);
		t.vertexUV((barRight - 1), (barTop + barHeight - 1), 0.0f, 1.0f, 1.0f);
		t.vertexUV((barRight - 1), barTop, 0.0f, 1.0f, 0.0f);
		t.vertexUV(barLeft, barTop, 0.0f, 0.0f, 0.0f);
		t.draw(m_materials.ui_fill_gradient);
	}

	renderDecorations(pointer);

	renderContext.setShadeMode(mce::SHADE_MODE_FLAT);
}

void ScrolledSelectionList::renderHoleBackground(float a, float b, int c, int d)
{
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");

	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.color(0x505050, d);
	t.vertexUV(0.0f,            b, 0.0f, 0.0f,             b / 32.0f);
	t.vertexUV(float(m_width), b, 0.0f, m_width / 32.0f, b / 32.0f);
	t.color(0x505050, c);
	t.vertexUV(float(m_width), a, 0.0f, m_width / 32.0f, a / 32.0f);
	t.vertexUV(0.0f,            a, 0.0f, 0.0f,             a / 32.0f);
	t.draw(m_materials.ui_texture_and_color);
}

void ScrolledSelectionList::setRenderHeader(bool b, int i)
{
	m_bRenderHeader = b;
	if (!b)
		i = 0;
	m_headerHeight = i;
}

void ScrolledSelectionList::handleScrollWheel(float force)
{
	float diff = 5.0f * force;
	m_accumulatedScroll -= diff;
}
