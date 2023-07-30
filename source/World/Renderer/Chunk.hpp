#pragma once

#include "Utils.hpp"
#include "FrustumCuller.hpp"
#include "RenderList.hpp"
#include "Tesselator.hpp"

class Level;
class Entity;

class Chunk
{
public:
	Chunk(Level*, int, int, int, int, int, GLuint*);

	float distanceToSqr(const Entity*) const;
	float squishedDistanceToSqr(const Entity*) const;
	void reset();
	int getList(int idx);
	RenderChunk* getRenderChunk(int idx);
	int getAllLists(int* arr, int arr_idx, int idx);
	void cull(Culler* pCuller);
	void renderBB();
	bool isEmpty();
	void setDirty();
	void setPos(int x, int y, int z);
	void setClean();
	bool isDirty();
	void rebuild();
	void translateToPos();

public:
	static int updates;

public:
	Level* m_pLevel;
	Pos m_pos;
	int field_10;
	int field_14;
	int field_18;
	bool field_1C[2];
	Pos m_pos2;
	float field_2C;
	AABB m_aabb;
	int field_48;
	bool m_bVisible;
	bool field_4D = true;
	bool field_4E = false;
	int field_50;
	bool field_54;
	RenderChunk m_renderChunks[2];
	Tesselator* m_pTesselator;
	int field_8C;
	GLuint* field_90;
	bool field_94 = false;
	bool m_bDirty = false;
};

