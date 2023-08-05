/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelRenderer.hpp"
#include "Minecraft.hpp"

LevelRenderer::LevelRenderer(Minecraft* pMC, Textures* pTexs)
{
	m_pMinecraft = pMC;
	m_pTextures = pTexs;

	m_pBuffers = new GLuint[m_nBuffers];
	xglGenBuffers(m_nBuffers, m_pBuffers);

	printf("numBuffers: %d\n", m_nBuffers);
	xglGenBuffers(1, &field_D8);

	generateSky(); // inlined in the 0.1.0 demo
}

void LevelRenderer::generateSky()
{
	Tesselator& t = Tesselator::instance;
	t.begin();
	field_DC = 0;

	float m = 16.0f;
	int n = 4;
	int p = 128;

	for (int i = n * -p; i <= n * p; i += p)
	{
		for (int j = n * -p; j <= n * p; j += p)
		{
			t.vertex(float(i + 0.0f), 16.0f, float(j + 0.0f));
			t.vertex(float(i + p)   , 16.0f, float(j + 0.0f));
			t.vertex(float(i + p)   , 16.0f, float(j + p)   );
			t.vertex(float(i + 0.0f), 16.0f, float(j + p)   );

			field_DC += 4;
		}
	}

	t.end(field_D8);
}

void LevelRenderer::deleteChunks()
{
	for (int i = 0; i < field_AC; i++)
	{
		for (int j = 0; j < field_A8; j++)
		{
			for (int k = 0; k < field_A4; k++)
			{
				int index = k + field_A4 * (j + field_A8 * i);
				delete m_chunks[index];
			}
		}
	}

	if (m_chunks)
		delete[] m_chunks;
	m_chunks = nullptr;

	if (field_98)
		delete[] field_98;
	field_98 = nullptr;
}

void LevelRenderer::cull(Culler* pCuller, float f)
{
	for (int i = 0; i < m_chunksLength; i++)
	{
		Chunk* pChunk = m_chunks[i];
		if (pChunk->isEmpty())
			continue;

		//@TODO: What does the shift do? (x % 4 == 0)?
		if (!pChunk->m_bVisible || !((i + field_30) << 28))
		{
			pChunk->cull(pCuller);
		}
	}

	field_30++;
}

void LevelRenderer::allChanged()
{
	deleteChunks();

	LeafTile* pLeaves = (LeafTile*)Tile::leaves;

	pLeaves->m_bTransparent = m_pMinecraft->m_options.m_bFancyGraphics;
	pLeaves->m_TextureFrame = !pLeaves->m_bTransparent + pLeaves->field_74;

	field_BC = m_pMinecraft->m_options.field_10;

	int x1 = 64 << (3 - field_BC);
	if (x1 >= 400)
		x1 = 400;

	field_A4 = x1 / 16 + 1;
	field_AC = x1 / 16 + 1;
	field_A8 = 8;

	m_chunksLength = field_A8 * field_A4 * field_AC;
	printf("chunksLength: %d\n", m_chunksLength);
	m_chunks = new Chunk* [m_chunksLength];
	field_98 = new Chunk* [m_chunksLength];

	field_6C = 0;
	field_70 = 0;
	field_74 = 0;

	field_88.clear();

	field_78 = field_A4;
	field_80 = field_AC;
	field_7C = field_A8;

	int x2 = 0, x3 = 0;

	for (int i = 0; i < field_A4; i++)
	{
		if (field_A8 <= 0)
			continue;

		for (int j = 0; j < field_A8; j++)
		{
			for (int k = 0; k < field_AC; k++)
			{
				int index = i + field_A4 * (j + field_A8 * k);

				Chunk* pChunk = new Chunk(m_pLevel, 16 * i, 16 * j, 16 * k, 16, x3 + field_B0, &m_pBuffers[x3]);

				if (field_B8)
					pChunk->field_50 = 0;

				pChunk->field_4E = false;
				pChunk->field_4D = true;
				pChunk->m_bVisible = true;
				pChunk->field_48 = x2++;
				pChunk->setDirty();
				m_chunks[index] = pChunk;
				field_98[index] = pChunk;

				x3 += 3;
				field_88.push_back(pChunk);
			}
		}
	}

	if (m_pLevel)
	{
		Mob* pMob = m_pMinecraft->m_pMobPersp;
		if (pMob)
		{
			resortChunks(Mth::floor(pMob->m_pos.x), Mth::floor(pMob->m_pos.y), Mth::floor(pMob->m_pos.z));

			std::sort(&field_98[0], &field_98[m_chunksLength], DistanceChunkSorter(pMob));
		}
	}

	field_14 = 2;
}

