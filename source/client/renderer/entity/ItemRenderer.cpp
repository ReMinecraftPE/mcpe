/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ItemRenderer.hpp"
#include "EntityRenderDispatcher.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "world/entity/ItemEntity.hpp"

TileRenderer* ItemRenderer::tileRenderer = new TileRenderer;

#ifndef ENH_3D_INVENTORY_TILES
const uint8_t g_ItemFrames[C_MAX_TILES] =
{
	0,  1,  2,  3,  4,  5,  0,  6,  0,  0,  0,  0,  7,  8,  9,  10,
	11, 12, 13, 0,  14, 15, 0,  0,  16, 0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  17, 0,  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 0,
	0,  28, 29, 0,  0,  30, 0,  0,  31, 32, 0,  0,  33, 0,  0,  0,
	34, 35, 0,  36, 0,  0,  0,  37, 0,  38, 39, 0,  0,  0,  40, 41,
	0,  0,  42, 43, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
	55, 56, 57, 58, 0
};
#endif

ItemRenderer::ItemRenderer()
{
	m_shadowRadius = 0.15f;
	m_shadowStrength = 0.75f;
}

void ItemRenderer::render(Entity* pEntity, const Vec3& pos, float rot, float a)
{
	m_random.init_genrand(187);
	ItemEntity* pItemEntity = (ItemEntity*)pEntity;

	glPushMatrix();
	float yOffset = Mth::sin((float(pItemEntity->m_age) + a) / 10.0f + pItemEntity->m_bobOffs);
	const ItemInstance* pItemInstance = pItemEntity->m_pItemInstance;
	if (ItemInstance::isNull(pItemInstance))
	{
		assert(!"Tried to render invalid ItemInstance for ItemEntity");
		return;
	}

	int itemsToRender = 1;
	if (pItemInstance->m_count > 1)
		itemsToRender = 2;
	if (pItemInstance->m_count > 5)
		itemsToRender = 3;
	if (pItemInstance->m_count > 20)
		itemsToRender = 4;

	glTranslatef(pos.x, pos.y + 0.1f + yOffset * 0.1f, pos.z);
	glEnable(GL_RESCALE_NORMAL);

	Tile* pTile = pItemInstance->getTile();
	if (pTile && TileRenderer::canRender(pTile->getRenderShape()))
	{
		glRotatef(((float(pItemEntity->m_age) + a) / 20.0f + pItemEntity->m_bobOffs) * 57.296f, 0.0f, 1.0f, 0.0f);
		bindTexture(C_TERRAIN_NAME);

		float scale = 0.5f;

		// @BUG: If cacti existed and were able to be dropped, they would be 2x the size of a regular tile.
		// This bug has been in the main game until Java Edition Beta 1.8.
		if (pTile->isCubeShaped() || pTile == Tile::stoneSlabHalf)
			scale = 0.25f;

		glScalef(scale, scale, scale);

		for (int i = 0; i < itemsToRender; i++)
		{
			glPushMatrix();
			if (i != 0)
			{
				glTranslatef(
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale);
			}

			tileRenderer->renderTile(pTile, pItemInstance->getAuxValue(), pItemEntity->getBrightness(1.0f));
			glPopMatrix();
		}
	}
	else
	{
		glScalef(0.5f, 0.5f, 0.5f);
		int icon = pItemInstance->getIcon();

		bindTexture(pItemInstance->getTile() ? C_TERRAIN_NAME : C_ITEMS_NAME);

		for (int i = 0; i < itemsToRender; i++)
		{
			glPushMatrix();
			if (i != 0)
			{
				glTranslatef(
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f);
			}

			glRotatef(180.0f - m_pDispatcher->m_rot.x, 0.0f, 1.0f, 0.0f);

			Tesselator& t = Tesselator::instance;
			t.begin();

#ifdef ENH_SHADE_HELD_TILES
			float bright = pItemEntity->getBrightness(1.0f);
			t.color(bright, bright, bright);
#endif
			t.normal(0.0f, 1.0f, 0.0f);
			t.vertexUV(-0.5f, -0.25f, 0.0f, float(16 * (icon % 16))     / 256.0f, float(16 * (icon / 16 + 1)) / 256.0f);
			t.vertexUV(+0.5f, -0.25f, 0.0f, float(16 * (icon % 16 + 1)) / 256.0f, float(16 * (icon / 16 + 1)) / 256.0f);
			t.vertexUV(+0.5f, +0.75f, 0.0f, float(16 * (icon % 16 + 1)) / 256.0f, float(16 * (icon / 16))     / 256.0f);
			t.vertexUV(-0.5f, +0.75f, 0.0f, float(16 * (icon % 16))     / 256.0f, float(16 * (icon / 16))     / 256.0f);

			t.draw();

			glPopMatrix();
		}
	}

	glDisable(GL_RESCALE_NORMAL);
	glPopMatrix();
}

void ItemRenderer::blitRect(Tesselator& t, int x, int y, int w, int h, int color)
{
	t.begin();
	t.color(color);
	t.vertex(float(x),     float(y),     0.0f);
	t.vertex(float(x),     float(y + h), 0.0f);
	t.vertex(float(x + w), float(y + h), 0.0f);
	t.vertex(float(x + w), float(y),     0.0f);
	t.draw();
}

