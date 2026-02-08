#include "InventoryScreen.hpp"
#include "client/renderer/Lighting.hpp"
#include "renderer/ShaderConstants.hpp"
#include "world/inventory/ArmorSlot.hpp"
//#include "stats/Achievement.hpp"

std::string InventoryScreen::CONSOLE_ARMOR_SLOTS[] = { "consolegui/Graphics/Armour_Slot_Feet.png", "consolegui/Graphics/Armour_Slot_Legs.png", "consolegui/Graphics/Armour_Slot_Body.png", "consolegui/Graphics/Armour_Slot_Head.png" };

InventoryScreen::InventoryScreen(Player* player) : ContainerScreen(player->m_pInventoryMenu)
{
    //player->awardStat(Achievements::openInventory);
}

void InventoryScreen::init()
{
    m_uiTheme = m_pMinecraft->getOptions()->getUITheme() ;
    if (m_uiTheme == UI_CONSOLE)
    {
        m_imageWidth = 432;
        m_imageHeight = 436;
    }
    ContainerScreen::init();
    if (m_uiTheme == UI_CONSOLE)
        m_topPos = Mth::Max(24, m_topPos - 48);
}

void InventoryScreen::renderBackground()
{
    if (m_uiTheme != UI_CONSOLE)
        ContainerScreen::renderBackground();
}

void InventoryScreen::_renderLabels()
{
    InventoryMenu* craftingMenu = (InventoryMenu*)m_pMenu;
    if (m_uiTheme == UI_CONSOLE)
    {
        if (m_pMinecraft->getOptions()->m_classicCrafting.get())
            m_pFont->drawScalable(craftingMenu->m_pCraftSlots->getName(), 220, 40, Color::GREY_TEXT);
        m_pFont->drawScalable(m_pMinecraft->m_pLocalPlayer->m_pInventory->getName(), 27, 207, Color::GREY_TEXT);
    }
    else
        m_pFont->draw(craftingMenu->m_pCraftSlots->getName(), 86, 16, Color::GREY_TEXT);
}

void InventoryScreen::_renderBg(float partialTick)
{
    currentShaderColor = Color::WHITE;
    int playerX;
    int playerY;
    float scale;

    if (m_uiTheme == UI_CONSOLE)
    {
        playerX = 243;
        playerY = 185;
        scale = 75.0f;
        blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::PANEL_SLICES, m_leftPos, m_topPos, m_imageWidth, m_imageHeight, 32);
        int playerSlotX = m_leftPos + 180;
        int playerSlotY = m_topPos + 27;

        if (m_pMinecraft->getOptions()->m_classicCrafting.get())
        {
            playerX -= 100;
            playerSlotX -= 100;
            blitSprite(*m_pMinecraft->m_pTextures, "consolegui/Graphics/Arrow_Small_Off.png", m_leftPos + 309, m_topPos + 89, 32, 32);
        }

        //For some reason, TU0 didn't had a texture for the player slot, but we do!
        blitNineSlice(*m_pMinecraft->m_pTextures, "gui/container/entity_slot.png", playerSlotX, playerSlotY, 126, 168, 2);
        currentShaderColor = Color::WHITE;
    }
    else
    {
        playerX = 51;
        playerY = 75;
        scale = 30.0f;
        m_pMinecraft->m_pTextures->loadAndBindTexture("gui/inventory.png");

        blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 0, 0);
    }

#if MCE_GFX_API_OGL && !defined(FEATURE_GFX_SHADERS)
    glEnable(GL_RESCALE_NORMAL);
