#include "ClassicCraftingScreen_Console.hpp"
#include "world/inventory/CraftingMenu.hpp"
#include "renderer/ShaderConstants.hpp"

ClassicCraftingScreen_Console::ClassicCraftingScreen_Console(Inventory* inventory, const TilePos& tilePos, Level* level) :
    ContainerScreen(new CraftingMenu(inventory, tilePos, level))
{
    m_uiTheme = UI_CONSOLE;
}

void ClassicCraftingScreen_Console::init()
{
    ContainerScreen::init();
    m_imageWidth = 428;
    m_imageHeight = 450;
}

void ClassicCraftingScreen_Console::renderBackground()
{
}

void ClassicCraftingScreen_Console::_renderLabels()
{
    CraftingMenu* craftingMenu = (CraftingMenu*)m_pMenu;

    m_pFont->drawScalable(craftingMenu->m_pCraftSlots->getName(), m_imageWidth / 2 - m_pFont->width(craftingMenu->m_pCraftSlots->getName()), 17, Color::TEXT_GREY);
    m_pFont->drawScalable(m_pMinecraft->m_pLocalPlayer->m_pInventory->getName(), 28, 213, Color::TEXT_GREY);
}

void ClassicCraftingScreen_Console::_renderBg(float partialTick)
{
    currentShaderColor = Color::WHITE;
    blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::PANEL_SLICES, m_leftPos, m_topPos, m_imageWidth, m_imageHeight, 32);
    blitSprite(*m_pMinecraft->m_pTextures, "gui/console/Graphics/Arrow_Off.png", m_leftPos + 206, m_topPos + 100, 72, 48);
}

SlotDisplay ClassicCraftingScreen_Console::_createSlotDisplay(const Slot& slot)
{
    constexpr int slotSize = 42;
    switch (slot.m_group)
    {
    case Slot::OUTPUT:
        return SlotDisplay(308, 97, 64, true);
    case Slot::INPUT:
        return SlotDisplay(62 + (slot.m_slot % 3) * slotSize, 64 + (slot.m_slot / 3) * slotSize, slotSize, true);
    case Slot::INVENTORY:
        return SlotDisplay(28 + (slot.m_slot % 9) * slotSize, 240 + ((slot.m_slot / 9) - 1) * slotSize, slotSize, true);
    case Slot::HOTBAR:
        return SlotDisplay(28 + (slot.m_slot % 9) * slotSize, 379, slotSize, true);
    default:
        return SlotDisplay();
    }
}