void ItemRenderer::blit(int dx, int dy, int sx, int sy, int tw, int th)
{
	Tesselator& t = Tesselator::instance;

	float ex = float(dx), ey = float(dy);
	float uw = float(tw), uh = float(th);
	float vx = float(sx), vy = float(sy);

	t.begin();
	t.vertexUV(ex,      ey + uh, 0.0f, float(vx)      / 256.0f, float(vy + uh) / 256.0f);
	t.vertexUV(ex + uw, ey + uh, 0.0f, float(vx + uw) / 256.0f, float(vy + uh) / 256.0f);
	t.vertexUV(ex + uw, ey,      0.0f, float(vx + uw) / 256.0f, float(vy)      / 256.0f);
	t.vertexUV(ex,      ey,      0.0f, float(vx)      / 256.0f, float(vy)      / 256.0f);
	t.draw();
}

void ItemRenderer::renderGuiItemOverlay(Font* font, Textures* textures, ItemInstance* instance, int x, int y)
{
	if (!instance)
		return;

	if (instance->m_count == 1)
		return;

	std::stringstream ss;
	ss << instance->m_count;
	std::string amtstr = ss.str();

	int width = font->width(amtstr), height = font->height(amtstr) + 8;

	font->drawShadow(amtstr, x + 17 - width, y + 17 - height, 0xFFFFFF);
}

void ItemRenderer::renderGuiItem(Font* font, Textures* textures, ItemInstance* instance, int x, int y, bool b)
{
	// @NOTE: Font unused but would presumably be used to draw the item amount.
	// As if that actually works due to us blocking t.begin() and t.draw() calls...
	if (!instance || !instance->isValid())
		return;

	if (!b)
		return;

	Item* pItem = instance->getItem();
	Tile* pTile = instance->getTile();

	// @BUG: This is one of the reasons you can't actually hold items in early Minecraft.
	// There's an attempt to index `Tile::tiles` out of bounds, which of course fails, and likely crashes the game. :(
	// If only they'd placed the g_ItemFrames[itemID] check before the TileRenderer::canRender check...
#ifdef ORIGINAL_CODE
#define COND_PRE
#else
#define COND_PRE pTile && 
#endif

	bool bCanRenderAsIs = false;

#ifdef ENH_3D_INVENTORY_TILES
	// We don't need to care about g_ItemFrames at all since blocks will get 3D rendered and 2D props will use the terrain.png as the texture.
	if (COND_PRE(TileRenderer::canRender(pTile->getRenderShape())))
	{
		bCanRenderAsIs = true;
	}
#else
	if (COND_PRE(TileRenderer::canRender(pTile->getRenderShape()) || g_ItemFrames[itemID] != 0))
	{
		bCanRenderAsIs = true;
	}
#endif
	
	if (pTile && bCanRenderAsIs)
	{
#ifndef ENH_3D_INVENTORY_TILES
		textures->loadAndBindTexture(C_BLOCKS_NAME);

		float texU = float(g_ItemFrames[instance->getId()] % 10) * 48.0f;
		float texV = float(g_ItemFrames[instance->getId()] / 10) * 48.0f;

		Tesselator& t = Tesselator::instance;
		// @NOTE: These do nothing, due to a previous t.voidBeginAndEndCalls call.
		t.begin();
		t.vertexUV(float(x +  0), float(y + 16), 0.0f,  texU          / 512.0f, (texV + 48.0f) / 512.0f);
		t.vertexUV(float(x + 16), float(y + 16), 0.0f, (texU + 48.0f) / 512.0f, (texV + 48.0f) / 512.0f);
		t.vertexUV(float(x + 16), float(y +  0), 0.0f, (texU + 48.0f) / 512.0f,  texV          / 512.0f);
		t.vertexUV(float(x +  0), float(y +  0), 0.0f,  texU          / 512.0f,  texV          / 512.0f);
		t.draw();
#else
		textures->loadAndBindTexture(C_TERRAIN_NAME);

		//glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);

		glPushMatrix();

		// scale, rotate, and translate the tile onto the correct screen coordinate
		glTranslatef((GLfloat)x + 8, (GLfloat)y + 8, -8);
		glScalef(10, 10, 10);
		glRotatef(210.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

		// TODO: Why can't we rotate stairs 90deg also? What's rotating them!?
		if (pTile->getRenderShape() != SHAPE_STAIRS)
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		
		tileRenderer->renderTile(pTile, instance->getAuxValue(), 1.0f, true);
		#undef PARM_HACK

		glPopMatrix();

		//glDisable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
#endif
	}
	else if (instance->getIcon() >= 0)
	{
		// @BUG: The last bound texture will be the texture that ALL items will take. This is because begin and end calls
		// have been void'ed by a  t.voidBeginAndEndCalls call in Gui::render.
		if (instance->getTile())
			textures->loadAndBindTexture(C_TERRAIN_NAME);
		else
			textures->loadAndBindTexture(C_ITEMS_NAME);

		blit(x, y, 16 * (instance->getIcon() % 16), 16 * (instance->getIcon() / 16), 16, 16);
	}
}
