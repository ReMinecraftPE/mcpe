/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "ModelPart.hpp"
#include "renderer/GL/GL.hpp"
#include "../models/Model.hpp"

#define MUL_DEG_TO_RAD (180.0f / float(M_PI))  // formerly known as Cube::c

void ModelPart::_init()
{
	m_pos = Vec3::ZERO;
	m_rot = Vec3::ZERO;
	m_buffer = 0;
	m_textureWidth = 64.0f;
	m_textureHeight = 32.0f;
	m_pMaterial = nullptr;
	field_4C = 0;
	m_bMirror = false;
	field_48 = true;
	field_49 = false;
	m_bCompiled = false;
	m_pModel = nullptr;
}

void ModelPart::_init(int xTexOffs, int yTexOffs)
{
	_init();
	texOffs(xTexOffs, yTexOffs);
}

ModelPart::ModelPart(int xTexOffs, int yTexOffs)
{
	_init(xTexOffs, yTexOffs);
}

ModelPart::ModelPart(Model* model, int xTexOffs, int yTexOffs)
{
	_init(xTexOffs, yTexOffs);
	setModel(model);
}

ModelPart::ModelPart(const std::string& baseId)
{
	_init();
	field_34 = baseId;
}

ModelPart::~ModelPart()
{
	clear();
}

void ModelPart::addChild(ModelPart* pPart)
{
	m_pChildren.push_back(pPart);
}

void ModelPart::addBox(float a, float b, float c, int d, int e, int f, float g)
{
	Cube* pCube = new Cube(this, m_texOffs, a, b, c, d, e, f, g);
	m_pCubes.push_back(pCube);
}

void ModelPart::addBox(const std::string& id, float a, float b, float c, int d, int e, int f, float g)
{
	Cube* pCube = new Cube(this, m_texOffs, a, b, c, d, e, f, g);
	pCube->setId(field_34 + "." + id);
	m_pCubes.push_back(pCube);
}

void ModelPart::clear()
{
	for (size_t i = 0; i < m_pCubes.size(); i++)
		delete m_pCubes[i];
	m_pCubes.clear();
    
	for (size_t i = 0; i < m_pChildren.size(); i++)
		delete m_pChildren[i];
    m_pChildren.clear();
}

void ModelPart::compile(float scale)
{
	Tesselator& t = Tesselator::instance;
	xglGenBuffers(1, &m_buffer);
	t.begin();
	t.color(255, 255, 255, 255);

	// @HUH: Recompiling every cube six times??
#ifdef ORIGINAL_CODE
	for (int i = 0; i < 6; i++)
	{
#endif

	for (size_t i = 0; i < m_pCubes.size(); i++)
	{
		m_pCubes[i]->compile(t, scale);
	}

#ifdef ORIGINAL_CODE
	}
#endif

	t.end(m_buffer);
	m_bCompiled = true;
}

void ModelPart::draw(float scale)
{
	// We are not using drawArrayVTC here since that would use the color that's compiled initially into the ModelPart
	// and would therefore not allow for on-the-fly coloring.
	drawArrayVTN(this->m_buffer, 36 * (int)m_pCubes.size());
}

void ModelPart::mimic(const ModelPart& other)
{
	m_pos = other.m_pos;
	m_rot = other.m_rot;
}

void ModelPart::translatePosTo(float scale)
{
	glTranslatef(m_pos.x * scale, m_pos.y * scale, m_pos.z * scale);
}

void ModelPart::translateRotTo(float scale)
{
	translatePosTo(scale);
	if (m_rot.z != 0) glRotatef(m_rot.z * MUL_DEG_TO_RAD, 0, 0, 1);
	if (m_rot.y != 0) glRotatef(m_rot.y * MUL_DEG_TO_RAD, 0, 1, 0);
	if (m_rot.x != 0) glRotatef(m_rot.x * MUL_DEG_TO_RAD, 1, 0, 0);
}

void ModelPart::translatePosTo(Matrix& matrix, float scale)
{
	matrix.translate(m_pos * scale);
}

void ModelPart::translateRotTo(Matrix& matrix, float scale)
{
	translatePosTo(matrix, scale);
	if (m_rot.z != 0.0f) matrix.rotate(m_rot.z * MUL_DEG_TO_RAD, Vec3::UNIT_Z);
	if (m_rot.y != 0.0f) matrix.rotate(m_rot.y * MUL_DEG_TO_RAD, Vec3::UNIT_Y);
	if (m_rot.x != 0.0f) matrix.rotate(m_rot.x * MUL_DEG_TO_RAD, Vec3::UNIT_X);
}

void ModelPart::render(float scale)
{
	if (field_49)
		return;

	if (!field_48)
		return;

	if (!m_bCompiled)
		compile(scale);

	if (!hasDefaultRot())
	{
		glPushMatrix();

		translateRotTo(scale);
		draw(scale);

		glPopMatrix();
	}
	else if (!hasDefaultPos())
	{
		translatePosTo(scale);
		draw(scale);
		translatePosTo(-scale);
	}
	else
	{
		draw(scale);
	}
}

void ModelPart::setModel(Model* pModel)
{
	m_pModel = pModel;
	pModel->m_parts.push_back(this);
	setTexSize(pModel->m_textureWidth, pModel->m_textureHeight);
}

void ModelPart::setPos(const Vec3& pos)
{
	m_pos = pos;
}

void ModelPart::setPos(float x, float y, float z)
{
	setPos(Vec3(x, y, z));
}

void ModelPart::setTexSize(int width, int height)
{
	m_textureWidth = float(width);
	m_textureHeight = float(height);
}

void ModelPart::texOffs(int xTexOffs, int yTexOffs)
{
	m_texOffs.x = xTexOffs;
	m_texOffs.y = yTexOffs;
}

void ModelPart::translateTo(float scale)
{
	if (field_49)
		return;

	if (!field_48)
		return;

	if (!hasDefaultRot())
		translateRotTo(scale);
	else if (!hasDefaultPos())
		translatePosTo(scale);
}

void ModelPart::translateTo(Matrix& matrix, float scale)
{
	if (!hasDefaultRot())
		translateRotTo(matrix, scale);
	else if (!hasDefaultPos())
		translatePosTo(matrix, scale);
}

void ModelPart::setBrightness(float brightness)
{
	//no op
}
