/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ItemInHandRenderer.hpp"
#include "client/app/Minecraft.hpp"

ItemInHandRenderer::ItemInHandRenderer(Minecraft* pMC) :
	m_ItemInstance(0, 1, 0),
	m_pMinecraft(pMC)
{
	field_0 = -1;
	field_18 = 0;
	field_1C = 0.0f;
	field_20 = 0.0f;
}

// This and itemUsed are probably leftovers from Minecraft Classic
void ItemInHandRenderer::itemPlaced()
{
	field_1C = 0;
}

void ItemInHandRenderer::itemUsed()
{
	field_1C = 0;
}

#ifdef ENH_SHADE_HELD_TILES
#define SHADE_IF_NEEDED(col) t.color(col*bright,col*bright,col*bright,1.0f)
#else
#define SHADE_IF_NEEDED(col)
#endif

void ItemInHandRenderer::renderItem(ItemInstance* inst)
{
#ifndef ORIGINAL_CODE
	if (inst->m_itemID < 0)
		return;
#endif

	glPushMatrix();
#ifdef ENH_SHADE_HELD_TILES
	float bright = m_pMinecraft->m_pLocalPlayer->getBrightness(0.0f);
#endif

	if (inst->m_itemID <= C_MAX_TILES && TileRenderer::canRender(Tile::tiles[inst->m_itemID]->getRenderShape()))
	{
		float red, grn, blu, alp = 1.0f;

		if (inst->m_itemID == Tile::leaves->m_ID)
		{
			red = 0.35f;
			grn = 0.65f;
			blu = 0.25f;
		}
		else
		{
			blu = grn = red = 1.0f;
		}

		glColor4f(red, grn, blu, alp);

		m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

#ifdef ENH_SHADE_HELD_TILES
#	define ARGPATCH , bright
#else
#	define ARGPATCH
#endif

		m_tileRenderer.renderTile(Tile::tiles[inst->m_itemID], inst->getAuxValue() ARGPATCH);

#ifdef ARGPATCH
#	undef ARGPATCH
#endif

		glPopMatrix();
		return;
	}

	std::string toBind;
	if (inst->m_itemID <= C_MAX_TILES)
		toBind = C_TERRAIN_NAME;
	else
		toBind = "gui/items.png";
	m_pMinecraft->m_pTextures->loadAndBindTexture(toBind);

	constexpr float C_RATIO     = 1.0f / 256.0f;
	constexpr float C_RATIO_2   = 1.0f / 512.0f;
	constexpr float C_ONE_PIXEL = 1.0f / 16.0f;

	int textureX = inst->getIcon() % 16 * 16;
	int textureY = inst->getIcon() / 16 * 16;

	float texU_1 = C_RATIO * float(textureX + 0.0f);
	float texU_2 = C_RATIO * float(textureX + 15.99f);
	float texV_1 = C_RATIO * float(textureY + 0.0f);
	float texV_2 = C_RATIO * float(textureY + 15.99f);

	Tesselator& t = Tesselator::instance;
	glTranslatef(-0.0f, -0.3f, 0.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(335.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.9375f, -0.0625f, 0.0f);

	t.begin();
	SHADE_IF_NEEDED(1.0f);

	t.vertexUV(0.0f, 0.0f, 0.0f,         texU_2, texV_2);
	t.vertexUV(1.0f, 0.0f, 0.0f,         texU_1, texV_2);
	t.vertexUV(1.0f, 1.0f, 0.0f,         texU_1, texV_1);
	t.vertexUV(0.0f, 1.0f, 0.0f,         texU_2, texV_1);
	t.vertexUV(0.0f, 1.0f, -C_ONE_PIXEL, texU_2, texV_1);
	t.vertexUV(1.0f, 1.0f, -C_ONE_PIXEL, texU_1, texV_1);
	t.vertexUV(1.0f, 0.0f, -C_ONE_PIXEL, texU_1, texV_2);
	t.vertexUV(0.0f, 0.0f, -C_ONE_PIXEL, texU_2, texV_2);

	SHADE_IF_NEEDED(0.8f);
	for (int i = 0; i < 16; i++)
	{
		t.vertexUV(i * C_ONE_PIXEL, 0.0f, -C_ONE_PIXEL, Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
		t.vertexUV(i * C_ONE_PIXEL, 0.0f, 0.0f,         Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
		t.vertexUV(i * C_ONE_PIXEL, 1.0f, 0.0f,         Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
		t.vertexUV(i * C_ONE_PIXEL, 1.0f, -C_ONE_PIXEL, Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
	}
	for (int i = 0; i < 16; i++)
	{
		t.vertexUV((i + 1) * C_ONE_PIXEL, 1.0f, -C_ONE_PIXEL, Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
		t.vertexUV((i + 1) * C_ONE_PIXEL, 1.0f, 0.0f,         Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_1);
		t.vertexUV((i + 1) * C_ONE_PIXEL, 0.0f, 0.0f,         Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
		t.vertexUV((i + 1) * C_ONE_PIXEL, 0.0f, -C_ONE_PIXEL, Lerp(texU_2, texU_1, i * C_ONE_PIXEL) - C_RATIO_2, texV_2);
	}

	SHADE_IF_NEEDED(0.6f);
	for (int i = 0; i < 16; i++)
	{
		t.vertexUV(0.0f, (i + 1) * C_ONE_PIXEL, 0.0f,         texU_2, Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
		t.vertexUV(1.0f, (i + 1) * C_ONE_PIXEL, 0.0f,         texU_1, Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
		t.vertexUV(1.0f, (i + 1) * C_ONE_PIXEL, -C_ONE_PIXEL, texU_1, Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
		t.vertexUV(0.0f, (i + 1) * C_ONE_PIXEL, -C_ONE_PIXEL, texU_2, Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
	}
	for (int i = 0; i < 16; i++)
	{
		t.vertexUV(1.0f, i * C_ONE_PIXEL, 0.0f,         texU_1, Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
		t.vertexUV(0.0f, i * C_ONE_PIXEL, 0.0f,         texU_2, Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
		t.vertexUV(0.0f, i * C_ONE_PIXEL, -C_ONE_PIXEL, texU_2, Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
		t.vertexUV(1.0f, i * C_ONE_PIXEL, -C_ONE_PIXEL, texU_1, Lerp(texV_2, texV_1, i * C_ONE_PIXEL));
	}

	t.draw();
	glPopMatrix();
}

void ItemInHandRenderer::render(float f)
{
	LocalPlayer* pLP = m_pMinecraft->m_pLocalPlayer;

	float f1 = field_20 + (field_1C - field_20) * f;
	glPushMatrix();
	glRotatef(pLP->field_60 + (pLP->m_pitch - pLP->field_60) * f, 1.0f, 0.0f, 0.0f);
	glRotatef(pLP->field_5C + (pLP->m_yaw   - pLP->field_5C) * f, 0.0f, 1.0f, 0.0f);
	glPopMatrix();//huh?

	float fBright = m_pMinecraft->m_pLevel->getBrightness(Mth::floor(pLP->m_pos.x), Mth::floor(pLP->m_pos.y), Mth::floor(pLP->m_pos.z));
	glColor4f(fBright, fBright, fBright, 1.0f);

	if (m_ItemInstance.m_itemID <= 0)
	{
		glPushMatrix();
		float fAnim = pLP->getAttackAnim(f);

		glTranslatef(-0.3f * Mth::sin(float(M_PI) * Mth::sqrt(fAnim)), 0.4f * Mth::sin(2.0f * float(M_PI) * Mth::sqrt(fAnim)), -0.4f * Mth::sin(float(M_PI) * fAnim));
		glTranslatef(0.64f, ((1.0f - f1) * -0.6f) - 0.6f, -0.72f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		glEnable(GL_RESCALE_NORMAL);

		// @HUH: refetch
		fAnim = pLP->getAttackAnim(f);

		glRotatef(Mth::sin(float(M_PI) * Mth::sqrt(fAnim)) * 70.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(Mth::sin(float(M_PI) * fAnim * fAnim) * -20.0f, 0.0f, 0.0f, 1.0f);

		m_pMinecraft->m_pTextures->loadAndBindTexture("mob/char.png");
		glTranslatef(-1.0f, 3.6f, 3.5f);
		glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(200.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-135.0f, 0.0f, 1.0f, 0.0f);
		glScalef(1.0f, 1.0f, 1.0f);
		glTranslatef(5.6f, 0.0f, 0.0f);

		HumanoidMobRenderer* pRenderer = (HumanoidMobRenderer*)EntityRenderDispatcher::getInstance()->getRenderer(m_pMinecraft->m_pLocalPlayer);
		glScalef(1.0f, 1.0f, 1.0f);
		pRenderer->renderHand();

		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		float fAnim = pLP->getAttackAnim(f);

		glTranslatef(-0.4f * Mth::sin(float(M_PI) * Mth::sqrt(fAnim)), 0.2f * Mth::sin(2.0f * float(M_PI) * Mth::sqrt(fAnim)), -0.2f * Mth::sin(float(M_PI) * fAnim));
		glTranslatef(0.56f, ((1.0f - f1) * -0.6f) - 0.52f, -0.72f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		glEnable(GL_RESCALE_NORMAL);

		// @HUH: refetch
		fAnim = pLP->getAttackAnim(f);

		float sin1 = Mth::sin(float(M_PI) * Mth::sqrt(fAnim));
		float sin2 = Mth::sin(float(M_PI) * fAnim * fAnim);

		glRotatef(sin2 * -20.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(sin1 * -20.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(sin1 * -80.0f, 1.0f, 0.0f, 0.0f);
		glScalef(0.4f, 0.4f, 0.4f);

		if (m_ItemInstance.getItem()->isMirroredArt())
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

		renderItem(&m_ItemInstance);
		glPopMatrix();
	}

	glDisable(GL_RESCALE_NORMAL);
}

void ItemInHandRenderer::renderFire(float f)
{
	glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 1, offset = 0; i != -3; i -= 2, offset += 16)
	{
		glPushMatrix();
		int texture = offset + Tile::fire->m_TextureFrame;

		float texX = 16.0f * float(texture % 16), texY = 16.0f * float(texture / 16);
		float texU_1 =  texX           / 256.0f;
		float texU_2 = (texX + 15.99f) / 256.0f;
		float texV_1 =  texY           / 256.0f;
		float texV_2 = (texY + 15.99f) / 256.0f;

		glTranslatef(float(i) * 0.24f, -0.3f, 0.0f);
		glRotatef(float(i) * 10.0f, 0.0f, 1.0f, 0.0f);

		Tesselator& t = Tesselator::instance;
		t.begin();
		t.vertexUV(-0.5f, -0.5f, -0.5f, texU_2, texV_2);
		t.vertexUV(+0.5f, -0.5f, -0.5f, texU_1, texV_2);
		t.vertexUV(+0.5f, +0.5f, -0.5f, texU_1, texV_1);
		t.vertexUV(-0.5f, +0.5f, -0.5f, texU_2, texV_1);
		t.draw();

		glPopMatrix();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
}

void ItemInHandRenderer::renderTex(float f, int texture)
{
	//float brightness = m_pMinecraft->m_pLocalPlayer->getBrightness(f);
	glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
	glPushMatrix();

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
	t.begin();
	t.vertexUV(-1.0f, -1.0f, -0.5f, texU_2, texV_2);
	t.vertexUV(+1.0f, -1.0f, -0.5f, texU_1, texV_2);
	t.vertexUV(+1.0f, +1.0f, -0.5f, texU_1, texV_1);
	t.vertexUV(-1.0f, +1.0f, -0.5f, texU_2, texV_1);
	t.draw();

	glPopMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void ItemInHandRenderer::tick()
{
	field_20 = field_1C;

	int itemID = m_pMinecraft->m_pLocalPlayer->m_pInventory->getSelectedItemId();

	bool bSameItem = itemID == m_ItemInstance.m_itemID;

	float b = bSameItem ? 1.0f : 0.0f;

	float a = b - field_1C;
	if (a < -0.4f)
		a = -0.4f;
	if (a >= 0.4f)
		a  = 0.4f;

	field_1C += a;

	if (field_1C < 0.1f)
		m_ItemInstance.m_itemID = itemID;
}

void ItemInHandRenderer::renderScreenEffect(float f)
{
	glDisable(GL_ALPHA_TEST);

	if (m_pMinecraft->m_pLocalPlayer->isOnFire())
	{
		m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		renderFire(f);
	}

	if (m_pMinecraft->m_pLocalPlayer->isInWall() && !m_pMinecraft->getOptions()->m_bFlyCheat)
	{
		int fx = Mth::floor(m_pMinecraft->m_pLocalPlayer->m_pos.x);
		int fy = Mth::floor(m_pMinecraft->m_pLocalPlayer->m_pos.y);
		int fz = Mth::floor(m_pMinecraft->m_pLocalPlayer->m_pos.z);
		m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		
		Tile* pTile = Tile::tiles[m_pMinecraft->m_pLevel->getTile(fx, fy, fz)];
		if (pTile)
		{
			int texture = pTile->getTexture(DIR_ZNEG);
			renderTex(f, texture);
		}
	}

	glEnable(GL_ALPHA_TEST);
}