void LevelRenderer::resortChunks(int x, int y, int z)
{
	int field_A4; // r11
	int z1; // r2
	int field_AC; // r10
	int x2; // r0
	int x3; // r9
	int x5; // r3
	int x9; // r0
	int x10; // r0
	int field_78; // r3
	int x11; // r7
	int field_AC_1; // r5
	int field_A8; // r6
	int x12; // r0
	int x13; // r8
	int field_80; // r3
	int x14; // r5
	int field_A4_1; // r2
	int x15; // r6
	int x16; // r11
	int field_7C; // r3
	bool v25; // cc
	Chunk** m_chunks; // r3
	int x17; // r2
	bool bIsDirty; // r6
	int x6_1; // [sp+0h] [bp-58h]
	int x8_1; // [sp+4h] [bp-54h]
	int x1; // [sp+8h] [bp-50h]
	int x10_1; // [sp+Ch] [bp-4Ch]
	int x4; // [sp+10h] [bp-48h]
	int x7; // [sp+1Ch] [bp-3Ch]
	int x6; // [sp+20h] [bp-38h]
	int x8; // [sp+24h] [bp-34h]
	Chunk* pChunk; // [sp+2Ch] [bp-2Ch] BYREF

	field_A4 = this->field_A4;
	z1 = 8 - z;
	this->field_6C = 0x7FFFFFFF;
	this->field_70 = 0x7FFFFFFF;
	this->field_74 = 0x7FFFFFFF;
	this->field_78 = 0x80000000;
	this->field_7C = 0x80000000;
	this->field_80 = 0x80000000;
	if (field_A4 > 0)
	{
		x1 = 16 * field_A4;
		field_AC = this->field_AC;
		x2 = (16 * field_A4) >> 1;
		x3 = 0;
		x4 = x2 + 8 - x;
		x5 = 1 - 16 * field_A4;
		x6 = z1 + x2;
		x7 = x5 + x4;
		x8 = z1 + x2 + x5;
		while (1)
		{
			x9 = x4 & ~(x4 >> 31);
			if (x4 < 0)
				x9 = x7;
			x10 = 16 * x3 - x1 * (x9 / x1);
			field_78 = this->field_78;
			x10_1 = x10;
			if (x10 < this->field_6C)
				this->field_6C = x10;
			if (x10 > field_78)
				this->field_78 = x10;
			if (field_AC > 0)
				break;
		LABEL_32:
			++x3;
			x4 += 16;
			x7 += 16;
			if (field_A4 <= x3)
				return;
		}
		x11 = 0;
		field_AC_1 = field_AC;
		field_A8 = this->field_A8;
		x6_1 = x6;
		x8_1 = x8;
		while (1)
		{
			x12 = x6_1 & ~(x6_1 >> 31);
			if (x6_1 < 0)
				x12 = x8_1;
			x13 = 16 * x11 - x1 * (x12 / x1);
			field_80 = this->field_80;
			if (x13 < this->field_74)
				this->field_74 = x13;
			if (x13 > field_80)
				this->field_80 = x13;
			if (field_A8 > 0)
				break;
		LABEL_30:
			++x11;
			x6_1 += 16;
			x8_1 += 16;
			if (field_AC_1 <= x11)
			{
				field_AC = field_AC_1;
				goto LABEL_32;
			}
		}
		x14 = 0;
		for (field_A4_1 = field_A4; ; field_A4_1 = this->field_A4)
		{
			x15 = x14 + field_A8 * x11;
			x16 = 16 * x14;
			field_7C = this->field_7C;
			if (this->field_70 > 16 * x14)
				this->field_70 = x16;
			v25 = field_7C < x16;
			m_chunks = this->m_chunks;
			x17 = x3 + field_A4_1 * x15;
			if (v25)
				this->field_7C = x16;
			pChunk = m_chunks[x17];
			bIsDirty = pChunk->isDirty();
			pChunk->Chunk::setPos(x10_1, 16 * x14, x13);
			if (bIsDirty || !pChunk->isDirty())
				goto LABEL_19;

			if (true) // need to expand
				break;

			++x14;

			field_A8 = this->field_A8;

			if (field_A8 <= x14)
			{
			LABEL_29:
				field_A4 = this->field_A4;
				field_AC_1 = this->field_AC;
				goto LABEL_30;
			}
		LABEL_20:
			;
		}
		this->field_88.push_back(pChunk);
	LABEL_19:
		field_A8 = this->field_A8;
		if (field_A8 <= ++x14)
			goto LABEL_29;
		goto LABEL_20;
	}
}

