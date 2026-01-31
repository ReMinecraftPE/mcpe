/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ItemInHandRenderer.hpp"
#include "common/Mth.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "Lighting.hpp"

ItemInHandRenderer::Materials::Materials()
{
    MATERIAL_PTR(switchable, entity);
    MATERIAL_PTR(switchable, entity_alphatest);
    MATERIAL_PTR(switchable, item_in_hand);
    MATERIAL_PTR(switchable, entity_glint);
    MATERIAL_PTR(switchable, entity_alphatest_glint);
    MATERIAL_PTR(switchable, item_in_hand_glint);
}

ItemInHandRenderer::ItemInHandRenderer(Minecraft* pMC) :
	m_selectedItem(0, 1, 0),
	m_pMinecraft(pMC)
{
	m_lastSlot = -1;
	field_18 = 0;
	m_height = 0.0f;
	m_oHeight = 0.0f;
}

// This and itemUsed are probably leftovers from Minecraft Classic
void ItemInHandRenderer::itemPlaced()
{
	m_height = 0;
}

void ItemInHandRenderer::itemUsed()
{
	m_height = 0;
}

void ItemInHandRenderer::render(float a)
{
	LocalPlayer* pLP = m_pMinecraft->m_pLocalPlayer;

#ifndef FEATURE_GFX_SHADERS
    // Apply lighting
    {
        MatrixStack::Ref matrix = MatrixStack::World.push();
        matrix->rotate(pLP->m_oRot.y + (pLP->m_rot.y - pLP->m_oRot.y) * a, Vec3::UNIT_X);
        matrix->rotate(pLP->m_oRot.x + (pLP->m_rot.x - pLP->m_oRot.x) * a, Vec3::UNIT_Y);

        Lighting::turnOn(matrix);
    }
#endif

    MatrixStack::Ref matrix = MatrixStack::World.push();

	if (m_pMinecraft->getOptions()->m_bDynamicHand && m_pMinecraft->m_pCameraEntity == pLP)
	{
		float rYaw   = Mth::Lerp(pLP->m_lastRenderArmRot.x, pLP->m_renderArmRot.x, a);
		float rPitch = Mth::Lerp(pLP->m_lastRenderArmRot.y, pLP->m_renderArmRot.y, a);
		matrix->rotate((pLP->m_rot.y - rPitch) * 0.1f, Vec3::UNIT_X);
		matrix->rotate((pLP->m_rot.x - rYaw  ) * 0.1f, Vec3::UNIT_Y);
	}

	float fBright = m_pMinecraft->m_pLevel->getBrightness(pLP->m_pos);
    currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color(fBright, fBright, fBright);

	ItemStack* pItem = &m_selectedItem;
	/*if (pLP->m_fishing != null)
    {
		pItem = new ItemStack(Item::stick);
	}*/
    
    float swing2, swing3;
    float fAnim = pLP->getAttackAnim(a);
    float h = m_oHeight + (m_height - m_oHeight) * a;
    constexpr float d = 0.8f;
    
	if (!ItemStack::isEmpty(pItem))
	{
        matrix->translate(Vec3(-0.4f * Mth::sin(float(M_PI) * Mth::sqrt(fAnim)), 0.2f * Mth::sin(2.0f * float(M_PI) * Mth::sqrt(fAnim)), -0.2f * Mth::sin(float(M_PI) * fAnim)));
        matrix->translate(Vec3(0.7f * d, -0.65f * d - (1.0f - h) * 0.6f, -0.9f * d));
        matrix->rotate(45.0f, Vec3::UNIT_Y);

#if MCE_GFX_API_OGL && !defined(FEATURE_GFX_SHADERS)
        glEnable(GL_RESCALE_NORMAL);
#endif

        swing3 = Mth::sin(float(M_PI) * fAnim * fAnim);
        swing2 = Mth::sin(float(M_PI) * Mth::sqrt(fAnim));

        matrix->rotate(swing3 * -20.0f, Vec3::UNIT_Y);
        matrix->rotate(swing2 * -20.0f, Vec3::UNIT_Z);
        matrix->rotate(swing2 * -80.0f, Vec3::UNIT_X);
        matrix->scale(0.4f);

        if (pItem->getItem()->isMirroredArt())
        {
            matrix->rotate(180.0f, Vec3::UNIT_Y);
        }

        renderItem(*pLP, *pItem, a);
	}
	else
	{
        matrix->translate(Vec3(-0.3f * Mth::sin(float(M_PI) * Mth::sqrt(fAnim)), 0.4f * Mth::sin(2.0f * float(M_PI) * Mth::sqrt(fAnim)), -0.4f * Mth::sin(float(M_PI) * fAnim)));
        matrix->translate(Vec3(0.8f * d, -0.75f * d - (1.0f - h) * 0.6f, -0.9f * d));
        matrix->rotate(45.0f, Vec3::UNIT_Y);
#if MCE_GFX_API_OGL && !defined(FEATURE_GFX_SHADERS)
        glEnable(GL_RESCALE_NORMAL);
#endif

        matrix->rotate(Mth::sin(float(M_PI) * Mth::sqrt(fAnim)) * 70.0f, Vec3::UNIT_Y);
        matrix->rotate(Mth::sin(float(M_PI) * fAnim * fAnim) * -20.0f, Vec3::UNIT_Z);

        m_pMinecraft->m_pTextures->loadAndBindTexture("mob/char.png");
        matrix->translate(Vec3(-1.0f, 3.6f, 3.5f));
        matrix->rotate(120.0f, Vec3::UNIT_Z);
        matrix->rotate(200.0f, Vec3::UNIT_X);
        matrix->rotate(-135.0f, Vec3::UNIT_Y);
        matrix->scale(1.0f);
        matrix->translate(Vec3(5.6f, 0.0f, 0.0f));

        HumanoidMobRenderer* pRenderer = (HumanoidMobRenderer*)EntityRenderDispatcher::getInstance()->getRenderer(*pLP);
        swing2 = 1.0f;
        matrix->scale(swing2);
        pRenderer->renderHand(*pLP, a);
	}

#if MCE_GFX_API_OGL && !defined(FEATURE_GFX_SHADERS)
    glDisable(GL_RESCALE_NORMAL);
#endif
	Lighting::turnOff();
}

