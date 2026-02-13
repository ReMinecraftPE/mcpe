/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "AvailableGamesList.hpp"

AvailableGamesList::AvailableGamesList(Minecraft* a, int b, int c, int d, int e, int f) :
    ScrolledSelectionList(a, b, c, d, e, f)
{
}

int AvailableGamesList::getNumberOfItems()
{
    return int(m_games.size());
}

bool AvailableGamesList::isSelectedItem(int i)
{
    return m_selectedIndex == i;
}

void AvailableGamesList::renderBackground(float f)
{
}

void AvailableGamesList::renderItem(int idx, int x, int y, int width, const MenuPointer& pointer, Tesselator& t)
{
    drawString(*m_pMinecraft->m_pFont, std::string(m_games[idx].m_name.C_String()),    x, y + 2,  0xA0FFFF);
    drawString(*m_pMinecraft->m_pFont, m_games[idx].getAddressStr(), x, y + 16, 0xA0FFFF);
}

void AvailableGamesList::selectItem(int index, bool b)
{
    m_selectedIndex = index;
}
