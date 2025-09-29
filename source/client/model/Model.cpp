/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Model.hpp"

Model::Model(int width, int height)
{
	field_4 = 0.0f;
	m_bRiding = false;
	m_bIsBaby = true; // @HUH: Why is this true?
	m_textureWidth  = width;
	m_textureHeight = height;
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
