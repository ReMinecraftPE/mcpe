/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/GL/GL.hpp"
#include "common/Mth.hpp"

#include "world/tile/LeafTile.hpp"
#include "world/tile/GrassTile.hpp"

bool LevelRenderer::_areCloudsAvailable = false; // false because 0.1 didn't have them

LevelRenderer::LevelRenderer(Minecraft* pMC, Textures* pTexs)
{
	field_4 = -9999.0f;
	field_8 = -9999.0f;
	field_C = -9999.0f;
	field_10 = 0.0f;
	field_14 = 2;
	field_18 = 0;
	field_1C = 0;
	field_20 = 0;
	field_30 = 0;
	field_54 = 0;
	field_58 = 0;
	field_5C = 0;
	field_60 = 0;
	field_64 = 0;
	field_68 = 0;
	m_resortedMinX = 0;
	m_resortedMinY = 0;
	m_resortedMinZ = 0;
	m_resortedMaxX = 0;
	m_resortedMaxY = 0;
	m_resortedMaxZ = 0;
	m_pLevel = nullptr;
	m_chunks = nullptr;
	field_98 = nullptr;
	m_chunksLength = 0;
	m_pTileRenderer = nullptr;
	field_A4 = 0;
	field_A8 = 0;
	field_AC = 0;
	field_B0 = 0;
	field_B8 = false;
	field_BC = -1;
	m_ticksSinceStart = 0;
	m_nBuffers = 26136;

	m_pMinecraft = pMC;
	m_pTextures = pTexs;

	m_pBuffers = new GLuint[m_nBuffers];
	xglGenBuffers(m_nBuffers, m_pBuffers);
	LOG_I("numBuffers: %d", m_nBuffers);

	xglGenBuffers(1, &m_skyBuffer);

	generateSky(); // inlined in the 0.1.0 demo
}

void LevelRenderer::generateSky()
{
	Tesselator& t = Tesselator::instance;
	t.begin();
	m_skyBufferCount = 0;

	//float m = 16.0f;
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

			m_skyBufferCount += 4;
		}
	}

	t.end(m_skyBuffer);
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

	pLeaves->m_bTransparent = m_pMinecraft->getOptions()->m_bFancyGraphics;
	pLeaves->m_TextureFrame = !pLeaves->m_bTransparent + pLeaves->field_74;

	TileRenderer::m_bFancyGrass = m_pMinecraft->getOptions()->m_bFancyGrass;
	TileRenderer::m_bBiomeColors = m_pMinecraft->getOptions()->m_bBiomeColors;

	field_BC = m_pMinecraft->getOptions()->m_iViewDistance;

	int x1 = 64 << (3 - field_BC);
	if (x1 >= 400)
		x1 = 400;

	field_A4 = x1 / 16 + 1;
	field_AC = x1 / 16 + 1;
	field_A8 = 8;

	m_chunksLength = field_A8 * field_A4 * field_AC;
	LOG_I("chunksLength: %d", m_chunksLength);
	m_chunks = new Chunk* [m_chunksLength];
	field_98 = new Chunk* [m_chunksLength];

	m_resortedMinX = 0;
	m_resortedMinY = 0;
	m_resortedMinZ = 0;

	field_88.clear();

	m_resortedMaxX = field_A4;
	m_resortedMaxY = field_AC;
	m_resortedMaxZ = field_A8;

	int x2 = 0, x3 = 0;

	// These are actually Chunk coordinates that get converted to Tile coordinates
	TilePos cp(0, 0, 0);
	for (cp.x = 0; cp.x < field_A4; cp.x++)
	{
		if (field_A8 <= 0)
			continue;

		for (cp.y = 0; cp.y < field_A8; cp.y++)
		{
			for (cp.z = 0; cp.z < field_AC; cp.z++)
			{
				int index = cp.x + field_A4 * (cp.y + field_A8 * cp.z);

				Chunk* pChunk = new Chunk(m_pLevel, cp * 16, 16, x3 + field_B0, &m_pBuffers[x3]);

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
			resortChunks(pMob->m_pos);

			std::sort(&field_98[0], &field_98[m_chunksLength], DistanceChunkSorter(pMob));
		}
	}

	field_14 = 2;
}

