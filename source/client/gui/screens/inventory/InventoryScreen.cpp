#include "InventoryScreen.hpp"
#include "client/renderer/Lighting.hpp"
#include "renderer/ShaderConstants.hpp"
//#include "stats/Achievement.hpp"

InventoryScreen::InventoryScreen(Player* player) : ContainerScreen(player->m_pInventoryMenu)
{
    //player->awardStat(Achievements::openInventory);
}

void InventoryScreen::_renderLabels()
{
    InventoryMenu* craftingMenu = (InventoryMenu*)m_pMenu;
	m_pFont->draw(craftingMenu->m_pCraftSlots->getName(), 86, 16, 0x404040);
}

void InventoryScreen::_renderBg(float partialTick)
{
    currentShaderColor = Color::WHITE;

    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/inventory.png");

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 0, 0);

#if MCE_GFX_API_OGL && !defined(FEATURE_GFX_SHADERS)
    glEnable(GL_RESCALE_NORMAL);
#endif
    MatrixStack::Ref matrix = MatrixStack::World.push();

    matrix->translate(Vec3(m_leftPos + 51.0f, m_topPos + 75.0f, 50.0f));
    const float scale = 30.0f;
    matrix->scale(Vec3(-scale, scale, scale));
    matrix->rotate(180.0f, Vec3(0.0f, 0.0f, 1.0f));

    LocalPlayer* player = m_pMinecraft->m_pLocalPlayer;

    float prevYBodyRot = player->m_yBodyRot;
    float prevXRot = player->m_rot.x;
    float prevYRot = player->m_rot.y;

    float dx = m_leftPos + 51.0f - m_menuPointer.x;
    float dy = m_topPos + 75 - 50 - m_menuPointer.y;

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