void LevelRenderer::entityAdded(Entity* pEnt)
{
	// TODO
}

std::string LevelRenderer::gatherStats1()
{
	//@NOTE: This data is based on the Java Edition pre-1.8 legend. This may not be accurate, but it's a good guideline.
	//See https://minecraft.fandom.com/wiki/Debug_screen#Pre-1.8_legend
	std::stringstream ss;
	ss  << "C: " << field_60 << "/" << field_54 // Number of chunk sections rendered over total number of chunks.
		<< ". F: " << field_58 // Number of chunk sections loaded outside the viewing distance.
		<< ", O: " << field_5C // Number of occluded chunk sections.
		<< ", E: " << field_64 // Number of empty chunk sections.
		<< "\n";

	return ss.str();
}

void LevelRenderer::onGraphicsReset()
{
	xglGenBuffers(m_nBuffers, m_pBuffers);
	allChanged();
}

void LevelRenderer::render(const AABB& aabb) const
{
	Tesselator& t = Tesselator::instance;

	t.begin(GL_LINE_STRIP);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.draw();
	t.begin(GL_LINE_STRIP);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.draw();
	t.begin(GL_LINES);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.draw();
}

void LevelRenderer::checkQueryResults(int a, int b)
{
}

void LevelRenderer::renderSameAsLast(int a, float b)
{
	m_renderList.render();
}

int LevelRenderer::renderChunks(int start, int end, int a, float b)
{
	field_24.clear();

	int result = 0;
	for (int i = start; i < end; i++)
	{
		Chunk* pChunk = field_98[i];
		if (!a)
		{
			field_54++;
			if (pChunk->field_1C[0])
			{
				field_64++;
			}
			else if (pChunk->m_bVisible)
			{
				if (!field_B8 || pChunk->field_4D)
					field_60++;
				else
					field_5C++;
			}
			else
			{
				field_58++;
			}
		}

		if (!pChunk->field_1C[a] && pChunk->m_bVisible && pChunk->field_4D && pChunk->getList(a) >= 0)
		{
			result++;
			field_24.push_back(pChunk);
		}
	}

	Mob* pMob = m_pMinecraft->m_pMobPersp;

	float fPosX = pMob->field_98.x + (pMob->m_pos.x - pMob->field_98.x) * b;
	float fPosY = pMob->field_98.y + (pMob->m_pos.y - pMob->field_98.y) * b;
	float fPosZ = pMob->field_98.z + (pMob->m_pos.z - pMob->field_98.z) * b;

	m_renderList.clear();
	m_renderList.init(fPosX, fPosY, fPosZ);

	for (auto pChk : field_24)
	{
		m_renderList.addR(*pChk->getRenderChunk(a));
		m_renderList.field_14++;
	}

	renderSameAsLast(a, b);
	return result;
}

