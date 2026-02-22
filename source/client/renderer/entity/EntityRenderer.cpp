/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "EntityRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "client/renderer/Lighting.hpp"
#include "renderer/ShaderConstants.hpp"
#include "EntityRenderDispatcher.hpp"

EntityRenderer::Materials::Materials()
{
	MATERIAL_PTR(switchable, entity_alphatest);
	MATERIAL_PTR(switchable, entity_alphatest_cull);
	MATERIAL_PTR(switchable, entity_alphatest_glint);
	MATERIAL_PTR(common, name_tag);
	MATERIAL_PTR(common, name_tag_depth_tested);
	MATERIAL_PTR(common, name_text_depth_tested);
}

bool EntityRenderer::_areShadowsAvailable = false; // false because PE used a reimplementation with geometry later on, rather than a texture

EntityRenderer::EntityRenderer()
{
	m_shadowRadius = 0.0f;
	m_shadowStrength = 1.0f;
	m_pDispatcher = nullptr;
}

EntityRenderer::~EntityRenderer()
{
}

bool EntityRenderer::bindTexture(const std::string& file, bool isRequired)
{
	if (!m_pDispatcher || !m_pDispatcher->m_pTextures)
		return false;
	return m_pDispatcher->m_pTextures->loadAndBindTexture(file, isRequired) != nullptr;
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

void EntityRenderer::renderFlame(const Entity& entity, const Vec3& pos, float a)
{
	Vec3 ePos(pos);
	ePos.y -= entity.m_heightOffset; // Fixed fire rendering above player's head in third-person

	Lighting::turnOff(false);

	int tex = Tile::fire->getTexture(Facing::NORTH);
	int xt = (tex & 15) << 4;
	int yt = tex & 240;
	float u0 = (float)xt / 256.0f;
	float u1 = ((float)xt + 15.99f) / 256.0f;
	float v0 = (float)yt / 256.0f;
	float v1 = ((float)yt + 15.99f) / 256.0f;

	MatrixStack::Ref matrix = MatrixStack::World.push();

	float s = entity.m_bbWidth * 1.4f; // bbWidth instead of e->m_hitbox.max.x
	float h = entity.m_bbHeight / entity.m_bbWidth;

	matrix->translate(ePos);
	matrix->scale(s);
	matrix->rotate(-m_pDispatcher->m_rot.x, Vec3::UNIT_Y);
	matrix->translate(Vec3(0.0f, 0.0f, -0.4f + (float)((int)h) * 0.02f));
	
	bindTexture(C_TERRAIN_NAME);
	Tesselator& t = Tesselator::instance;
	float r = 1.0f;
	float xo = 0.5f;
	float yo = 0.0f;
	currentShaderColor = Color::WHITE;
	t.begin(12);
	t.normal(Vec3::ZERO); // this is required for HLSL shaders since we're using the entity shader

	while (h > 0.0f)
	{
		t.vertexUV(r - xo, 0.0f - yo, 0.0f, u1, v1);
		t.vertexUV(0.0f - xo, 0.0f - yo, 0.0f, u0, v1);
		t.vertexUV(0.0f - xo, 1.4f - yo, 0.0f, u0, v0);
		t.vertexUV(r - xo, 1.4f - yo, 0.0f, u1, v0);
		--h;
		--yo;
		r *= 0.9f;
		matrix->translate(Vec3(0.0f, 0.0f, -0.04f));
	}

	t.draw(m_materials.entity_alphatest_cull);

	Lighting::turnOn(false);
}

Level* EntityRenderer::getLevel() const
{
	return m_pDispatcher->m_pLevel;
}

void EntityRenderer::render(const AABB& aabb, const Vec3& pos)
{
	Tesselator& t = Tesselator::instance;
	currentShaderColor = Color::WHITE;
	t.begin(24);
	//t.vertex(pos); // Why were we doing this?
	t.setOffset(pos);
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
	
	t.setOffset(Vec3::ZERO);
	t.draw(m_shaderMaterials.entity); // t.end() on Java
}

void EntityRenderer::renderFlat(const AABB& aabb)
{
	Tesselator& t = Tesselator::instance;

	t.begin(24);
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
	t.draw(m_shaderMaterials.entity); // t.end() on Java
}

void EntityRenderer::postRender(const Entity& entity, const Vec3& pos, float rot, float a)
{
	if (m_pDispatcher->m_pOptions && m_pDispatcher->m_pOptions->m_fancyGraphics.get() && areShadowsAvailable() && m_shadowRadius > 0.0f)
	{
		float dist = m_pDispatcher->distanceToSqr(entity.m_pos);
		float pow = (1.0f - dist / 256.0f) * m_shadowStrength;
		if (pow > 0.0f)
			m_pDispatcher->m_pMinecraft->m_pLevelRenderer->renderShadow(entity, pos, m_shadowRadius, pow, a);
	}

	if (entity.isOnFire())
		renderFlame(entity, pos, a);
}
