/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ScrolledSelectionList.hpp"
#include "client/network/PingedCompatibleServer.hpp"

class AvailableGamesList : public ScrolledSelectionList
{
public:
	AvailableGamesList(Minecraft*, int, int, int, int, int);
	int getNumberOfItems() override;
	bool isSelectedItem(int i) override;
	void renderBackground() override;
	void renderItem(int, int, int, int, Tesselator& t) override;
	void selectItem(int, bool) override;

public:
	int m_selectedIndex;
	std::vector<PingedCompatibleServer> m_games;
};

