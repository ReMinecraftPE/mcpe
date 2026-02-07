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
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "world/entity/ItemEntity.hpp"

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

ItemRenderer::Materials::Materials()
{
	MATERIAL_PTR(switchable, item_entity);
	MATERIAL_PTR(common, ui_fill_color);
	MATERIAL_PTR(common, ui_fill_gradient);
	MATERIAL_PTR(common, ui_textured);
	MATERIAL_PTR(common, ui_texture_and_color);
	MATERIAL_PTR(common, ui_item);
	MATERIAL_PTR(common, ui_item_glint);
}

ItemRenderer* ItemRenderer::singletonPtr = nullptr;

ItemRenderer& ItemRenderer::singleton()
{
	if (!ItemRenderer::singletonPtr)
	{
		ItemRenderer::singletonPtr = new ItemRenderer();
	}

	return *singletonPtr;
}

ItemRenderer::ItemRenderer()
{
	static TileRenderer* tileRenderer = new TileRenderer();
	m_pTileRenderer = tileRenderer;

	m_shadowRadius = 0.15f;
	m_shadowStrength = 0.75f;
}

void ItemRenderer::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
	m_random.init_genrand(187);
	const ItemEntity& itemEntity = (const ItemEntity&)entity;

	MatrixStack::Ref matrix = MatrixStack::World.push();

	float yOffset = Mth::sin((float(itemEntity.m_age) + a) / 10.0f + itemEntity.m_bobOffs);
	const ItemStack& itemStack = itemEntity.m_itemStack;
	if (itemStack.isEmpty())
		return;

	int itemsToRender = 1;
	if (itemStack.m_count > 1)
		itemsToRender = 2;
	if (itemStack.m_count > 5)
		itemsToRender = 3;
	if (itemStack.m_count > 20)
		itemsToRender = 4;

	matrix->translate(Vec3(pos.x, pos.y + 0.1f + yOffset * 0.1f, pos.z));

#if MCE_GFX_API_OGL && !defined(FEATURE_GFX_SHADERS)
	glEnable(GL_RESCALE_NORMAL);
#endif

	_setupShaderParameters(entity, Color::NIL, a);

	Tile* pTile = itemStack.getTile();
	if (pTile && TileRenderer::canRender(pTile->getRenderShape()))
	{
		matrix->rotate(((float(itemEntity.m_age) + a) / 20.0f + itemEntity.m_bobOffs) * 57.296f, Vec3::UNIT_Y);

		bindTexture(C_TERRAIN_NAME);

		float scale = 0.5f;

		// @BUG: If cacti existed and were able to be dropped, they would be 2x the size of a regular tile.
		// This bug has been in the main game until Java Edition Beta 1.8.
		if (pTile->isCubeShaped() || pTile == Tile::stoneSlabHalf)
			scale = 0.25f;

		matrix->scale(scale);

		for (int i = 0; i < itemsToRender; i++)
		{
			MatrixStack::Ref matrix = MatrixStack::World.push();
			if (i != 0)
			{
				matrix->translate(Vec3(
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) / scale));
			}

			m_pTileRenderer->renderTile(FullTile(pTile, itemStack.getAuxValue()), m_itemMaterials.item_entity, itemEntity.getBrightness(1.0f));
		}
	}
	else
	{
		matrix->scale(0.5f);
		int icon = itemStack.getIcon();

		bindTexture(itemStack.getTile() ? C_TERRAIN_NAME : C_ITEMS_NAME);

		for (int i = 0; i < itemsToRender; i++)
		{
			MatrixStack::Ref matrix = MatrixStack::World.push();
			if (i != 0)
			{
				matrix->translate(Vec3(
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f,
					0.2f * (m_random.nextFloat() * 2.0f - 1.0f) * 0.3f));
			}

			matrix->rotate(180.0f - m_pDispatcher->m_rot.x, Vec3::UNIT_Y);

			Tesselator& t = Tesselator::instance;
			t.begin(4);

#ifdef ENH_SHADE_HELD_TILES
			float bright = itemEntity.getBrightness(1.0f);
			t.color(bright, bright, bright);
#endif
			t.normal(Vec3::UNIT_Y);
			t.vertexUV(-0.5f, -0.25f, 0.0f, float(16 * (icon % 16))     / 256.0f, float(16 * (icon / 16 + 1)) / 256.0f);
			t.vertexUV(+0.5f, -0.25f, 0.0f, float(16 * (icon % 16 + 1)) / 256.0f, float(16 * (icon / 16 + 1)) / 256.0f);
			t.vertexUV(+0.5f, +0.75f, 0.0f, float(16 * (icon % 16 + 1)) / 256.0f, float(16 * (icon / 16))     / 256.0f);
			t.vertexUV(-0.5f, +0.75f, 0.0f, float(16 * (icon % 16))     / 256.0f, float(16 * (icon / 16))     / 256.0f);
            
			t.draw(m_itemMaterials.item_entity);
		}
	}

#if MCE_GFX_API_OGL && !defined(FEATURE_GFX_SHADERS)
	glDisable(GL_RESCALE_NORMAL);
#endif
}