void LevelRenderer::render(Mob* pMob, int a, float b)
{
	for (int i = 0; i < 10; i++)
	{
		field_68 = (field_68 + 1) % m_chunksLength;
		Chunk* pChunk = m_chunks[field_68];

		if (!pChunk->m_bDirty)
			continue;

		auto iter = std::find(field_88.begin(), field_88.end(), pChunk);
		if (iter != field_88.end())
			continue;

		field_88.push_back(pChunk);
	}

	if (m_pMinecraft->m_options.field_10 != field_BC)
		allChanged();

	if (!a)
		field_54 = field_58 = field_5C = field_60 = field_64 = 0;

	float mobX1 = pMob->m_pos.x;
	float mobX2 = pMob->field_98.x + (pMob->m_pos.x - pMob->field_98.x) * b;
	float mobY1 = pMob->m_pos.y;
	float mobY2 = pMob->field_98.y + (pMob->m_pos.y - pMob->field_98.y) * b;
	float mobZ1 = pMob->m_pos.z;
	float mobZ2 = pMob->field_98.z + (pMob->m_pos.z - pMob->field_98.z) * b;

	float dX = pMob->m_pos.x - field_4, dY = pMob->m_pos.y - field_8, dZ = pMob->m_pos.z - field_C;

	if (dX * dX + dY * dY + dZ * dZ > 16.0f)
	{
		field_4 = pMob->m_pos.x;
		field_8 = pMob->m_pos.y;
		field_C = pMob->m_pos.z;

		resortChunks(Mth::floor(pMob->m_pos.x), Mth::floor(pMob->m_pos.y), Mth::floor(pMob->m_pos.z));
		std::sort(&field_98[0], &field_98[m_chunksLength], DistanceChunkSorter(pMob));
	}

	// @TODO: Fix goto hell

	// @NOTE: Field_B8 doesn't appear to be used??
	if (field_B8 && !a && !m_pMinecraft->m_options.m_bAnaglyphs)
	{
		checkQueryResults(0, 16);

		// @HUH: why 16?
		for (int i = 0; i < 16; i++)
		{
			field_98[i]->field_4D = true;
		}

		int x1 = renderChunks(0, 16, 0, b);
		int x2 = 16, x3;

		while (true)
		{
			x3 = 2 * x2;
			if (x3 >= m_chunksLength)
				x3 = m_chunksLength;

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_FOG);
			glColorMask(false, false, false, false);
			glDepthMask(false);
			if (x2 < x3)
				break;
		label_37:
			glDepthMask(true);
			glColorMask(true, true, true, true);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_FOG);

			int res = renderChunks(x2, x3, 0, b);
			x1 += res;
			if (x3 >= m_chunksLength)
				return;
			x2 = x3;
		}

		float y1 = 0.0f;
		int y2 = x2;
		int y3 = x2;
		float y4 = 0.0f, y5 = 0.0f;

		while (true)
		{
			while (!field_98[y2]->isEmpty())
			{
				Chunk* pChunk = field_98[y2];

				if (!pChunk->m_bVisible)
				{
					pChunk->field_4D = true;
					goto label_26;
				}

				if (pChunk->field_4E)
					goto label_26;

				float y6 = pChunk->distanceToSqr(pMob);
				int y7 = int(Mth::sqrt(y6) / 128.0f + 1.0f);

				if (field_C0 % y7 != y3 % y7)
					goto label_26;

				float fXdiff, fYdiff, fZdiff;
				fXdiff = float(pChunk->m_pos.x) - mobX2 - y5;
				fYdiff = float(pChunk->m_pos.y) - mobY2 - y4;
				fZdiff = float(pChunk->m_pos.z) - mobZ2 - y1;

				if (fXdiff != 0.0f || fYdiff != 0.0f || fZdiff != 0.0f)
				{
					y5 += fXdiff;
					y4 += fYdiff;
					y1 += fZdiff;
					glTranslatef(fXdiff, fYdiff, fZdiff);
				}

				pChunk->renderBB();

				y3++;
				y2++;
				//pChunk->field_4E++;
				pChunk->field_4E = true;
				if (y3 == x3)
					goto label_37;
			}

			field_98[y2]->m_bVisible = 0;
		label_26:
			y3++;
			y2++;

			if (y3 == x3)
				goto label_37;

			continue;
		}
	}

	renderChunks(0, m_chunksLength, a, b);
}

