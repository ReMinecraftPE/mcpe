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
	x -= 8;
	y -= 8;
	z -= 8;
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
		int x2 = x1 + blkCntHalf - x;
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
			int z2 = z1 + blkCntHalf - z;
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
				pChunk->setPos(x1, y1, z1);

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

	float mobX2 = pMob->field_98.x + (pMob->m_pos.x - pMob->field_98.x) * b;
	float mobY2 = pMob->field_98.y + (pMob->m_pos.y - pMob->field_98.y) * b;
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
				fXdiff = float(m_chunks[i]->m_pos.x) - mobX2 - lastX;
				fYdiff = float(m_chunks[i]->m_pos.y) - mobY2 - lastY;
				fZdiff = float(m_chunks[i]->m_pos.z) - mobZ2 - lastZ;

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
	m_ticksSinceStart++;
}

typedef std::vector<Chunk*> ChunkVector;
typedef ChunkVector::iterator ChunkVectorIterator;

bool LevelRenderer::updateDirtyChunks(Mob* pMob, bool b)
{
	constexpr int C_MAX = 3;
	DirtyChunkSorter dcs(pMob);
	Chunk* pChunks[C_MAX] = { nullptr };
	ChunkVector* pVec = nullptr;

	int nr1 = 0;
	int sz = int(field_88.size());
	for (int i = 0; i < sz; i++)
	{
		Chunk* pChunk = field_88[i];
		if (!b)
		{
			if (pChunk->distanceToSqr(pMob) > 1024.0f)
			{
				int j;
				// find place to insert this chunk within the pChunks array
				for (j = 0; j < C_MAX; j++)
				{
					if (pChunks[j] && !dcs(pChunks[j], pChunk))
						break;
				}
				// insert it
				if (--j <= 0)
					continue;
				
				for (int k = j; --k != 0;) {
					pChunks[k - 1] = pChunks[k];
				}

				pChunks[j] = pChunk;
				continue;
			}
		}
		else if (!pChunk->m_bVisible)
		{
			continue;
		}

		if (!pVec)
			pVec = new ChunkVector;

		nr1++;
		pVec->push_back(pChunk);
		field_88[i] = nullptr;
	}

	if (pVec)
	{
		if (pVec->size() > 1)
			std::sort(pVec->begin(), pVec->end(), dcs);

		for (int i = int(pVec->size()) - 1; i >= 0; i--)
		{
			(*pVec)[i]->rebuild();
			(*pVec)[i]->setClean();
		}

		SAFE_DELETE(pVec);
	}

	int nr2 = 0;
	for (int m = C_MAX - 1; m >= 0; m--)
	{
		if (!pChunks[m])
			continue;

		if (!pChunks[m]->m_bVisible && m != C_MAX - 1)
		{
			pChunks[m] = nullptr;
			pChunks[0] = nullptr;
			break;
		}

		pChunks[m]->rebuild();
		pChunks[m]->setClean();
		nr2++;
	}

	int nr3 = 0;
	int nr4 = 0;
	for (; nr4 < int(field_88.size()); nr4++)
	{
		Chunk* pChunk = field_88[nr4];
		if (!pChunk)
			continue;

		bool flag = false;
		for (int j = 0; j < C_MAX; j++)
		{
			if (pChunk == pChunks[j])
				flag = true;
		}

		if (flag)
			continue;

		if (nr3 != nr4)
			field_88[nr3] = pChunk;

		nr3++;
	}

	if (nr4 > nr3)
		field_88.erase(field_88.begin() + nr3, field_88.end());

	return nr1 + nr2 == sz;
}

