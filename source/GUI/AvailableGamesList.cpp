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

void AvailableGamesList::renderBackground()
{
}

void AvailableGamesList::renderItem(int idx, int x, int y, int width, Tesselator& t)
{
    drawString(m_pMinecraft->m_pFont, std::string(m_games[idx].m_name.C_String()),    x, y + 2,  0xFFFFA0);
    drawString(m_pMinecraft->m_pFont, std::string(m_games[idx].m_address.ToString()), x, y + 16, 0xFFFFA0);
}

void AvailableGamesList::selectItem(int index, bool b)
{
    m_selectedIndex = index;
}
