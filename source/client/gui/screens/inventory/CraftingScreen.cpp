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

    m_pFont->draw(craftingMenu->m_pCraftSlots->getName(), 28, 6, Color::GREY_TEXT);
    m_pFont->draw(m_pMinecraft->m_pLocalPlayer->m_pInventory->getName(), 8, m_imageHeight - 96 + 2, Color::GREY_TEXT);
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

ConsoleClassicCraftingScreen::ConsoleClassicCraftingScreen(Inventory* inventory, const TilePos& tilePos, Level* level) :
    ContainerScreen(new CraftingMenu(inventory, tilePos, level))
{
    m_uiTheme = UI_CONSOLE;
}

void ConsoleClassicCraftingScreen::init()
{
    m_imageWidth = 428;
    m_imageHeight = 450;
    ContainerScreen::init();
}

void ConsoleClassicCraftingScreen::renderBackground()
{
}

void ConsoleClassicCraftingScreen::_renderLabels()
{
    CraftingMenu* craftingMenu = (CraftingMenu*)m_pMenu;

    m_pFont->drawScalable(craftingMenu->m_pCraftSlots->getName(), m_imageWidth / 2 - m_pFont->width(craftingMenu->m_pCraftSlots->getName()), 17, Color::GREY_TEXT);
    m_pFont->drawScalable(m_pMinecraft->m_pLocalPlayer->m_pInventory->getName(), 28, 213, Color::GREY_TEXT);
}

void ConsoleClassicCraftingScreen::_renderBg(float partialTick)
{
    currentShaderColor = Color::WHITE;
    blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::PANEL_SLICES, m_leftPos, m_topPos, m_imageWidth, m_imageHeight, 32);
    blitSprite(*m_pMinecraft->m_pTextures, "consolegui/Graphics/Arrow_Off.png", m_leftPos + 206, m_topPos + 100, 72, 48);
}

SlotDisplay ConsoleClassicCraftingScreen::_createSlotDisplay(const Slot& slot)
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