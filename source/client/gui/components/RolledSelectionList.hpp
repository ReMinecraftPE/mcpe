/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../GuiComponent.hpp"
#include "Minecraft.hpp"

class RolledSelectionList : public GuiComponent
{
public:
	RolledSelectionList(Minecraft*, int, int, int, int, int, int, int);
	virtual int getItemAtPosition(int, int);
	virtual bool capXPosition();
	virtual void tick();
	virtual void render(int mouseX, int mouseY, float);
	virtual void renderHoleBackground(float y1, float y2, int a, int b);
	virtual void setRenderSelection(bool);
	virtual void setComponentSelected(bool);
	virtual int getNumberOfItems() = 0;
	virtual void selectItem(int, bool) = 0;
	virtual bool isSelectedItem(int) = 0;
	virtual int getMaxPosition();
	virtual float getPos(float f);
	virtual void touched();
	virtual void renderItem(int, int, int, int, Tesselator&) = 0;
	virtual void renderHeader(int, int, Tesselator&);
	virtual void renderBackground() = 0;
	virtual void renderDecorations(int x, int y);
	virtual void clickedHeader(int, int);

	int getItemAtXPositionRaw(int x);

	// @NOTE: This is inlined.
	inline int transformX(int x)
	{
		return int(x - field_C - float(field_44) + float(int(field_30)) - 4.0f);
	}

public:
	Minecraft* m_pMinecraft;
	float field_C;
	float field_10;
	int m_itemWidth;
	int field_18;
	int field_1C;
	float field_20;
	float field_24;
	int field_28 = -2;
	float field_2C = 0.0f;
	float field_30;
	float field_34;
	float field_38 = 0.0f;
	int field_3C = -1;
	bool m_bRenderSelection = true;
	bool m_bComponentSelected = false;
	int field_44 = 0;
	bool field_48 = false;
	int field_4C = 0;
	int field_50 = -1;
};

