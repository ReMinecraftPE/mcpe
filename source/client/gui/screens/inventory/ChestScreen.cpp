#include "ChestScreen.hpp"
#include "world/inventory/ChestMenu.hpp"
#include "renderer/ShaderConstants.hpp"

ChestScreen::ChestScreen(Container* inventory, Container* container) : ContainerScreen(new ChestMenu(inventory, container)),
      m_pInventory(inventory),
      m_pContainer(container)
{
    const int defaultHeight = 222;
    const int noRowHeight = defaultHeight - 108;
    m_containerRows = m_pContainer->getContainerSize() / 9;
    m_imageHeight = noRowHeight + m_containerRows * 18;
}

void ChestScreen::renderLabels()
{
    m_pFont->draw(m_pContainer->getName(), 8, 6, 0x404040);
    m_pFont->draw(m_pInventory->getName(), 8, m_imageHeight - 96 + 2, 0x404040);
}

void ChestScreen::renderBg(float partialTicks)
{
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/container.png");
    currentShaderColor = Color::WHITE;

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_containerRows * 18 + 17, 0, 0);
    blit(m_leftPos, m_topPos + m_containerRows * 18 + 17, 0, 126, m_imageWidth, 96, 0, 0);
}