void LevelRenderer::renderHit(Player* pPlayer, const HitResult& hr, int i, void* vp, float f)
{
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// @BUG: possible leftover from Minecraft Classic? This is overridden anyways
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f * (0.4f + 0.2f * Mth::sin(float(getTimeMs()) / 100.0f)));

	if (!i && field_10 > 0.0f)
	{
		glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

		m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glPushMatrix();
		Tile* pTile = nullptr;
		TileID tile = m_pLevel->getTile(hr.m_tileX, hr.m_tileY, hr.m_tileZ);
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

		m_pTileRenderer->tesselateInWorld(pTile, hr.m_tileX, hr.m_tileY, hr.m_tileZ, 240 + int(field_10 * 10.0f));

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

	TileID tile = m_pLevel->getTile(hr.m_tileX, hr.m_tileY, hr.m_tileZ);
	if (tile > 0)
	{
		Tile::tiles[tile]->updateShape(
			m_pLevel,
			hr.m_tileX,
			hr.m_tileY,
			hr.m_tileZ);
		float posX = pPlayer->field_98.x + ((pPlayer->m_pos.x - pPlayer->field_98.x) * f);
		float posY = pPlayer->field_98.y + ((pPlayer->m_pos.y - pPlayer->field_98.y) * f);
		float posZ = pPlayer->field_98.z + ((pPlayer->m_pos.z - pPlayer->field_98.z) * f);
		AABB aabb, tileAABB = Tile::tiles[tile]->getTileAABB(m_pLevel, hr.m_tileX, hr.m_tileY, hr.m_tileZ);
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

	EntityRenderDispatcher::getInstance()->prepare(m_pLevel, m_pMinecraft->m_pTextures, m_pMinecraft->m_pFont, mob, m_pMinecraft->getOptions(), f);

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

		if (m_pMinecraft->m_pMobPersp == pEnt && !m_pMinecraft->getOptions()->m_bThirdPerson)
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

void LevelRenderer::addParticle(const std::string& name, float x, float y, float z, float vx, float vy, float vz)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	float maxDist = 256.0f;
	if (name == "explodeColor")
		maxDist = 16384.0f;

	if (m_pMinecraft->m_pMobPersp->distanceToSqr_inline(x, y, z) > maxDist)
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
	if (name == "explodeColor")
	{
		ExplodeParticle* pExplPart = new ExplodeParticle(m_pLevel, x, y, z, vx, vy, vz);
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
		pe->add(new FlameParticle(m_pLevel, x, y, z, vx, vy, vz));
		return;
	}
	if (name == "flame2")
	{
		FlameParticle* pFlamePart = new FlameParticle(m_pLevel, x, y, z, vx, vy, vz);
		pFlamePart->scale(4.0f);
		pe->add(pFlamePart);
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

	LOG_W("Unknown particle type: %s", name.c_str());
}

void LevelRenderer::playSound(const std::string& name, float x, float y, float z, float volume, float pitch)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	float mult = 1.0f, maxDist = 16.0f;
	float playerDist = m_pMinecraft->m_pMobPersp->distanceToSqr(x, y, z);

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
		m_pMinecraft->m_pSoundEngine->play(name, x, y, z, volume, pitch);
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

// TODO: This should be inside of an initialized "Minecraft" instance rather than the global namespace
bool g_bAreCloudsAvailable = false; // false because 0.1 didn't have them

void LevelRenderer::renderClouds(float f)
{
	if (!g_bAreCloudsAvailable)
		return;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	float yPos = Mth::Lerp(m_pMinecraft->m_pMobPersp->field_98.y, m_pMinecraft->m_pMobPersp->m_pos.y, f); // not certain if this old pos Y is used
	m_pTextures->loadAndBindTexture("environment/clouds.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec3 cloudColor = m_pLevel->getCloudColor(f);

	float offX = Mth::Lerp(m_pMinecraft->m_pMobPersp->field_3C.x, m_pMinecraft->m_pMobPersp->m_pos.x, f) + (float(m_ticksSinceStart) + f) * 0.3f;
	float offZ = Mth::Lerp(m_pMinecraft->m_pMobPersp->field_3C.z, m_pMinecraft->m_pMobPersp->m_pos.z, f);
	
	int dx2048 = Mth::floor(offX / 2048.0f);
	int dz2048 = Mth::floor(offZ / 2048.0f);

	offX -= float(dx2048 * 2048);
	offZ -= float(dz2048 * 2048);

	Tesselator& t = Tesselator::instance;

	float fYPos = (128.0f - yPos) + 0.33f;
	offX /= 2048.0f;
	offZ /= 2048.0f;
	t.begin();
	t.color(cloudColor.x, cloudColor.y, cloudColor.z, 0.8f);

	const int incr = 32;
	const int in2 = 256 / incr;
	for (int x = -incr * in2; x < incr * in2; x += incr)
	{
		for (int z = -incr * in2; z < incr * in2; z += incr)
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
