/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Model.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"

Model::Materials::Materials()
{
	MATERIAL_PTR(switchable, entity);
	MATERIAL_PTR(switchable, entity_alphatest);
	MATERIAL_PTR(switchable, entity_alphablend);
	MATERIAL_PTR(switchable, entity_static);
	MATERIAL_PTR(switchable, entity_emissive);
	MATERIAL_PTR(switchable, entity_emissive_alpha);
	MATERIAL_PTR(switchable, entity_change_color);
	MATERIAL_PTR(switchable, entity_glint);
	MATERIAL_PTR(switchable, entity_alphatest_glint);
}

Model::Model(int width, int height)
{
	field_4 = 0.0f;
	m_bRiding = false;
	m_bIsBaby = true; // @HUH: Why is this true?
	m_textureWidth  = width;
	m_textureHeight = height;
	m_pMaterial = &m_materials.entity;
}

void Model::clear()
{
	for (size_t i = 0; i < m_parts.size(); i++)
	{
		m_parts[i]->m_mesh.reset();
	}
}

void Model::onGraphicsReset()
{
	for (size_t i = 0; i < m_parts.size(); i++)
	{
		m_parts[i]->m_bCompiled = false;
	}
}

void Model::prepareMobModel(Mob*, float, float, float)
{
	
}

void Model::render(float, float, float, float, float, float)
{
	
}

void Model::setupAnim(float, float, float, float, float, float)
{
	
}

void Model::setBrightness(float)
{
}

void Model::copyModelPart(const ModelPart& a, ModelPart& b)
{
	b.m_rot = a.m_rot;
	b.m_pos = a.m_pos;
}
