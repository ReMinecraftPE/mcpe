/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiComponent.hpp"
#include "Minecraft.hpp"

class ScrolledSelectionList : public GuiComponent
{
public:
	ScrolledSelectionList(Minecraft*, int, int, int, int, int);

	virtual void setRenderSelection(bool);
	virtual int getNumberOfItems() = 0;
	virtual void selectItem(int, bool) = 0;
	virtual bool isSelectedItem(int) = 0;
	virtual int getMaxPosition();
	virtual void renderItem(int, int, int, int, Tesselator&) = 0;
	virtual void renderHeader(int, int, Tesselator&);
	virtual void renderBackground() = 0;
	virtual void renderDecorations(int, int);
	virtual void clickedHeader(int x, int y);
	virtual int getItemAtPosition(int x, int y);
	virtual void capYPosition();
	virtual void render(int mouseX, int mouseY, float f);
	virtual void renderHoleBackground(float, float, int, int);

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
		return int(y - field_C - field_48 + field_34 - 4.0f);
	}

public:
	Minecraft* m_pMinecraft;
	float field_C;
	float field_10;
	int m_itemHeight;
	int field_18;
	int field_1C;
	float field_20;
	float field_24 = 0.0f;
	int field_28;
	int field_2C = -2;
	float field_30 = 0.0f;
	float field_34 = 0.0f;
	float field_38 = 0.0f;
	int field_3C = -1;
	int field_40 = 0;
	bool m_bRenderSelection = true;
	bool field_45 = false;
	int field_48 = 0;
};

