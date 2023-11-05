/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ScrolledSelectionList.hpp"

class Options;
class OptionList;

// Abstract class - actual options will inherit from it
class OptionItem
{
public:
	OptionItem() {}
	virtual ~OptionItem() {}
	virtual void onClick(OptionList*, int mouseX, int mouseY) = 0;
	virtual void render(OptionList*, int x, int y) = 0;
	virtual bool maySelect() { return true; }
	virtual void setDisabled(bool b) { };
};

class BooleanOptionItem : public OptionItem
{
public:
	BooleanOptionItem(bool* pValue, const std::string& text);
	void onClick(OptionList*, int mouseX, int mouseY) override;
	void render(OptionList*, int x, int y) override;
	void setDisabled(bool b) override { m_bDisabled = b; }

	virtual void toggleState(OptionList*);

protected:
	friend class AORenderOptionItem;

	std::string m_text;
	bool* m_pValue; // Reference to the value to be modified by this item
	bool m_bDisabled;
};

// An option item that controls the view distance.
class DistanceOptionItem : public OptionItem
{
	enum
	{
		RD_EXTREME,
		RD_FAR,
		RD_NORMAL,
		RD_SHORT,
		RD_COUNT,
	};

public:
	DistanceOptionItem(int* pValue, const std::string& text);
	void onClick(OptionList*, int mouseX, int mouseY) override;
	void render(OptionList*, int x, int y) override;

protected:
	std::string m_text;
	int* m_pValue; // Reference to the value to be modified by this item
};

class RenderOptionItem : public BooleanOptionItem
{
public:
	RenderOptionItem(bool* pValue, const std::string& text);
	void toggleState(OptionList*) override;
};

class AORenderOptionItem : public RenderOptionItem
{
public:
	AORenderOptionItem(bool* pValue, const std::string& text);
	void toggleState(OptionList*) override;
};

class HeaderOptionItem : public OptionItem
{
public:
	HeaderOptionItem(const std::string& text);
	void render(OptionList*, int x, int y) override;
	bool maySelect() override { return false; }
	void onClick(OptionList*, int mouseX, int mouseY) override {}

private:
	std::string m_text;
};

class OptionList : public ScrolledSelectionList
{
public:
	OptionList(Minecraft*, int, int, int, int);
	~OptionList();

	// Inherited from ScrolledSelectionList
	int getNumberOfItems() override;
	void selectItem(int, bool) override;
	bool isSelectedItem(int) override;
	void renderItem(int, int, int, int, Tesselator&) override;
	void renderBackground(float) override;
	void renderHoleBackground(float, float, int, int) override;
	void onClickItem(int index, int mouseX, int mouseY) override;
	void renderScrollBackground() override;

	void clear();
	void initDefaultMenu();
	void drawOnOffSwitch(int x, int y, bool state, bool disabled = false);

private:
	int m_selectedItem;
	std::vector<OptionItem*> m_items;
};

