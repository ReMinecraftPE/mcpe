/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "EntityRenderer.hpp"
#include "EntityRenderDispatcher.hpp"

EntityRenderer::EntityRenderer() : m_model(0.0f, 0.0f)
{
}

void EntityRenderer::bindTexture(const std::string& file)
{
	m_pDispatcher->m_pTextures->loadAndBindTexture(file);
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

void EntityRenderer::render(const AABB& aabb, float offX, float offY, float offZ)
{
	Tesselator& t = Tesselator::instance;

	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	t.begin();
	t.vertex(offX, offY, offZ);
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
	t.offset(0.0f, 0.0f, 0.0f);
	t.draw();
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
	t.draw();
}

