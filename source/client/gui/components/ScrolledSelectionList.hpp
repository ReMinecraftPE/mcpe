/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiComponent.hpp"
#include "client/app/Minecraft.hpp"

#define C_SCROLLED_LIST_ITEM_WIDTH (220)

class ScrolledSelectionList : public GuiComponent
{
public:
	ScrolledSelectionList(Minecraft*, int, int, int, int, int);
	virtual ~ScrolledSelectionList() {}

	virtual void setRenderSelection(bool);
	virtual int getNumberOfItems() = 0;
	virtual void selectItem(int, bool) = 0;
	virtual bool isSelectedItem(int) = 0;
	virtual int getMaxPosition();
	virtual void renderItem(int, int, int, int, const MenuPointer& pointer, Tesselator&) = 0;
	virtual void renderHeader(int, int, Tesselator&);
	virtual void renderBackground(float) = 0;
	virtual void renderDecorations(const MenuPointer& pointer);
	virtual void clickedHeader(int x, int y);
	virtual int getItemAtPosition(int x, int y);
	virtual void capYPosition();
	virtual void render(const MenuPointer& pointer, float f);
	virtual void renderHoleBackground(float, float, int, int);
	virtual void checkInput(const MenuPointer& pointer);
	virtual void onClickItem(int index, const MenuPointer& pointer, int relMouseX, int relPointerY, bool doubleClick);
	virtual void onReleaseItem(int index, const MenuPointer& pointer);
	virtual void mouseClicked(const MenuPointer& pointer, int relMouseX, int relPointerY);
	virtual void renderScrollBackground();
	virtual void handleScrollWheel(float force);

	void setRenderHeader(bool, int);

	// @NOTE: This is inlined.
	inline int getItemAtYPositionRaw(int y)
	{
		if (y < 0)
			return -1;

		// @NOTE: redundant check
		int idx = y / m_itemHeight;
		if (idx < 0)
			return -1;

		if (idx >= getNumberOfItems())
			return -1;

		return idx;
	}
	// @NOTE: This is also inlined.
	inline int transformY(int y)
	{
		return int(y - m_y0 - m_headerHeight + m_scrollAmount - 4.0f);
	}

public:
	Minecraft* m_pMinecraft;
	float m_y0;
	float m_y1;
	int m_itemHeight;
	int m_width;
	int field_1C;
	float m_x0;
	float m_x1;
	int field_28;
	float m_scrollAmount;
	float m_accumulatedScroll;
	float m_scrollBarGrabOffset;
	float m_mouseYWhenPressed;
	int m_lastClickedIndex;
	int m_lastClickTime;
	bool m_bRenderSelection;
	bool m_bRenderHeader;
	bool m_bRenderScrollbar;
	int m_headerHeight;
};