#endif
    MatrixStack::Ref matrix = MatrixStack::World.push();

    matrix->translate(Vec3(m_leftPos + playerX, m_topPos + playerY, 50.0f));
    matrix->scale(Vec3(-scale, scale, scale));
    matrix->rotate(180.0f, Vec3(0.0f, 0.0f, 1.0f));

    LocalPlayer* player = m_pMinecraft->m_pLocalPlayer;

    float prevYBodyRot = player->m_yBodyRot;
    float prevXRot = player->m_rot.x;
    float prevYRot = player->m_rot.y;

    float dx = m_leftPos + playerX - m_menuPointer.x;
    float dy = m_topPos + playerY - 50 - m_menuPointer.y;

    matrix->rotate(135.0f, Vec3(0.0f, 1.0f, 0.0f));
    Lighting::turnOn();
    matrix->rotate(-135.0f, Vec3(0.0f, 1.0f, 0.0f));

    matrix->rotate(-Mth::atan(dy / 40.0f) * 20.0f, Vec3(1.0f, 0.0f, 0.0f));
    player->m_yBodyRot = Mth::atan(dx / 40.0f) * 20.0f;
    player->m_rot.y = -Mth::atan(dy / 40.0f) * 20.0f;
    player->m_rot.x = Mth::atan(dx / 40.0f) * 40.0f;

    matrix->translate(Vec3(0.0f, player->m_heightOffset, 0.0f));
    EntityRenderer* renderer = EntityRenderDispatcher::instance->getRenderer(player->m_renderType);
    float oldShadowRadius = renderer->m_shadowRadius;
    renderer->m_shadowRadius = 0.0f;
    player->m_minBrightness = 1.0f;
    EntityRenderDispatcher::instance->m_rot.y = 180;
    EntityRenderDispatcher::instance->render(*player, Vec3::ZERO, 0.0f, 1.0f);
    player->m_minBrightness = 0.0f;
    renderer->m_shadowRadius = oldShadowRadius;
    player->m_yBodyRot = prevYBodyRot;
    player->m_rot.x = prevXRot;
    player->m_rot.y = prevYRot;

    Lighting::turnOff();
#if MCE_GFX_API_OGL && !defined(FEATURE_GFX_SHADERS)
    glDisable(GL_RESCALE_NORMAL);
#endif
}

SlotDisplay InventoryScreen::_createSlotDisplay(const Slot& slot)
{
    if (m_uiTheme == UI_CONSOLE)
    {
        constexpr int slotSize = 42;
        switch (slot.m_group)
        {
        case Slot::OUTPUT:
            return m_pMinecraft->getOptions()->m_classicCrafting.get() ? SlotDisplay(352, 83, 54, true) : SlotDisplay();
        case Slot::INPUT:
            return m_pMinecraft->getOptions()->m_classicCrafting.get() ? SlotDisplay(221 + (slot.m_slot % 2) * slotSize, 67 + (slot.m_slot / 2) * slotSize, slotSize, true) : SlotDisplay();
        case Slot::ARMOR:
        {
            const ArmorSlot& armorSlot = (const ArmorSlot&)slot;
            return SlotDisplay(m_pMinecraft->getOptions()->m_classicCrafting.get() ? 27 : 127, 29 + (Item::SLOT_HEAD - armorSlot.m_equipmentSlot) * slotSize, slotSize, true, -1, CONSOLE_ARMOR_SLOTS[armorSlot.m_equipmentSlot]);
        }
        case Slot::INVENTORY:
            return SlotDisplay(28 + (slot.m_slot % 9) * slotSize, 233 + ((slot.m_slot / 9) - 1) * slotSize, slotSize, true);
        case Slot::HOTBAR:
            return SlotDisplay(28 + (slot.m_slot % 9) * slotSize, 372, slotSize, true);
        default:
            return SlotDisplay();
        }
    }
    else
    {
        constexpr int slotSize = 18;
        switch (slot.m_group)
        {
        case Slot::OUTPUT:
            return SlotDisplay(144, 36);
        case Slot::INPUT:
            return SlotDisplay(88 + (slot.m_slot % 2) * slotSize, 26 + (slot.m_slot / 2) * slotSize);
        case Slot::ARMOR:
        {
            const ArmorSlot& armorSlot = (const ArmorSlot&)slot;
            return SlotDisplay(8, 8 + (Item::SLOT_HEAD - armorSlot.m_equipmentSlot) * slotSize, slotSize, false, 16 * ((Item::SLOT_HEAD - armorSlot.m_equipmentSlot) + 1) - 1);
        }
        case Slot::INVENTORY:
            return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 84 + ((slot.m_slot / 9) - 1) * slotSize, slotSize);
        case Slot::HOTBAR:
            return SlotDisplay(8 + (slot.m_slot % 9) * slotSize, 142, slotSize);
        default:
            return SlotDisplay();
        }
    }
}