void LevelRenderer::resortChunks(const TilePos& pos)
{
	TilePos tp(pos - 8);
	m_resortedMinX = 0x7FFFFFFF;
	m_resortedMinY = 0x7FFFFFFF;
	m_resortedMinZ = 0x7FFFFFFF;
	m_resortedMaxX = 0x80000000;
	m_resortedMaxY = 0x80000000;
	m_resortedMaxZ = 0x80000000;

	int blkCount = field_A4 * 16;
	int blkCntHalf = blkCount / 2;

	for (int fx = 0; fx < field_A4; fx++)
	{
		int x1 = 16 * fx;
		int x2 = x1 + blkCntHalf - tp.x;
		if (x2 < 0) x2 -= blkCount - 1;
		x2 /= blkCount;
		x1 -= blkCount * x2;

		if (m_resortedMinX > x1)
			m_resortedMinX = x1;
		if (m_resortedMaxX < x1)
			m_resortedMaxX = x1;

		for (int fz = 0; fz < field_AC; fz++)
		{
			int z1 = 16 * fz;
			int z2 = z1 + blkCntHalf - tp.z;
			if (z2 < 0) z2 -= blkCount - 1;
			z2 /= blkCount;
			z1 -= blkCount * z2;

			if (m_resortedMinZ > z1)
				m_resortedMinZ = z1;
			if (m_resortedMaxZ < z1)
				m_resortedMaxZ = z1;

			for (int fy = 0; fy < field_A8; fy++)
			{
				int y1 = 16 * fy;
				if (m_resortedMinY > y1)
					m_resortedMinY = y1;
				if (m_resortedMaxY < y1)
					m_resortedMaxY = y1;

				Chunk* pChunk = m_chunks[(fz * field_A8 + fy) * field_A4 + fx];
				bool wasDirty = pChunk->isDirty();
				pChunk->setPos(TilePos(x1, y1, z1));

				if (!wasDirty && pChunk->isDirty())
					field_88.push_back(pChunk);
			}
		}
	}
}

void LevelRenderer::entityAdded(Entity* pEnt)
{
	if (pEnt->isPlayer())
		// @HUH: Why would you do that?
		EntityRenderDispatcher::getInstance()->onGraphicsReset();
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

	xglGenBuffers(1, &m_skyBuffer);
	generateSky(); // inlined in the 0.1.0 demo
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

	for (int i = 0; i < int(field_24.size()); i++)
	{
		Chunk* pChk = field_24[i];
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

		std::vector<Chunk*>::iterator iter = std::find(field_88.begin(), field_88.end(), pChunk);
		if (iter != field_88.end())
			continue;

		field_88.push_back(pChunk);
	}

	if (m_pMinecraft->getOptions()->m_iViewDistance != field_BC)
		allChanged();

	if (!a)
		field_54 = field_58 = field_5C = field_60 = field_64 = 0;

	Vec3 mobPos = pMob->field_98 + (pMob->m_pos - pMob->field_98) * b;

	float dX = pMob->m_pos.x - field_4, dY = pMob->m_pos.y - field_8, dZ = pMob->m_pos.z - field_C;

	if (dX * dX + dY * dY + dZ * dZ > 16.0f)
	{
		field_4 = pMob->m_pos.x;
		field_8 = pMob->m_pos.y;
		field_C = pMob->m_pos.z;

		resortChunks(pMob->m_pos);
		std::sort(&field_98[0], &field_98[m_chunksLength], DistanceChunkSorter(pMob));
	}

	// @NOTE: Field_B8 doesn't appear to be used??
	if (field_B8 && !a && !m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		int c = 16;
		checkQueryResults(0, c);
		
		// @HUH: why 16?
		for (int i = 0; i < c; i++)
			field_98[i]->field_4D = true;

		int d = renderChunks(0, c, 0, b);

		do
		{
			int cold = c;
			c *= 2;
			if (c > m_chunksLength)
				c = m_chunksLength;

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_FOG);
			glColorMask(false, false, false, false);
			glDepthMask(false);
			checkQueryResults(cold, c);

			glPushMatrix();

			float lastX = 0.0f, lastY = 0.0f, lastZ = 0.0f;

			for (int i = cold; i < c; i++)
			{
				if (m_chunks[i]->isEmpty())
				{
					m_chunks[i]->m_bVisible = false;
					continue;
				}

				if (!m_chunks[i]->m_bVisible)
				{
					m_chunks[i]->field_4D = true;
					continue;
				}

				if (!m_chunks[i]->field_4E)
					continue;

				int roughDist = int(Mth::sqrt(m_chunks[i]->distanceToSqr(pMob)) / 128.0f + 1.0f);

				if (m_ticksSinceStart % roughDist != i % roughDist)
					continue;
				
				float fXdiff, fYdiff, fZdiff;
				fXdiff = float(m_chunks[i]->m_pos.x) - mobPos.x - lastX;
				fYdiff = float(m_chunks[i]->m_pos.y) - mobPos.y - lastY;
				fZdiff = float(m_chunks[i]->m_pos.z) - mobPos.z - lastZ;

				if (fXdiff != 0.0f || fYdiff != 0.0f || fZdiff != 0.0f)
				{
					glTranslatef(fXdiff, fYdiff, fZdiff);

					lastX += fXdiff;
					lastY += fYdiff;
					lastZ += fZdiff;
				}

				m_chunks[i]->renderBB();
				m_chunks[i]->field_4E = true;
			}
		}
		while (c < m_chunksLength);
	}
	else
	{
		renderChunks(0, m_chunksLength, a, b);
	}
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

