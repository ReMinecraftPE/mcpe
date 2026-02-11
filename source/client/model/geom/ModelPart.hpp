/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include <vector>
#include "renderer/MatrixStack.hpp"
#include "renderer/Mesh.hpp"
#include "world/phys/Vec3.hpp"
#include "TextureOffset.hpp"

class Cube;
class Model;

class ModelPart
{
private:
	void _init();
	void _init(int, int);

public:
	ModelPart(int, int);
	ModelPart(Model*, int, int);
	ModelPart(const std::string&);
	~ModelPart();

	void addChild(ModelPart*);
	void addBox(float a, float b, float c, int d, int e, int f, float g = 0);
	void addBox(const std::string& id, float a, float b, float c, int d, int e, int f, float g = 0);
	void clear();
	void compile(float scale);
	void draw(float scale, const mce::MaterialPtr* materialOverride = nullptr);
	void mimic(const ModelPart& other);
	void render(float scale, const mce::MaterialPtr* materialOverride = nullptr);
	void setModel(Model* pModel);
	void setPos(const Vec3& pos);
	void setPos(float x, float y, float z);
	void setTexSize(int, int);
	void texOffs(int a, int b);
	void translateTo(Matrix& matrix, float scale);
	void setBrightness(float brightness);

private:
	bool hasDefaultPos() { return m_pos == Vec3::ZERO; }
	bool hasDefaultRot() { return m_rot == Vec3::ZERO; }
	void translatePosTo(Matrix& matrix, float scale);
	void translateRotTo(Matrix& matrix, float scale);

public:
	Vec3 m_pos;
	Vec3 m_rot;
	bool m_bMirror;
	std::vector<Cube*> m_pCubes;
	std::vector<ModelPart*> m_pChildren;
	std::string field_34;
	float m_textureWidth;
	float m_textureHeight;
	mce::MaterialPtr* m_pMaterial;
	TextureOffset m_texOffs;
	bool m_bVisible;
	bool m_bNeverRender;
	bool m_bCompiled;
	mce::Mesh m_mesh;
	Model* m_pModel;
};