void LevelRenderer::setLevel(Level* level)
{
	if (m_pLevel)
		m_pLevel->removeListener(this);

	field_4 = -9999.0f;
	field_8 = -9999.0f;
	field_C = -9999.0f;

	EntityRenderDispatcher::getInstance()->setLevel(level);
	EntityRenderDispatcher::getInstance()->setMinecraft(m_pMinecraft);

	m_pLevel = level;

	delete m_pTileRenderer;
	m_pTileRenderer = new TileRenderer(m_pLevel);

	if (level)
	{
		level->addListener(this);
		allChanged();
	}
}

void LevelRenderer::setDirty(int x1, int y1, int z1, int x2, int y2, int z2)
{
	int x1c = Mth::intFloorDiv(x1, 16);
	int y1c = Mth::intFloorDiv(y1, 16);
	int z1c = Mth::intFloorDiv(z1, 16);
	int x2c = Mth::intFloorDiv(x2, 16);
	int y2c = Mth::intFloorDiv(y2, 16);
	int z2c = Mth::intFloorDiv(z2, 16);

	for (int x = x1c; x <= x2c; x++)
	{
		int x1 = x % field_A4;
		if (x1 < 0)
			x1 += field_A4;

		for (int y = y1c; y <= y2c; y++)
		{
			int y1 = y % field_A8;
			if (y1 < 0)
				y1 += field_A8;

			for (int z = z1c; z <= z2c; z++)
			{
				int z1 = z % field_AC;
				if (z1 < 0)
					z1 += field_AC;

				Chunk* pChunk = m_chunks[x1 + field_A4 * (y1 + field_A8 * z1)];
				if (pChunk->isDirty())
					continue;

				field_88.push_back(pChunk);
				pChunk->setDirty();
			}
		}
	}
}

void LevelRenderer::setTilesDirty(int x1, int y1, int z1, int x2, int y2, int z2)
{
	setDirty(x1 - 1, y1 - 1, z1 - 1, x2 + 1, y2 + 1, z2 + 1);
}

void LevelRenderer::tick()
{
	field_C0++;
}

/*
void LevelRenderer::updateDirtyChunks(Mob* pMob, bool b)
{
	// @TODO This updates 16 chunks per frame. Not good.

	int updated = 0;
	for (int i = 0; i < 16 && i < int(field_88.size()); i++)
	{
		Chunk* pChk = field_88[i];
		pChk->rebuild();
		pChk->setClean();
		updated++;
	}

	field_88.erase(field_88.begin(), field_88.begin() + updated);
}
*/

typedef std::vector<Chunk*> ChunkVector;
typedef ChunkVector::iterator ChunkVectorIterator;