void LevelRenderer::setDirty(const TilePos& min, const TilePos& max)
{
	int minX = Mth::intFloorDiv(min.x, 16);
	int minY = Mth::intFloorDiv(min.y, 16);
	int minZ = Mth::intFloorDiv(min.z, 16);
	int maxX = Mth::intFloorDiv(max.x, 16);
	int maxY = Mth::intFloorDiv(max.y, 16);
	int maxZ = Mth::intFloorDiv(max.z, 16);

	for (int x = minX; x <= maxX; x++)
	{
		int x1 = x % field_A4;
		if (x1 < 0)
			x1 += field_A4;

		for (int y = minY; y <= maxY; y++)
		{
			int y1 = y % field_A8;
			if (y1 < 0)
				y1 += field_A8;

			for (int z = minZ; z <= maxZ; z++)
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

void LevelRenderer::setTilesDirty(const TilePos& min, const TilePos& max)
{
	setDirty(min - 1, max + 1);
}

void LevelRenderer::tick()
{
	m_ticksSinceStart++;
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

void LevelRenderer::renderHit(Player* pPlayer, const HitResult& hr, int i, void* vp, float f)
{
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// @BUG: possible leftover from Minecraft Classic? This is overridden anyways
	//glColor4f(1.0f, 1.0f, 1.0f, 0.5f * (0.4f + 0.2f * Mth::sin(float(getTimeMs()) / 100.0f)));

	if (!i && field_10 > 0.0f)
	{
		glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

		m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glPushMatrix();
		Tile* pTile = nullptr;
		TileID tile = m_pLevel->getTile(hr.m_tilePos);
		if (tile > 0)
			pTile = Tile::tiles[tile];
		glDisable(GL_ALPHA_TEST);
		glPolygonOffset(-3.0f, -3.0f);
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

		m_pTileRenderer->tesselateInWorld(pTile, hr.m_tilePos, 240 + int(field_10 * 10.0f));

		t.draw();
		t.offset(0, 0, 0);

		glPolygonOffset(0.0f, 0.0f);
		glDisable(GL_POLYGON_OFFSET_FILL);

		glDepthMask(true); //@HUH: What is the reason for this? You never disable the depth mask.
		glPopMatrix();
	}

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
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
	TileID tileID = m_pLevel->getTile(hr.m_tilePos);
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

	m_pTileRenderer->tesselateInWorld(pTile, hr.m_tilePos);

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

void LevelRenderer::renderHitOutline(Player* pPlayer, const HitResult& hr, int i, void* vp, float f)
{
	if (i != 0 || hr.m_hitType != 0)
		return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
	//glLineWidth(1.0f);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(false);

	// Maximize Line Width
	glEnable(GL_LINE_SMOOTH);
	
	float range[2];
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, range);

	float lineWidth = 2.0f;
	if (lineWidth > range[1])
		lineWidth = range[1];

	glLineWidth(lineWidth);

	TileID tile = m_pLevel->getTile(hr.m_tilePos);
	if (tile > 0)
	{
		Tile::tiles[tile]->updateShape(
			m_pLevel,
			hr.m_tilePos);
		float posX = pPlayer->field_98.x + ((pPlayer->m_pos.x - pPlayer->field_98.x) * f);
		float posY = pPlayer->field_98.y + ((pPlayer->m_pos.y - pPlayer->field_98.y) * f);
		float posZ = pPlayer->field_98.z + ((pPlayer->m_pos.z - pPlayer->field_98.z) * f);
		AABB aabb, tileAABB = Tile::tiles[tile]->getTileAABB(m_pLevel, hr.m_tilePos);
		aabb.min.y = tileAABB.min.y - 0.002f - posY;
		aabb.max.y = tileAABB.max.y + 0.002f - posY;
		aabb.min.z = tileAABB.min.z - 0.002f - posZ;
		aabb.max.z = tileAABB.max.z + 0.002f - posZ;
		aabb.min.x = tileAABB.min.x - 0.002f - posX;
		aabb.max.x = tileAABB.max.x + 0.002f - posX;
		render(aabb);
	}

	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void LevelRenderer::tileChanged(const TilePos& pos)
{
	setDirty(pos - 1, pos + 1);
}

void LevelRenderer::renderEntities(Vec3 pos, Culler* culler, float f)
{
	if (field_14 > 0)
	{
		field_14--;
		return;
	}

	Mob* mob = m_pMinecraft->m_pMobPersp;

	EntityRenderDispatcher::getInstance()->prepare(m_pLevel, m_pMinecraft->m_pTextures, m_pMinecraft->m_pFont, mob, m_pMinecraft->getOptions(), f);

	field_18 = 0;
	field_1C = 0;
	field_20 = 0;

	EntityRenderDispatcher::xOff = mob->field_98.x + (mob->m_pos.x - mob->field_98.x) * f;
	EntityRenderDispatcher::yOff = mob->field_98.y + (mob->m_pos.y - mob->field_98.y) * f;
	EntityRenderDispatcher::zOff = mob->field_98.z + (mob->m_pos.z - mob->field_98.z) * f;

	const EntityVector* pVec = m_pLevel->getAllEntities();
	field_18 = int(pVec->size());

	for (int i = 0; i < field_18; i++)
	{
		Entity* pEnt = (*pVec)[i];
		if (!pEnt->shouldRender(pos))
			continue;

		if (!culler->isVisible(pEnt->m_hitbox))
			continue;

		if (m_pMinecraft->m_pMobPersp == pEnt && !m_pMinecraft->getOptions()->m_bThirdPerson)
			continue;

		if (m_pLevel->hasChunkAt(pEnt->m_pos))
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
	bool bOldDontRenderGui = m_pMinecraft->getOptions()->m_bDontRenderGui;
	bool bOldThirdPerson = m_pMinecraft->getOptions()->m_bThirdPerson;

#ifdef ENH_CAMERA_NO_PARTICLES
	extern bool g_bDisableParticles;
	g_bDisableParticles = true;
#endif

	m_pMinecraft->m_pMobPersp = pCamera;
	m_pMinecraft->getOptions()->m_bDontRenderGui = true;
	m_pMinecraft->getOptions()->m_bThirdPerson = false; // really from the perspective of the camera
	m_pMinecraft->m_pGameRenderer->render(0.0f);
	m_pMinecraft->m_pMobPersp = pOldMob;
	m_pMinecraft->getOptions()->m_bDontRenderGui = bOldDontRenderGui;
	m_pMinecraft->getOptions()->m_bThirdPerson = bOldThirdPerson;

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

void LevelRenderer::addParticle(const std::string& name, const Vec3& pos, const Vec3& dir)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	float maxDist = 256.0f;
	if (name == "explodeColor")
		maxDist = 16384.0f;

	if (m_pMinecraft->m_pMobPersp->distanceToSqr_inline(pos) > maxDist)
		return;

	ParticleEngine* pe = m_pMinecraft->m_pParticleEngine;
	if (name == "bubble")
	{
		pe->add(new BubbleParticle(m_pLevel, pos, dir));
		return;
	}
	if (name == "smoke")
	{
		pe->add(new SmokeParticle(m_pLevel, pos, dir, 1.0f));
		return;
	}
	if (name == "explode")
	{
		pe->add(new ExplodeParticle(m_pLevel, pos, dir));
		return;
	}
	if (name == "explodeColor")
	{
		ExplodeParticle* pExplPart = new ExplodeParticle(m_pLevel, pos, dir);
		pExplPart->m_bIsUnlit = true;
		pExplPart->field_F8 = Mth::random();
		pExplPart->field_FC = Mth::random();
		pExplPart->field_100 = Mth::random();
		pExplPart->scale(3.0f);
		pe->add(pExplPart);
		return;
	}
	if (name == "flame")
	{
		pe->add(new FlameParticle(m_pLevel, pos, dir));
		return;
	}
	if (name == "flame2")
	{
		FlameParticle* pFlamePart = new FlameParticle(m_pLevel, pos, dir);
		pFlamePart->scale(4.0f);
		pe->add(pFlamePart);
		return;
	}
	if (name == "lava")
	{
		pe->add(new LavaParticle(m_pLevel, pos));
		return;
	}
	if (name == "largesmoke")
	{
		pe->add(new SmokeParticle(m_pLevel, pos, dir, 2.5f));
		return;
	}
	if (name == "reddust")
	{
		pe->add(new RedDustParticle(m_pLevel, pos, dir));
		return;
	}

	LOG_W("Unknown particle type: %s", name.c_str());
}

void LevelRenderer::playSound(const std::string& name, const Vec3& pos, float volume, float pitch)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	float mult = 1.0f, maxDist = 16.0f;
	float playerDist = m_pMinecraft->m_pMobPersp->distanceToSqr(pos);

	if (volume > 1.0f)
	{
		mult = 16.0f;
		maxDist = volume * mult;
	}

	if (name == "random.explode")
	{
		volume *= 1.0f - playerDist / 65536.0f;
		if (volume < 0)
			return;
		maxDist = 256.0f;
	}

	if (maxDist * maxDist > playerDist)
		m_pMinecraft->m_pSoundEngine->play(name, pos, volume, pitch);
}

void LevelRenderer::renderSky(float f)
{
	if (m_pMinecraft->m_pLevel->m_pDimension->field_C)
		return;

	glDisable(GL_TEXTURE_2D);

	Vec3 skyColor = m_pLevel->getSkyColor(m_pMinecraft->m_pMobPersp, f);
	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		skyColor.x = (((skyColor.x * 30.0f) + (skyColor.y * 59.0f)) + (skyColor.z * 11.0f)) / 100.0f;
		skyColor.y = ((skyColor.x * 30.0f) + (skyColor.y * 70.0f)) / 100.0f;
		skyColor.z = ((skyColor.x * 30.0f) + (skyColor.z * 70.0f)) / 100.0f;
	}

	glColor4f(skyColor.x, skyColor.y, Mth::Min(1.0f, skyColor.z), 1.0f);
	glDepthMask(false);
	glColor4f(skyColor.x, skyColor.y, skyColor.z, 1.0f);
	drawArrayVT(m_skyBuffer, m_skyBufferCount, sizeof(Tesselator::Vertex));
	glEnable(GL_TEXTURE_2D);
	glDepthMask(true);
}

void LevelRenderer::renderClouds(float f)
{
	if (!areCloudsAvailable())
		return;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	float yPos = Mth::Lerp(m_pMinecraft->m_pMobPersp->field_98.y, m_pMinecraft->m_pMobPersp->m_pos.y, f); // not certain if this old pos Y is used
	m_pTextures->loadAndBindTexture("environment/clouds.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec3 cloudColor = m_pLevel->getCloudColor(f);

	float offX = Mth::Lerp(m_pMinecraft->m_pMobPersp->field_3C.x, m_pMinecraft->m_pMobPersp->m_pos.x, f) + (float(m_ticksSinceStart) + f) * 0.03f;
	float offZ = Mth::Lerp(m_pMinecraft->m_pMobPersp->field_3C.z, m_pMinecraft->m_pMobPersp->m_pos.z, f);
	
	int dx2048 = Mth::floor(offX / 2048.0f);
	int dz2048 = Mth::floor(offZ / 2048.0f);

	offX -= float(dx2048 * 2048);
	offZ -= float(dz2048 * 2048);

	Tesselator& t = Tesselator::instance;

	float fYPos = ((float)C_MAX_Y - yPos) + 0.33f;
	offX /= 2048.0f;
	offZ /= 2048.0f;
	t.begin();
	t.color(cloudColor.x, cloudColor.y, cloudColor.z, 0.8f);

	const int incr = 16 * 2;
	const int maxX = C_MAX_CHUNKS_X * 16;
	const int maxZ = C_MAX_CHUNKS_Z * 16;
	for (int x = -maxX; x < maxX; x += incr)
	{
		for (int z = -maxZ; z < maxZ; z += incr)
		{
			t.vertexUV(float(x) + 0.0f, fYPos, float(z) + incr, float(x + 0.0f) / 2048.0f + offX, float(z + incr) / 2048.0f + offZ);
			t.vertexUV(float(x) + incr, fYPos, float(z) + incr, float(x + incr) / 2048.0f + offX, float(z + incr) / 2048.0f + offZ);
			t.vertexUV(float(x) + incr, fYPos, float(z) + 0.0f, float(x + incr) / 2048.0f + offX, float(z + 0.0f) / 2048.0f + offZ);
			t.vertexUV(float(x) + 0.0f, fYPos, float(z) + 0.0f, float(x + 0.0f) / 2048.0f + offX, float(z + 0.0f) / 2048.0f + offZ);
		}

	}

	t.voidBeginAndEndCalls(false); // why??
	t.draw();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
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
