/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ScrolledSelectionList.hpp"

class OptionList : public ScrolledSelectionList
{
public:
	OptionList(Minecraft*, int, int, int, int);
	~OptionList();

	// Inherited from ScrolledSelectionList
	int getNumberOfItems() override;
	void selectItem(int, bool) override;
	bool isSelectedItem(int) override;
	void renderItem(int, int, int, int, const MenuPointer& pointer, Tesselator&) override;
	void renderBackground(float) override;
	void renderHoleBackground(float, float, int, int) override;
	void onClickItem(int index, const MenuPointer& pointer, int relMouseX, int relPointerY, bool doubleClick) override;
	void onReleaseItem(int index, const MenuPointer& pointer) override;
	void renderScrollBackground() override;

	void clear();
	void initDefaultMenu();
	void initVideoMenu();
	void initControlsMenu();
	void initGameplayMenu();

private:
	int m_selectedItem;
	GuiElementList m_items;
};

class OptionHeader : public GuiElement
{
public:
	OptionHeader(GuiElement::ID id, const std::string& text);

public:
	void render(Minecraft* pMinecraft, const MenuPointer& pointer) override;

private:
	std::string m_text;
};
