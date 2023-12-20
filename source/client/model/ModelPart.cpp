/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "ModelPart.hpp"
#include "renderer/GL/GL.hpp"

#define MUL_DEG_TO_RAD (180.0f / float(M_PI))  // formerly known as Cube::c

ModelPart::ModelPart(int a, int b)
{
	m_pModel = nullptr;
	field_40 = a;
	field_44 = b;
	_init();
}

ModelPart::ModelPart(const std::string& baseId)
{
	m_pModel = nullptr;
	field_34 = baseId;
	field_40 = 0;
	field_44 = 0;
	_init();
}

ModelPart::~ModelPart()
{
	clear();
}

void ModelPart::_init()
{
	m_posX = m_posY = m_posZ = 0;
	m_rotX = m_rotY = m_rotZ = 0;
	m_buffer = 0;
	m_textureWidth = 64.0f;
	m_textureHeight = 32.0f;
	field_4C = 0;
	field_18 = false;
	field_48 = true;
	field_49 = false;
	m_bCompiled = false;
}

void ModelPart::addChild(ModelPart* pPart)
{
	m_pChildren.push_back(pPart);
}

void ModelPart::addBox(float a, float b, float c, int d, int e, int f, float g)
{
	Cube* pCube = new Cube(this, field_40, field_44, a, b, c, d, e, f, g);
	m_pCubes.push_back(pCube);
}

void ModelPart::addBox(const std::string& id, float a, float b, float c, int d, int e, int f, float g)
{
	Cube* pCube = new Cube(this, field_40, field_44, a, b, c, d, e, f, g);
	pCube->setId(field_34 + "." + id);
	m_pCubes.push_back(pCube);
}

void ModelPart::clear()
{
	for (size_t i = 0; i < m_pCubes.size(); i++)
		delete m_pCubes[i];

	// N.B. does not clear children
	m_pCubes.clear();
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

void ModelPart::draw()
{
	drawArrayVT(this->m_buffer, 36 * m_pCubes.size(), sizeof(Tesselator::Vertex));
}

void ModelPart::drawSlow(float scale)
{
	Tesselator& t = Tesselator::instance;
	t.begin();

	for (size_t i = 0; i < m_pCubes.size(); i++)
	{
		for (int f = 0; f < 6; f++)
			m_pCubes[i]->m_faces[f].render(t, scale);
	}
	
	t.draw();
}

void ModelPart::mimic(ModelPart* pPart)
{
	m_posX = pPart->m_posX;
	m_posY = pPart->m_posY;
	m_posZ = pPart->m_posZ;
	m_rotX = pPart->m_rotX;
	m_rotY = pPart->m_rotY;
	m_rotZ = pPart->m_rotZ;
}

void ModelPart::translatePosTo(float scale)
{
	glTranslatef(m_posX * scale, m_posY * scale, m_posZ * scale);
}

void ModelPart::translateRotTo(float scale)
{
	glTranslatef(m_posX * scale, m_posY * scale, m_posZ * scale);
	if (m_rotZ != 0) glRotatef(m_rotZ * MUL_DEG_TO_RAD, 0, 0, 1);
	if (m_rotY != 0) glRotatef(m_rotY * MUL_DEG_TO_RAD, 0, 1, 0);
	if (m_rotX != 0) glRotatef(m_rotX * MUL_DEG_TO_RAD, 1, 0, 0);
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
		draw();

		glPopMatrix();
	}
	else if (!hasDefaultPos())
	{
		translatePosTo(scale);
		draw();
		translatePosTo(-scale);
	}
	else
	{
		draw();
	}
}

void ModelPart::renderHorrible(float scale)
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
		drawSlow(scale);

		glPopMatrix();
	}
	else if (!hasDefaultPos())
	{
		translatePosTo(scale);
		drawSlow(scale);
		translatePosTo(-scale);
	}
	else
	{
		drawSlow(scale);
	}
}

void ModelPart::renderRollable(float scale)
{
	if (field_49)
		return;

	if (!field_48)
		return;

	if (!m_bCompiled)
		compile(scale);

	glPushMatrix();
	translatePosTo(scale);

	translateRotTo(scale);
	draw();

	glPopMatrix();
}

void ModelPart::setModel(Model* pModel)
{
	m_pModel = pModel;
	pModel->m_parts.push_back(this);
	setTexSize(pModel->m_textureWidth, pModel->m_textureHeight);
}

void ModelPart::setPos(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
}

void ModelPart::setTexSize(int width, int height)
{
	m_textureWidth = float(width);
	m_textureHeight = float(height);
}

void ModelPart::texOffs(int a, int b)
{
	field_40 = a;
	field_44 = b;
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

void ModelPart::setBrightness(float brightness)
{
	//no op
}
