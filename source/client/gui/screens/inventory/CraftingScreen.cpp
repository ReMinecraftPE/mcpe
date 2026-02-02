#include "CraftingScreen.hpp"
#include "world/inventory/CraftingMenu.hpp"
#include "renderer/ShaderConstants.hpp"

CraftingScreen::CraftingScreen(Inventory* inventory, const TilePos& tilePos, Level* level) :
    ContainerScreen(new CraftingMenu(inventory, tilePos, level))
{
}

void CraftingScreen::_renderLabels()
{
    CraftingMenu* craftingMenu = (CraftingMenu*)m_pMenu;
	m_pFont->draw(craftingMenu->m_pCraftSlots->getName(), 28, 6, 0x404040);
    m_pFont->draw(m_pMinecraft->m_pLocalPlayer->m_pInventory->getName(), 8, m_imageHeight - 96 + 2, 0x404040);
}

void CraftingScreen::_renderBg(float partialTick)
{
    currentShaderColor = Color::WHITE;
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/crafting.png");

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 0, 0);
}
