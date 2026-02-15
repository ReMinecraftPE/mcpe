#include "CraftingScreen.hpp"
#include "world/inventory/CraftingMenu.hpp"
#include "renderer/ShaderConstants.hpp"

CraftingScreen::CraftingScreen(Inventory* inventory, const TilePos& tilePos, Level* level) :
    ContainerScreen(new CraftingMenu(inventory, tilePos, level))
{
    m_uiTheme = UI_JAVA;
}

void CraftingScreen::_renderLabels()
{
    CraftingMenu* craftingMenu = (CraftingMenu*)m_pMenu;

    m_pFont->draw(craftingMenu->m_pCraftSlots->getName(), 28, 6, Color::TEXT_GREY);
    m_pFont->draw(m_pMinecraft->m_pLocalPlayer->m_pInventory->getName(), 8, m_imageHeight - 96 + 2, Color::TEXT_GREY);
}

void CraftingScreen::_renderBg(float partialTick)
{
    currentShaderColor = Color::WHITE;
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/crafting.png");

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 0, 0);
}

SlotDisplay CraftingScreen::_createSlotDisplay(const Slot& slot)
{
    constexpr int slotSize = 18;
    switch (slot.m_group)
    {
    case Slot::OUTPUT:
        return SlotDisplay(124, 35);
    case Slot::INPUT:
        return SlotDisplay(30 + (slot.m_slot % 3) * slotSize, 17 + (slot.m_slot / 3) * slotSize);
    case Slot::INVENTORY:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 84 + ((slot.m_slot / 9) - 1) * slotSize, slotSize);
    case Slot::HOTBAR:
        return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 142, slotSize);
    default:
        return SlotDisplay();
    }
}