bool LevelRenderer::updateDirtyChunks(Mob* pMob, bool b)
{
	// @TODO: untangle this thing

	int v3; // r4
	ChunkVectorIterator field_88_Beg; // r3
	size_t size; // r9
	int v8; // r11
	Chunk* v9; // r0
	ChunkVector* xvec; // r5
	int v11; // r7
	ChunkVectorIterator v12; // r1
	int v13; // r7
	Chunk* v14; // r1
	bool v15; // r0
	int v16; // r3
	int v17; // r7
	int v18; // r2
	ChunkVectorIterator v19; // r1
	ChunkVectorIterator v20; // r0
	size_t v21; // r8
	size_t v22; // r4
	ChunkVector* v23; // r10
	size_t v24; // r8
	Chunk* v25; // r5
	int v26; // r4
	int v27; // r8
	Chunk* v28; // r5
	bool v29; // r3
	ChunkVectorIterator v31; // r1
	size_t v32; // r5
	int v33; // r0
	int v34; // r3
	Chunk* v35; // r2
	Chunk** v38; // r3
	Chunk* v39; // r2
	ChunkVector* v40; // r0
	Chunk* v42[3]; // [sp+1Ch] [bp+0h] BYREF
	Chunk* a3; // [sp+28h] [bp+Ch] BYREF
	Entity* pMob_1; // [sp+2Ch] [bp+10h] BYREF

	v3 = 0;
	pMob_1 = pMob;
	DirtyChunkSorter dcs(pMob);
	memset(v42, 0, sizeof v42);
	field_88_Beg = this->field_88.begin();
	size = this->field_88.end() - field_88_Beg;
	if (size <= 0)
	{
		v8 = 0;
		goto LABEL_28;
	}
	v8 = 0;
	v9 = *field_88_Beg;
	xvec = 0;
	v11 = 0;
	a3 = *field_88_Beg;
	if (!b)
		goto LABEL_11;
	while (1)
	{
		if (!v9->m_bVisible)
			goto LABEL_9;
	LABEL_5:
		if (!xvec)
		{
			++v8;
			v40 = new ChunkVector;
			v12 = v40->end();
			xvec = v40;
		LABEL_55:
			xvec->insert(v12, a3);
			goto LABEL_8;
		}
		v12 = xvec->end();
		++v8;
		if (true) // (v12 == xvec->capacity)
			goto LABEL_55;
		xvec->insert(v12, a3);
	LABEL_8:
		field_88[v11] = 0;
	LABEL_9:

		if (++v3 == size)
			break;
		while (1)
		{
			v11 = v3;
			v9 = field_88[v3];
			a3 = v9;
			if (b)
				break;
		LABEL_11:
			if (v9->distanceToSqr(pMob) <= 1024.0f)
				goto LABEL_5;
			v13 = b;
			while (1)
			{
				v14 = v42[v13];
				if (v14)
				{
					v15 = dcs(v14, a3);
					v16 = v13;
					if (!v15)
						break;
				}
				if (++v13 == 3)
				{
					v17 = 2;
					v18 = 1;
					v16 = 3;
					goto LABEL_51;
				}
			}
			v17 = v13 - 1;
			if (v17 <= 0)
				goto LABEL_9;
			v18 = v17 - 1;
			if (v17 == 1)
			{
				v42[1] = a3;
				goto LABEL_18;
			}
		LABEL_51:
			v38 = &v42[v16];
			v39 = v42[v18];
			do
			{
				*(v38 - 3) = v39;
				--v38;
			} while (v38 != &v42[2]);
			v42[v17] = a3;
		LABEL_18:
			if (++v3 == size)
				goto LABEL_19;
		}
	}
LABEL_19:
	if (xvec)
	{
		v19 = xvec->end();
		v20 = xvec->begin();
		v21 = v19 - xvec->begin();
		if (v21 > 1)
		{
			std::sort(v20, v19, dcs);
			v20 = xvec->begin();
			v21 = xvec->end() - xvec->begin();
		}
		v22 = v21 - 1;
		if ((int)(v21 - 1) >= 0)
		{
			v23 = xvec;
			v24 = v21 - 1;// v21 + 0x3FFFFFFF;
			while (1)
			{
				v25 = v20[v24--];
				v25->rebuild();
				v25->setClean();
				if ((--v22 & 0x80000000) != 0)
					break;
				v20 = v23->begin();
			}
			xvec = v23;
		}

		delete xvec;
	}
LABEL_28:
	v26 = 2;
	v27 = 0;
	while (2)
	{
		v28 = v42[v26];
		if (!v28)
		{
		LABEL_33:
			if (v26-- == 0)
				goto LABEL_34;
			continue;
		}
		break;
	}
	v29 = v28->m_bVisible;
	if (v28->m_bVisible || v26 == 2)
	{
		++v27;
		v42[v26]->rebuild();
		v28->setClean();
		goto LABEL_33;
	}
	v42[v26] = (Chunk*)v29;
	v42[0] = (Chunk*)v29;
LABEL_34:
	v31 = this->field_88.begin();
	v32 = this->field_88.end() - v31;
	if (v32)
	{
		v33 = 0;
		v34 = 0;
		while (1)
		{
			v35 = v31[v34];
			if (v35 && v42[0] != v35 && v42[1] != v35 && v42[2] != v35)
			{
				if (v33 != v34)
					v31[v33] = v35;
				++v33;
			}
			if (++v34 == v32)
				break;
			v31 = this->field_88.begin();
		}
		if (v34 > v33)
		{
			field_88.resize(v33);
		}
	}
	return v27 + v8 == size;
}

