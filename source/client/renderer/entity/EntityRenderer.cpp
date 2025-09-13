/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "EntityRenderer.hpp"
#include "EntityRenderDispatcher.hpp"

bool EntityRenderer::_areShadowsAvailable = false; // false because PE used a reimplementation with geometry later on, rather than a texture

EntityRenderer::EntityRenderer() : m_model(0.0f, 0.0f)
{
	m_shadowRadius = 0.0f;
	m_shadowStrength = 1.0f;
	m_pDispatcher = nullptr;
}

bool EntityRenderer::bindTexture(const std::string& file, bool isRequired)
{
	return m_pDispatcher->m_pTextures->loadAndBindTexture(file, isRequired) != -1;
}

Font* EntityRenderer::getFont()
{
	return m_pDispatcher->m_pFont;
}

void EntityRenderer::init(EntityRenderDispatcher* d)
{
	m_pDispatcher = d;
}

void EntityRenderer::onGraphicsReset()
{

}

void EntityRenderer::renderFlame(Entity* e, const Vec3& pos, float a)
{
	Vec3 ePos(pos);
	ePos.y -= e->m_heightOffset; // Fixed fire rendering above player's head in third-person

	glDisable(GL_LIGHTING);
	int tex = Tile::fire->getTexture(Facing::NORTH);
	int xt = (tex & 15) << 4;
	int yt = tex & 240;
	float u0 = (float)xt / 256.0f;
	float u1 = ((float)xt + 15.99f) / 256.0f;
	float v0 = (float)yt / 256.0f;
	float v1 = ((float)yt + 15.99f) / 256.0f;
	glPushMatrix();
	glTranslatef(ePos.x, ePos.y, ePos.z);
	float s = e->m_bbWidth * 1.4f; // bbWidth instead of e->m_hitbox.max.x
	glScalef(s, s, s);
	bindTexture(C_TERRAIN_NAME);
	Tesselator& t = Tesselator::instance;
	float r = 1.0f;
	float xo = 0.5f;
	float yo = 0.0f;
	float h = e->m_bbHeight / e->m_bbWidth;
	glRotatef(-m_pDispatcher->m_rot.x, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -0.4f + (float)((int)h) * 0.02f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	t.begin();

	while (h > 0.0f)
	{
		t.vertexUV(r - xo, 0.0f - yo, 0.0f, u1, v1);
		t.vertexUV(0.0f - xo, 0.0f - yo, 0.0f, u0, v1);
		t.vertexUV(0.0f - xo, 1.4f - yo, 0.0f, u0, v0);
		t.vertexUV(r - xo, 1.4f - yo, 0.0f, u1, v0);
		--h;
		--yo;
		r *= 0.9f;
		glTranslatef(0.0f, 0.0f, -0.04f);
	}

	t.draw();
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void EntityRenderer::renderShadow(Entity* e, const Vec3& pos, float pow, float a)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pDispatcher->m_pTextures->setClampToEdge(true);
	bindTexture("misc/shadow.png");
	m_pDispatcher->m_pTextures->setClampToEdge(false);

	Level* level = getLevel();

	glDepthMask(false);
	float r = m_shadowRadius;

	Vec3 ePos(e->m_posPrev + (e->m_pos - e->m_posPrev) * a);
	ePos.y -= e->m_heightOffset; // We gotta do this so the renderer can correctly determine if there's a tile below the entity
	ePos.y += e->getShadowHeightOffs();

	TilePos tpMin(ePos - r);
	TilePos tpMax(ePos.x + r, ePos.y, ePos.z + r);
	Vec3 ePosO(pos - ePos);

	Tesselator& tt = Tesselator::instance;
	tt.begin();
	TilePos tp(tpMin);
	for (tp.x = tpMin.x; tp.x <= tpMax.x; tp.x++)
	{
		for (tp.y = tpMin.y; tp.y <= tpMax.y; tp.y++)
		{
			for (tp.z = tpMin.z; tp.z <= tpMax.z; tp.z++)
			{
				TileID t = level->getTile(tp.below());
				if (t > 0 && level->getRawBrightness(tp) > 3)
				{
					renderTileShadow(Tile::tiles[t],
						Vec3(pos.x, pos.y - e->m_heightOffset + e->getShadowHeightOffs(), pos.z), tp,
						pow, r,
						Vec3(ePosO.x, ePosO.y - e->m_heightOffset + e->getShadowHeightOffs(), ePosO.z)
					);
				}
			}
		}
	}
	tt.draw();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glDepthMask(true);
}

Level* EntityRenderer::getLevel() const
{
	return m_pDispatcher->m_pLevel;
}

void EntityRenderer::renderTileShadow(Tile* tt, const Vec3& pos, TilePos& tilePos, float pow, float r, const Vec3& oPos)
{
	Tesselator& t = Tesselator::instance;
	if (!tt->isCubeShaped()) return;

	float a = (pow - (pos.y - ((float)tilePos.y + oPos.y)) / 2.0f) * 0.5f * getLevel()->getBrightness(tilePos);
	if (a < 0.0f)
		return;
	if (a > 1.0f) a = 1.0f;

	t.color(1.0f, 1.0f, 1.0f, a);
	float x0 = (float)tilePos.x + tt->m_aabb.min.x + oPos.x;
	float x1 = (float)tilePos.x + tt->m_aabb.max.x + oPos.x;
	float y0 = (float)tilePos.y + tt->m_aabb.min.y + oPos.y + (1.0f / 64.0f);
	float z0 = (float)tilePos.z + tt->m_aabb.min.z + oPos.z;
	float z1 = (float)tilePos.z + tt->m_aabb.max.z + oPos.z;
	float u0 = ((pos.x - x0) / 2.0f / r + 0.5f);
	float u1 = ((pos.x - x1) / 2.0f / r + 0.5f);
	float v0 = ((pos.z - z0) / 2.0f / r + 0.5f);
	float v1 = ((pos.z - z1) / 2.0f / r + 0.5f);
	t.vertexUV(x0, y0, z0, u0, v0);
	t.vertexUV(x0, y0, z1, u0, v1);
	t.vertexUV(x1, y0, z1, u1, v1);
	t.vertexUV(x1, y0, z0, u1, v0);
}


void EntityRenderer::render(const AABB& aabb, const Vec3& pos)
{
	glDisable(GL_TEXTURE_2D);
	Tesselator& t = Tesselator::instance;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	t.begin();
	//t.vertex(pos); // Why were we doing this?
	t.offset(pos);
	t.normal(0.0f, 0.0f, -1.0f);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);

	t.normal(0.0f, 0.0f, 1.0f);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);

	t.normal(0.0f, -1.0f, 0.0f);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);

	t.normal(0.0f, 1.0f, 0.0f);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	
	t.normal(-1.0f, 0.0f, 0.0f);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	
	t.normal(1.0f, 0.0f, 0.0f);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	
	t.offset(Vec3::ZERO);
	t.draw(); // t.end() on Java
	glEnable(GL_TEXTURE_2D);
}

void EntityRenderer::renderFlat(const AABB& aabb)
{
	Tesselator& t = Tesselator::instance;

	t.begin();
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.draw(); // t.end() on Java
}

void EntityRenderer::postRender(Entity* entity, const Vec3& pos, float rot, float a)
{
	if (m_pDispatcher->m_pOptions->m_bFancyGraphics && areShadowsAvailable() && m_shadowRadius > 0.0f)
	{
		float dist = m_pDispatcher->distanceToSqr(entity->m_pos);
		float pow = (1.0f - dist / 256.0f) * m_shadowStrength;
		if (pow > 0.0f)
			renderShadow(entity, pos, pow, a);
	}

	if (entity->isOnFire())
		renderFlame(entity, pos, a);
}
