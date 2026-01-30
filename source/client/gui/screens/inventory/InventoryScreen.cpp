#include "InventoryScreen.hpp"
#include "client/renderer/Lighting.hpp"
#include "renderer/ShaderConstants.hpp"
#include "world/inventory/ArmorSlot.hpp"
//#include "stats/Achievement.hpp"

InventoryScreen::InventoryScreen(Player* player) : ContainerScreen(player->m_pInventoryMenu)
{
    //player->awardStat(Achievements::openInventory);
}

void InventoryScreen::init()
{
    m_uiProfile = m_pMinecraft->getOptions()->m_uiProfile;
    if (m_uiProfile == UI_LEGACY)
    {
        m_imageWidth = 432;
        m_imageHeight = 436;

        for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
        {
            Slot* slot = *it;
            slot->m_size = 42;

            if (slot->m_index == 0)
            {
                //result slot
                slot->m_x = m_width;
            }
            else if (slot->m_index <= 4)
            {
                //crafting grid
                slot->m_x = m_width;
            }
            else if (slot->m_index <= 5 + Item::SLOT_HEAD)
            {
                ArmorSlot* armorSlot = (ArmorSlot*) slot;
                slot->m_x = 127;
                slot->m_y = 29 + (Item::SLOT_HEAD - armorSlot->m_equipmentSlot) * slot->m_size;

                switch (armorSlot->m_equipmentSlot)
                {
                case Item::SLOT_FEET:
                    armorSlot->m_noItemTexture = "gui/container/feet_slot.png";
                    break;
                case Item::SLOT_LEGS:
                    armorSlot->m_noItemTexture = "gui/container/legs_slot.png";
                    break;
                case Item::SLOT_CHEST:
                    armorSlot->m_noItemTexture = "gui/container/body_slot.png";
                    break;
                case Item::SLOT_HEAD:
                    armorSlot->m_noItemTexture = "gui/container/head_slot.png";
                    break;
                default:
                    break;
                }
                
            }
            else if (slot->m_index <= 5 + Item::SLOT_HEAD + 9 * 3)
            {
                slot->m_x = 28 + (slot->m_slot % 9) * slot->m_size;
                slot->m_y = 233 + ((slot->m_slot / 9) - 1) * slot->m_size;
            }
            else
            {
                slot->m_x = 28 + (slot->m_slot % 9) * slot->m_size;
                slot->m_y = 372;
            }
        }
    }
    ContainerScreen::init();
    if (m_uiProfile == UI_LEGACY)
        m_topPos = Mth::Max(24, m_topPos - 48);
}

void InventoryScreen::renderBackground()
{
    if (!m_uiProfile == UI_LEGACY)
        ContainerScreen::renderBackground();
}

void InventoryScreen::_renderLabels()
{
    InventoryMenu* craftingMenu = (InventoryMenu*)m_pMenu;
    if (m_uiProfile == UI_LEGACY)
    {
        m_pFont->drawLegacy(m_pMinecraft->m_pLocalPlayer->m_pInventory->getName(), 27, 207, 0x404040);
    }
    else
        m_pFont->draw(craftingMenu->m_pCraftSlots->getName(), 86, 16, 0x404040);
}

void InventoryScreen::_renderBg(float partialTick)
{
    currentShaderColor = Color::WHITE;
    int playerX;
    int playerY;
    float scale;

    if (m_uiProfile == UI_LEGACY)
    {
        playerX = 243;
        playerY = 185;
        scale = 75.0f;
        blitNineSlice(*m_pMinecraft->m_pTextures, m_leftPos, m_topPos, m_imageWidth, m_imageHeight, 32, "gui/panel.png");
        int playerSlotX = m_leftPos + 180;
        int playerSlotY = m_topPos + 27;
        //For some reason, TU0 didn't had a texture for the player slot, but we do!
        blitTexture(*m_pMinecraft->m_pTextures, "gui/container/entity_slot.png", playerSlotX, playerSlotY, 0, 0, 126, 168);
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