void LevelRenderer::renderHitSelect(Player* pPlayer, const HitResult& hr, int i, void* vp, float f)
{
	if (i) return;

	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
	glEnable(GL_DEPTH_TEST);

	m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

	Tile* pTile = nullptr;
	TileID tileID = m_pLevel->getTile(hr.m_tileX, hr.m_tileY, hr.m_tileZ);
	if (tileID > 0)
		pTile = Tile::tiles[tileID];

	glDisable(GL_ALPHA_TEST);
	glColor4f(0.65f, 0.65f, 0.65f, 0.65f);
	glPushMatrix();
	glPolygonOffset(-0.3f, -0.3f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	float px = pPlayer->field_98.x + (pPlayer->m_pos.x - pPlayer->field_98.x) * f;
	float py = pPlayer->field_98.y + (pPlayer->m_pos.y - pPlayer->field_98.y) * f;
	float pz = pPlayer->field_98.z + (pPlayer->m_pos.z - pPlayer->field_98.z) * f;

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.offset(-px, -py, -pz);
	t.noColor();
	if (!pTile)
		pTile = Tile::rock;

	m_pTileRenderer->tesselateInWorld(pTile, hr.m_tileX, hr.m_tileY, hr.m_tileZ);

	t.draw();
	t.offset(0, 0, 0);

	glPolygonOffset(0.0f, 0.0f);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(true);
	glPopMatrix();
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

void LevelRenderer::tileChanged(int x, int y, int z)
{
	setDirty(x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

void LevelRenderer::renderEntities(Vec3 pos, Culler* culler, float f)
{
	if (field_14 > 0)
	{
		field_14--;
		return;
	}

	Mob* mob = m_pMinecraft->m_pMobPersp;

	EntityRenderDispatcher::getInstance()->prepare(m_pLevel, m_pMinecraft->m_pTextures, m_pMinecraft->m_pFont, mob, &m_pMinecraft->m_options, f);

	field_18 = 0;
	field_1C = 0;
	field_20 = 0;

	EntityRenderDispatcher::xOff = mob->field_98.x + (mob->m_pos.x - mob->field_98.x) * f;
	EntityRenderDispatcher::yOff = mob->field_98.y + (mob->m_pos.y - mob->field_98.y) * f;
	EntityRenderDispatcher::zOff = mob->field_98.z + (mob->m_pos.z - mob->field_98.z) * f;

	EntityVector* pVec = m_pLevel->getAllEntities();
	field_18 = int(pVec->size());

	for (int i = 0; i < field_18; i++)
	{
		Entity* pEnt = (*pVec)[i];
		if (!pEnt->shouldRender(pos))
			continue;

		if (!culler->isVisible(pEnt->m_hitbox))
			continue;

		if (m_pMinecraft->m_pMobPersp == pEnt && !m_pMinecraft->m_options.field_23D) // @NOTE: third person ?
			continue;

		if (m_pLevel->hasChunkAt(Mth::floor(pEnt->m_pos.x), Mth::floor(pEnt->m_pos.y), Mth::floor(pEnt->m_pos.z)))
		{
			field_1C++;
			EntityRenderDispatcher::getInstance()->render(pEnt, f);
		}
	}
}

extern int t_keepPic;

void LevelRenderer::takePicture(TripodCamera* pCamera, Entity* pOwner)
{
	Mob* pOldMob = m_pMinecraft->m_pMobPersp;
	bool bOldField23C = m_pMinecraft->m_options.field_23C;
	bool bOldField23D = m_pMinecraft->m_options.field_23D;

#ifdef ENH_CAMERA_NO_PARTICLES
	extern bool g_bDisableParticles;
	g_bDisableParticles = true;
#endif

	m_pMinecraft->m_pMobPersp = pCamera;
	m_pMinecraft->m_options.field_23C = true;
	m_pMinecraft->m_options.field_23D = false; // really from the perspective of the camera
	m_pMinecraft->m_pGameRenderer->render(0.0f);
	m_pMinecraft->m_pMobPersp = pOldMob;
	m_pMinecraft->m_options.field_23C = bOldField23C;
	m_pMinecraft->m_options.field_23D = bOldField23D;

#ifdef ENH_CAMERA_NO_PARTICLES
	g_bDisableParticles = false;
#endif

	t_keepPic = -1;

	static char str[256];
	// @HUH: This has the potential to overwrite a file
#ifdef ORIGINAL_CODE
	sprintf(str, "%s/games/com.mojang/img_%.4d.jpg", m_pMinecraft->m_externalStorageDir.c_str(), getTimeMs());
#else
	sprintf(str, "img_%.4d.png", getTimeMs());
#endif

	m_pMinecraft->platform()->saveScreenshot(std::string(str), Minecraft::width, Minecraft::height);
}

void LevelRenderer::addParticle(const std::string& name, float x, float y, float z, float vx, float vy, float vz)
{
	if (m_pMinecraft->m_pMobPersp->distanceToSqr_inline(x, y, z) > 256.0f)
		return;

	ParticleEngine* pe = m_pMinecraft->m_pParticleEngine;
	if (name == "bubble")
	{
		pe->add(new BubbleParticle(m_pLevel, x, y, z, vx, vy, vz));
		return;
	}
	if (name == "smoke")
	{
		pe->add(new SmokeParticle(m_pLevel, x, y, z, vx, vy, vz, 1.0f));
		return;
	}
	if (name == "explode")
	{
		pe->add(new ExplodeParticle(m_pLevel, x, y, z, vx, vy, vz));
		return;
	}
	if (name == "flame")
	{
		pe->add(new FlameParticle(m_pLevel, x, y, z, vx, vy, vz));
		return;
	}
	if (name == "lava")
	{
		pe->add(new LavaParticle(m_pLevel, x, y, z));
		return;
	}
	if (name == "largesmoke")
	{
		pe->add(new SmokeParticle(m_pLevel, x, y, z, vx, vy, vz, 2.5f));
		return;
	}
	if (name == "reddust")
	{
		pe->add(new RedDustParticle(m_pLevel, x, y, z, vx, vy, vz));
		return;
	}

#ifndef ORIGINAL_CODE
	//LogMsg("Unknown particle type: %s", name.c_str());
#endif
}

void LevelRenderer::playSound(const std::string& name, float x, float y, float z, float a, float b)
{
	float mult = 1.0f, dist = 16.0f;

	if (a > 1.0f)
	{
		mult = 16.0f;
		dist = a * mult;
	}

	if (dist * dist > m_pMinecraft->m_pMobPersp->distanceToSqr(x, y, z))
		m_pMinecraft->m_pSoundEngine->play(name, x, y, z, a, b);
}

void LevelRenderer::renderSky(float f)
{
	if (m_pMinecraft->m_pLevel->m_pDimension->field_C)
		return;

	glDisable(GL_TEXTURE_2D);

	Vec3 skyColor = m_pLevel->getSkyColor(m_pMinecraft->m_pMobPersp, f);
	if (m_pMinecraft->m_options.m_bAnaglyphs)
	{
		skyColor.x = (((skyColor.x * 30.0f) + (skyColor.y * 59.0f)) + (skyColor.z * 11.0f)) / 100.0f;
		skyColor.y = ((skyColor.x * 30.0f) + (skyColor.y * 70.0f)) / 100.0f;
		skyColor.z = ((skyColor.x * 30.0f) + (skyColor.z * 70.0f)) / 100.0f;
	}

	glColor4f(skyColor.x, skyColor.y, Mth::Min(1.0f, skyColor.z), 1.0f);
	glDepthMask(false);
	glColor4f(skyColor.x, skyColor.y, skyColor.z, 1.0f);
	drawArrayVT(field_D8, field_DC, sizeof(Tesselator::Vertex));
	glEnable(GL_TEXTURE_2D);
	glDepthMask(true);
}

void LevelRenderer::skyColorChanged()
{
	for (int i = 0; i < m_chunksLength; i++)
	{
		Chunk* pChunk = m_chunks[i];
		if (!pChunk->field_54)
			continue;

		if (pChunk->isDirty())
			continue;

		field_88.push_back(pChunk);
		pChunk->setDirty();
	}
}