#ifdef ENH_SHADE_HELD_TILES
#define SHADE_IF_NEEDED(col) t.color(col*bright,col*bright,col*bright,1.0f)
#else
#define SHADE_IF_NEEDED(col)
#endif

void ItemInHandRenderer::renderItem(const Entity& entity, const ItemStack& item, float a)
{
    if (item.isEmpty())
        return;

#ifdef ENH_SHADE_HELD_TILES
    float bright = entity.getBrightness(a);
#endif

    _setupShaderParameters(entity, Color::NIL, a);

    Tile* pTile = item.getTile();
    if (pTile && TileRenderer::canRender(pTile->getRenderShape()))
    {
        currentShaderColor = Color::WHITE;
        currentShaderDarkColor = Color::WHITE;
        
        m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
        
#ifdef ENH_SHADE_HELD_TILES
#	define ARGPATCH , bright
#else
#	define ARGPATCH
#endif
        
        m_tileRenderer.renderTile(FullTile(pTile, item.getAuxValue()), m_materials.item_in_hand ARGPATCH);
        
#ifdef ARGPATCH
#	undef ARGPATCH
#endif
        
    }
    else
    {

        MatrixStack::Ref matrix = MatrixStack::World.push();

        std::string toBind;
        if (pTile)
            toBind = C_TERRAIN_NAME;
        else
            toBind = C_ITEMS_NAME;
        m_pMinecraft->m_pTextures->loadAndBindTexture(toBind);
        
        constexpr float C_RATIO     = 1.0f / 256.0f;
        constexpr float C_RATIO_2   = 1.0f / 512.0f;
        constexpr float C_ONE_PIXEL = 1.0f / 16.0f;
        
        int textureX = item.getIcon() % 16 * 16;
        int textureY = item.getIcon() / 16 * 16;
        
        float texU_1 = C_RATIO * float(textureX + 0.0f);
        float texU_2 = C_RATIO * float(textureX + 15.99f);
        float texV_1 = C_RATIO * float(textureY + 0.0f);
        float texV_2 = C_RATIO * float(textureY + 15.99f);
        
        Tesselator& t = Tesselator::instance;

        matrix->translate(Vec3(-0.0f, -0.3f, 0.0f));
        matrix->scale(1.5f);
        matrix->rotate(50.0f, Vec3::UNIT_Y);
        matrix->rotate(335.0f, Vec3::UNIT_Z);
        matrix->translate(Vec3(-0.9375f, -0.0625f, 0.0f));
        /*matrix->translate(Vec3(0.075f, -0.245f, -0.1f));
        matrix->scale(0.0625f);
        matrix->rotate(90.0f, Vec3::UNIT_Z);
        matrix->rotate(-90.0f, Vec3::UNIT_X);
        matrix->rotate(-90.0f, Vec3::UNIT_Y);
        matrix->translate(Vec3(0.0f, 0.0f, -16.0f));*/
        
        t.begin(264);
        SHADE_IF_NEEDED(1.0f);
        
        t.normal(Vec3::UNIT_Z);
        t.vertexUV(0.0f, 0.0f, 0.0f,         texU_2, texV_2);
        t.vertexUV(1.0f, 0.0f, 0.0f,         texU_1, texV_2);
        t.vertexUV(1.0f, 1.0f, 0.0f,         texU_1, texV_1);
        t.vertexUV(0.0f, 1.0f, 0.0f,         texU_2, texV_1);
        
        t.normal(Vec3::NEG_UNIT_Z);
        t.vertexUV(0.0f, 1.0f, -C_ONE_PIXEL, texU_2, texV_1);
        t.vertexUV(1.0f, 1.0f, -C_ONE_PIXEL, texU_1, texV_1);
        t.vertexUV(1.0f, 0.0f, -C_ONE_PIXEL, texU_1, texV_2);
        t.vertexUV(0.0f, 0.0f, -C_ONE_PIXEL, texU_2, texV_2);
        
        SHADE_IF_NEEDED(0.8f);
        t.normal(Vec3::NEG_UNIT_X);
        for (int i = 0; i < 16; i++)
        {
            t.vertexUV(i * C_ONE_PIXEL, 0.0f, -C_ONE_PIXEL, Mth::Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
            t.vertexUV(i * C_ONE_PIXEL, 0.0f, 0.0f,         Mth::Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
            t.vertexUV(i * C_ONE_PIXEL, 1.0f, 0.0f,         Mth::Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
            t.vertexUV(i * C_ONE_PIXEL, 1.0f, -C_ONE_PIXEL, Mth::Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
        }
        for (int i = 0; i < 16; i++)
        {
            t.vertexUV((i + 1) * C_ONE_PIXEL, 1.0f, -C_ONE_PIXEL, Mth::Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
            t.vertexUV((i + 1) * C_ONE_PIXEL, 1.0f, 0.0f,         Mth::Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
            t.vertexUV((i + 1) * C_ONE_PIXEL, 0.0f, 0.0f,         Mth::Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
            t.vertexUV((i + 1) * C_ONE_PIXEL, 0.0f, -C_ONE_PIXEL, Mth::Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
        }
        
        SHADE_IF_NEEDED(0.6f);
        for (int i = 0; i < 16; i++)
        {
            t.vertexUV(0.0f, (i + 1) * C_ONE_PIXEL, 0.0f,         texU_2, Mth::Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(1.0f, (i + 1) * C_ONE_PIXEL, 0.0f,         texU_1, Mth::Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(1.0f, (i + 1) * C_ONE_PIXEL, -C_ONE_PIXEL, texU_1, Mth::Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(0.0f, (i + 1) * C_ONE_PIXEL, -C_ONE_PIXEL, texU_2, Mth::Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
        }
        for (int i = 0; i < 16; i++)
        {
            t.vertexUV(1.0f, i * C_ONE_PIXEL, 0.0f,         texU_1, Mth::Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(0.0f, i * C_ONE_PIXEL, 0.0f,         texU_2, Mth::Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(0.0f, i * C_ONE_PIXEL, -C_ONE_PIXEL, texU_2, Mth::Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
            t.vertexUV(1.0f, i * C_ONE_PIXEL, -C_ONE_PIXEL, texU_1, Mth::Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
        }
        
        t.draw(m_materials.item_in_hand);
    }
}

void ItemInHandRenderer::renderScreenEffect(float a)
{
    LocalPlayer* player = m_pMinecraft->m_pLocalPlayer;
    Textures* textures = m_pMinecraft->m_pTextures;
    Level* level = m_pMinecraft->m_pLevel;

    if (player->isOnFire())
    {
        textures->loadAndBindTexture(C_TERRAIN_NAME);
        renderFire(a);
    }

    if (player->isInWall() && !m_pMinecraft->getOptions()->m_bFlyCheat)
    {
        textures->loadAndBindTexture(C_TERRAIN_NAME);

        Tile* pTile = Tile::tiles[level->getTile(player->m_pos)];
        if (pTile)
        {
            int texture = pTile->getTexture(Facing::NORTH);
            renderTex(a, texture);
        }
    }

    if (player->isUnderLiquid(Material::water))
    {
        if (textures->loadAndBindTexture("/misc/water.png", false))
        {
            renderWater(a);
        }
    }
}

void ItemInHandRenderer::renderWater(float a)
{
    ScreenRenderer& screenRenderer = ScreenRenderer::singleton();
    LocalPlayer& player = *m_pMinecraft->m_pLocalPlayer;

    float br = player.getBrightness(a);
    currentShaderColor = Color(br, br, br, 0.5f);
    currentShaderDarkColor = Color::WHITE;
    MatrixStack::Ref matrix = MatrixStack::World.push();

    constexpr float size = 4.0f;
    constexpr float x0  = -1.0f;
    constexpr float x1  =  1.0f;
    constexpr float y0  = -1.0f;
    constexpr float y1  =  1.0f;
    constexpr float z0  = -0.5f;
    float uo = -player.m_rot.x / 64.0f;
    float vo = player.m_rot.y / 64.0f;
    Tesselator& t = Tesselator::instance;
    t.begin(4);
    t.vertexUV(x0, y0, z0, (size + uo), (size + vo));
    t.vertexUV(x1, y0, z0, (0.0f + uo), (size + vo));
    t.vertexUV(x1, y1, z0, (0.0f + uo), (0.0f + vo));
    t.vertexUV(x0, y1, z0, (size + uo), (0.0f + vo));
	t.draw(screenRenderer.m_materials.ui_textured_and_glcolor);
}

void ItemInHandRenderer::renderFire(float a)
{
    currentShaderColor = Color(1.0f, 1.0f, 1.0f, 0.9f);
    currentShaderDarkColor = Color::WHITE;
	for (int i = 0; i < 2; i++)
	{
        MatrixStack::Ref matrix = MatrixStack::World.push();
		int texture = Tile::fire->m_TextureFrame + i * 16;

		float texX = 16.0f * float(texture % 16), texY = 16.0f * float(texture / 16);
		float texU_1 =  texX           / 256.0f;
		float texU_2 = (texX + 15.99f) / 256.0f;
		float texV_1 =  texY           / 256.0f;
		float texV_2 = (texY + 15.99f) / 256.0f;

		matrix->translate(Vec3(float(-(i * 2 - 1)) * 0.24f, -0.3f, 0.0f));
		matrix->rotate(float(-(i * 2 - 1)) * 10.0f, Vec3::UNIT_Y);

        ScreenRenderer& screenRenderer = ScreenRenderer::singleton();
        screenRenderer.blitRaw(-0.5f, 0.5f, -0.5f, 0.5f, -0.5f, texU_1, texU_2, texV_1, texV_2);
	}
}

void ItemInHandRenderer::renderTex(float a, int texture)
{
    ScreenRenderer& screenRenderer = ScreenRenderer::singleton();

	//m_pMinecraft->m_pLocalPlayer->getBrightness(a);
    constexpr float br = 0.1f; // 0.3f on PE 0.12.1
    currentShaderColor = Color(br, br, br); // Java passed 0.5f for transparency, but this never actually worked
    currentShaderDarkColor = Color::WHITE;
    MatrixStack::Ref matrix = MatrixStack::World.push();

	// @BUG: The texture x/y isn't multiplied by 16. This causes some weird textures to show up instead of the correct ones.
#ifdef ORIGINAL_CODE
#  define MULT
#else
#  define MULT 16 *
#endif
	float texX = MULT float(texture % 16), texY = MULT float(texture / 16);
	float texU_1 =  texX           / 256.0f - 1 / 128.0f;
	float texU_2 = (texX + 15.99f) / 256.0f + 1 / 128.0f;
	float texV_1 =  texY           / 256.0f - 1 / 128.0f;
	float texV_2 = (texY + 15.99f) / 256.0f + 1 / 128.0f;

	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.vertexUV(-1.0f, -1.0f, -0.5f, texU_2, texV_2);
	t.vertexUV(+1.0f, -1.0f, -0.5f, texU_1, texV_2);
	t.vertexUV(+1.0f, +1.0f, -0.5f, texU_1, texV_1);
	t.vertexUV(-1.0f, +1.0f, -0.5f, texU_2, texV_1);
	t.draw(screenRenderer.m_materials.ui_textured_and_glcolor);
}

void ItemInHandRenderer::tick()
{
	m_oHeight = m_height;

	ItemStack& item = m_pMinecraft->m_pLocalPlayer->m_pInventory->getSelectedItem();

	bool bSameItem = m_pMinecraft->m_pLocalPlayer->m_pInventory->m_selectedSlot == m_lastSlot && m_selectedItem == item;

	if (item.isEmpty() && m_selectedItem.isEmpty())
		bSameItem = true;

    // This isn't really needed anymore
    //if (!item.isEmpty() && !m_selectedItem.isEmpty())
    //{
    //    if (&item != &m_selectedItem && item.getId() == m_selectedItem.getId() && item.getAuxValue() == m_selectedItem.getAuxValue())
    //    {
    //        bSameItem = true;
    //        m_selectedItem = ItemStack(item);
    //    }
    //}

	float b = bSameItem ? 1.0f : 0.0f;

	float a = b - m_height;
	if (a < -0.4f)
		a = -0.4f;
	if (a >= 0.4f)
		a = 0.4f;

	m_height += a;

	if (m_height < 0.1f)
	{
		m_selectedItem = ItemStack(item);
		m_lastSlot = m_pMinecraft->m_pLocalPlayer->m_pInventory->m_selectedSlot;
	}
}

void ItemInHandRenderer::turn(const Vec2& rot)
{
}

