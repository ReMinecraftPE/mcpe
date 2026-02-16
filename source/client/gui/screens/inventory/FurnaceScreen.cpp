#include "FurnaceScreen.hpp"
#include "world/inventory/FurnaceMenu.hpp"
#include "renderer/ShaderConstants.hpp"

FurnaceScreen::FurnaceScreen(Inventory* inventory, FurnaceTileEntity* container)
    : ContainerScreen(new FurnaceMenu(inventory, container)), m_inventory(inventory), m_furnace(container)
{
    m_uiTheme = UI_JAVA;
}

void FurnaceScreen::tick()
{
    ContainerScreen::tick();
}

void FurnaceScreen::_renderLabels()
{
    m_pFont->draw(m_furnace->getName(), 66, 6, 0x404040);
    m_pFont->draw(m_inventory->getName(), 8, m_imageHeight - 96 + 2, 0x404040);
}

void FurnaceScreen::_renderBg(float a)
{
    currentShaderColor = Color::WHITE;

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

SlotDisplay FurnaceScreen::_createSlotDisplay(const Slot& slot)
{
    constexpr int slotSize = 18;
    switch (slot.m_group)
    {
    case Slot::INPUT:
        return SlotDisplay(56, 17 + (slot.m_slot % 2) * (slotSize * 2));
    case Slot::OUTPUT:
        return SlotDisplay(116, 35);
    case Slot::INVENTORY:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 84 + ((slot.m_slot / 9) - 1) * slotSize, slotSize);
    case Slot::HOTBAR:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 142, slotSize);
    default:
        return SlotDisplay();
    }
}