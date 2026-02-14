#include "FurnaceScreen.hpp"
#include "world/inventory/FurnaceMenu.hpp"

std::string quip = "Heeeey I'm da fuckin furnaaace";
FurnaceScreen::FurnaceScreen(Inventory* inventory, FurnaceTileEntity* container)
    : ContainerScreen(new FurnaceMenu(inventory, container)), m_inventory(inventory), m_furnace(container)
{
    ticker = 0;
}

void FurnaceScreen::tick()
{
    ContainerScreen::tick();
    ticker += 2;   
}

void FurnaceScreen::_renderLabels()
{
    std::string ok = quip.substr(0, std::min((float)quip.length(), ticker));
    m_pFont->draw(ok, 6, 6, 0x404040);
    m_pFont->draw(m_inventory->getName(), 8, m_imageHeight - 96 + 2, 0x404040);
}

void FurnaceScreen::_renderBg(float a)
{
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/furnace.png");

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 0, 0);

    int p;
    if (m_furnace->isLit())
    {
        p = m_furnace->getLitProgress(12);
        blit(m_leftPos + 56, m_topPos + 36 + 12 - p, 176, 12 - p, 14, p + 2, 0, 0);
    }

    p = m_furnace->getBurnProgress(24);
    blit(m_leftPos + 79, m_topPos + 34, 176, 14, p + 1, 16, 0, 0);
}
