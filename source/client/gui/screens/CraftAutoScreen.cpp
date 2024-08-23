#include "CraftAutoScreen.hpp"


CraftAutoScreen::CraftAutoScreen() : 
	m_btnUnknown(1, "Pause")
{
}


void CraftAutoScreen::init()
{

}

void CraftAutoScreen::render(int x, int y, float f)
{
	renderBackground();

	drawCenteredString(m_pFont, "Crafting Screen Goes here!", m_width / 2, 24, 0xFFFFFF);
	Screen::render(x, y, f);
}
void CraftAutoScreen::buttonClicked(Button* pButton)
{
}
void CraftAutoScreen::mouseClicked(int x, int y, int type)
{

}
void CraftAutoScreen::mouseReleased(int x, int y, int type)
{

}
void CraftAutoScreen::removed()
{

}