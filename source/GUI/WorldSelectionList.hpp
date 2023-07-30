/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "RolledSelectionList.hpp"

class WorldSelectionList : public RolledSelectionList
{
public:
	WorldSelectionList(Minecraft*, int, int);

	bool capXPosition() override;
	void tick() override;
	int getNumberOfItems() override;
	void selectItem(int, bool) override;
	bool isSelectedItem(int) override;
	float getPos(float) override;
	void touched() override;
	void renderItem(int, int, int, int, Tesselator&) override;
	void renderBackground() override;

	void commit();
	void stepLeft();
	void stepRight();
	void tweenInited();

public:
	float field_54;
	float field_58;
	float field_5C;
	float field_60;
	int m_selectedIndex;
	int field_68;
	std::vector<LevelSummary> m_items;
	std::vector<std::vector<std::string> > m_vvs;
	std::vector<std::string> m_previewImages;
	bool field_90 = false;
	LevelSummary m_levelSummary;
	int field_CC = -1;
	int field_D0 = 0;
	int field_D4;
	int field_D8 = 0;
};