void ItemRenderer::blitRect(Tesselator& t, int x, int y, int w, int h, int color)
{
	t.begin(4);
	t.color(color);
	t.vertex(float(x),     float(y),     0.0f);
	t.vertex(float(x),     float(y + h), 0.0f);
	t.vertex(float(x + w), float(y + h), 0.0f);
	t.vertex(float(x + w), float(y),     0.0f);
	t.draw(m_itemMaterials.ui_fill_gradient);
}

void ItemRenderer::blit(int dx, int dy, int sx, int sy, int tw, int th)
{
	Tesselator& t = Tesselator::instance;

	float ex = float(dx), ey = float(dy);
	float uw = float(tw), uh = float(th);
	float vx = float(sx), vy = float(sy);

	t.begin(4);
	t.vertexUV(ex,      ey + uh, 0.0f, float(vx)      / 256.0f, float(vy + uh) / 256.0f);
	t.vertexUV(ex + uw, ey + uh, 0.0f, float(vx + uw) / 256.0f, float(vy + uh) / 256.0f);
	t.vertexUV(ex + uw, ey,      0.0f, float(vx + uw) / 256.0f, float(vy)      / 256.0f);
	t.vertexUV(ex,      ey,      0.0f, float(vx)      / 256.0f, float(vy)      / 256.0f);
	t.draw(m_itemMaterials.ui_textured);
}

void ItemRenderer::renderGuiItemOverlay(Font* font, Textures* textures, ItemStack& item, int x, int y)
{
	if (item.isEmpty())
		return;

	// Draw damage amount
	if (item.isDamaged()) {
		int duraWidth = ceilf(13.0f - static_cast<float>(item.getDamageValue()) * 13.0f / static_cast<float>(item.getMaxDamage()));
		int duraPercent = ceilf(255.0f - static_cast<float>(item.getDamageValue()) * 255.0f / static_cast<float>(item.getMaxDamage()));


		int duraBgColor = (((255 - duraPercent) / 4) << 16) | 0x3F00;
		int duraColor = ((255 - duraPercent) << 16) | (duraPercent << 8);

		Tesselator& t = Tesselator::instance;
		
		blitRect(t, x + 2, y + 13, 13, 2, 0);
		blitRect(t, x + 2, y + 13, 12, 1, duraBgColor);
		blitRect(t, x + 2, y + 13, duraWidth, 1, duraColor);
	}

	if (item.m_count <= 1) {
		return;
	}

	// Draw num items
	std::stringstream ss;
	ss << item.m_count;
	std::string amtstr = ss.str();

	int width = font->width(amtstr), height = font->height(amtstr) + 8;

	font->drawShadow(amtstr, x + 17 - width, y + 17 - height, 0xFFFFFF);
}

void ItemRenderer::renderGuiItem(Font* font, Textures* textures, ItemStack& item, int x, int y, bool b)
{
	// @NOTE: Font unused but would presumably be used to draw the item amount.
	// As if that actually works due to us blocking t.begin() and t.draw() calls...
	if (item.isEmpty() || !item.isValid())
		return;

	if (!b)
		return;

	//Item* pItem = item->getItem();
	Tile* pTile = item.getTile();

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

		float texU = float(g_ItemFrames[item->getId()] % 10) * 48.0f;
		float texV = float(g_ItemFrames[item->getId()] / 10) * 48.0f;

		Tesselator& t = Tesselator::item;
		// @NOTE: These do nothing, due to a previous t.voidBeginAndEndCalls call.
		t.begin();
		t.vertexUV(float(x +  0), float(y + 16), 0.0f,  texU          / 512.0f, (texV + 48.0f) / 512.0f);
		t.vertexUV(float(x + 16), float(y + 16), 0.0f, (texU + 48.0f) / 512.0f, (texV + 48.0f) / 512.0f);
		t.vertexUV(float(x + 16), float(y +  0), 0.0f, (texU + 48.0f) / 512.0f,  texV          / 512.0f);
		t.vertexUV(float(x +  0), float(y +  0), 0.0f,  texU          / 512.0f,  texV          / 512.0f);
		t.draw();
#else
		textures->loadAndBindTexture(C_TERRAIN_NAME);

		MatrixStack::Ref matrix = MatrixStack::World.push();

		// scale, rotate, and translate the tile onto the correct screen coordinate
		matrix->translate(Vec3(x + 8, y + 8, -8));
		matrix->scale(10);
		matrix->rotate(210.0f, Vec3::UNIT_X);
		matrix->rotate(45.0f, Vec3::UNIT_Y);

		// TODO: Why can't we rotate stairs 90deg also? What's rotating them!?
		if (pTile->getRenderShape() != SHAPE_STAIRS)
		{
			matrix->rotate(-90.0f, Vec3::UNIT_Y);
		}
		
		m_pTileRenderer->renderTile(FullTile(pTile, item.getAuxValue()), m_itemMaterials.ui_item, 1.0f, true);
		#undef PARM_HACK
#endif
	}
	else if (item.getIcon() >= 0)
	{
		// @BUG: The last bound texture will be the texture that ALL items will take. This is because begin and end calls
		// have been void'ed by a  t.voidBeginAndEndCalls call in Gui::render.
		if (item.getTile())
			textures->loadAndBindTexture(C_TERRAIN_NAME);
		else
			textures->loadAndBindTexture(C_ITEMS_NAME);

		blit(x, y, 16 * (item.getIcon() % 16), 16 * (item.getIcon() / 16), 16, 16);
	}
}
