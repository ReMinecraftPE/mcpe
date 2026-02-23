#include "ChestScreen.hpp"
#include "world/inventory/ChestMenu.hpp"
#include "renderer/ShaderConstants.hpp"

ChestScreen::ChestScreen(Container* inventory, Container* container) : ContainerScreen(new ChestMenu(inventory, container)),
      m_pInventory(inventory),
      m_pContainer(container)
{
    constexpr int defaultHeight = 222;
    constexpr int noRowHeight = defaultHeight - 108;
    m_containerRows = m_pContainer->getContainerSize() / 9;
    m_imageHeight = noRowHeight + m_containerRows * 18;
}

void ChestScreen::_renderLabels()
{
    m_pFont->draw(m_pContainer->getName(), 8, 6, Color::TEXT_GREY);
    m_pFont->draw(m_pInventory->getName(), 8, m_imageHeight - 96 + 2, Color::TEXT_GREY);
}

void ChestScreen::_renderBg(float partialTicks)
{
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/container.png");
    currentShaderColor = Color::WHITE;

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_containerRows * 18 + 17, 0, 0);
    blit(m_leftPos, m_topPos + m_containerRows * 18 + 17, 0, 126, m_imageWidth, 96, 0, 0);
}

SlotDisplay ChestScreen::_createSlotDisplay(const Slot& slot)
{
    constexpr int slotSize = 18;
    int rows = m_pContainer->getContainerSize() / 9;
    int verticalOffset = (rows - 4) * slotSize;
    switch (slot.m_group)
    {
    case Slot::CONTAINER:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 18 + verticalOffset + ((slot.m_slot / 9) - 1) * slotSize, slotSize);
    case Slot::INVENTORY:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 103 + verticalOffset + ((slot.m_slot / 9) - 1) * slotSize, slotSize);
    case Slot::HOTBAR:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 142, slotSize);
    default:
        return SlotDisplay();
    }
}
