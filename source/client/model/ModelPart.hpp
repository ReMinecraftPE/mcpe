/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include <vector>
#include "Cube.hpp"
#include "Model.hpp"

class Cube;
class Model;

class ModelPart
{
public:
	ModelPart(int, int);
	ModelPart(const std::string&);
	~ModelPart();

	void addChild(ModelPart*);
	void addBox(float a, float b, float c, int d, int e, int f, float g = 0);
	void addBox(const std::string& id, float a, float b, float c, int d, int e, int f, float g = 0);
	void clear();
	void compile(float scale);
	void draw();
	void drawSlow(float scale);
	void mimic(ModelPart* pPart);
	void render(float scale);
	void renderHorrible(float scale);
	void renderRollable(float scale);
	void setModel(Model* pModel);
	void setPos(float x, float y, float z);
	void setTexSize(int, int);
	void texOffs(int a, int b);
	void translateTo(float scale);
	void setBrightness(float brightness);

private:
	void _init();
	bool hasDefaultPos() { return m_posX == 0 && m_posY == 0 && m_posZ == 0; }
	bool hasDefaultRot() { return m_rotX == 0 && m_rotY == 0 && m_rotZ == 0; }
	void translatePosTo(float scale);
	void translateRotTo(float scale);

public:
	float m_posX;
	float m_posY;
	float m_posZ;
	float m_rotX;
	float m_rotY;
	float m_rotZ;
	bool field_18;
	std::vector<Cube*> m_pCubes;
	std::vector<ModelPart*> m_pChildren;
	std::string field_34;
	float m_textureWidth;
	float m_textureHeight;
	int field_40;
	int field_44;
	bool field_48;
	bool field_49;
	bool m_bCompiled;
	int field_4C;
	GLuint m_buffer;
	Model* m_pModel;
};

