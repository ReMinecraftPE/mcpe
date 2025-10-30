/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelRenderer.hpp"

#include "common/Logger.hpp"
#include "common/Mth.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/GL/GL.hpp"

#include "world/tile/LeafTile.hpp"
#include "world/tile/GrassTile.hpp"

bool LevelRenderer::_areCloudsAvailable = false; // false because 0.1 didn't have them
bool LevelRenderer::_arePlanetsAvailable = false; // false because 0.1 didn't have them

LevelRenderer::LevelRenderer(Minecraft* pMC, Textures* pTexs)
{
	field_4 = -9999.0f;
	field_8 = -9999.0f;
	field_C = -9999.0f;
	field_10 = 0.0f;
	m_noEntityRenderFrames = 2;
	m_totalEntities = 0;
	m_renderedEntities = 0;
	m_culledEntities = 0;
	field_30 = 0;
	m_totalChunks = 0;
	m_offscreenChunks = 0;
	m_occludedChunks = 0;
	m_renderedChunks = 0;
	m_emptyChunks = 0;
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

	xglGenBuffers(1, &m_starBuffer);
	generateStars();

	xglGenBuffers(1, &m_skyBuffer);
	xglGenBuffers(1, &m_darkBuffer);
	generateSky(); // inlined in the 0.1.0 demo
}

void LevelRenderer::generateSky()
{
	int s = 128;
	int d = 256 / s + 2;

	Tesselator& t = Tesselator::instance;
	t.begin();
	m_skyBufferCount = 0;
	float yy = 16.0f;

	for (int xx = -s * d; xx <= s * d; xx += s)
	{
		for (int zz = -s * d; zz <= s * d; zz += s)
		{
			t.vertex(xx + 0, yy, zz + 0);
			t.vertex(xx + s, yy, zz + 0);
			t.vertex(xx + s, yy, zz + s);
			t.vertex(xx + 0, yy, zz + s);

			m_skyBufferCount += 4;
		}
	}

	t.end(m_skyBuffer);

	// This code is almost the same, ugly

	t.begin();
	m_darkBufferCount = 0;
	yy = -16.0f;

	for (int xx = -s * d; xx <= s * d; xx += s)
	{
		for (int zz = -s * d; zz <= s * d; zz += s)
		{
			t.vertex(xx + s, yy, zz + 0);
			t.vertex(xx + 0, yy, zz + 0);
			t.vertex(xx + 0, yy, zz + s);
			t.vertex(xx + s, yy, zz + s);

			m_darkBufferCount += 4;
		}
	}

	t.end(m_darkBuffer);
}

void LevelRenderer::generateStars()
{
	Random random = Random(10842L);
	Tesselator& t = Tesselator::instance;
	t.begin();
	m_starBufferCount = 0;

	for (int i = 0; i < 1500; i++)
	{
		float x = (random.nextFloat() * 2.0f - 1.0f);
		float y = (random.nextFloat() * 2.0f - 1.0f);
		float z = (random.nextFloat() * 2.0f - 1.0f);
		float ss = (0.25f + random.nextFloat() * 0.25f);
		float d = x * x + y * y + z * z;
		if (d < 1.0f && d > 0.01f)
		{
			d = 1.0f / Mth::sqrt(d);
			x *= d;
			y *= d;
			z *= d;
			float xp = x * 100.0f;
			float yp = y * 100.0f;
			float zp = z * 100.0f;
			float yRot = Mth::atan2(x, z);
			float ySin = Mth::sin(yRot);
			float yCos = Mth::cos(yRot);
			float xRot = Mth::atan2(Mth::sqrt(x * x + z * z), y);
			float xSin = Mth::sin(xRot);
			float xCos = Mth::cos(xRot);
			float zRot = random.nextFloat() * M_PI * 2.0f;
			float zSin = Mth::sin(zRot);
			float zCos = Mth::cos(zRot);

			for (int c = 0; c < 4; c++)
			{
				float ___xo = 0.0f;
				float ___yo = ((c & 2) - 1) * ss;
				float ___zo = ((c + 1 & 2) - 1) * ss;
				float __yo = ___yo * zCos - ___zo * zSin;
				float __zo = ___zo * zCos + ___yo * zSin;
				float _yo = __yo * xSin + ___xo * xCos;
				float _xo = ___xo * xSin - __yo * xCos;
				float xo = _xo * ySin - __zo * yCos;
				float zo = __zo * ySin + _xo * yCos;
				t.vertex(xp + xo, yp + _yo, zp + zo);
			}
			m_starBufferCount += 4;
		}
	}

	t.end(m_starBuffer);
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

	m_noEntityRenderFrames = 2;
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
	ss  << "C: " << m_renderedChunks << "/" << m_totalChunks // Number of chunk sections rendered over total number of chunks.
		<< ". F: " << m_offscreenChunks // Number of chunk sections loaded outside the viewing distance.
		<< ", O: " << m_occludedChunks // Number of occluded chunk sections.
		<< ", E: " << m_emptyChunks // Number of empty chunk sections.
		<< "\n";

	return ss.str();
}

std::string LevelRenderer::gatherStats2()
{
	std::stringstream ss;
	ss  << "E: " << m_renderedEntities << "/" << m_totalEntities
		<< ". B: " << m_culledEntities
		<< ", I: " << m_totalEntities - m_culledEntities - m_renderedEntities
		<< "\n";

	return ss.str();
}

void LevelRenderer::onGraphicsReset()
{
	xglGenBuffers(m_nBuffers, m_pBuffers);
	allChanged();

	xglGenBuffers(1, &m_starBuffer);
	generateStars();

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
			m_totalChunks++;
			if (pChunk->field_1C[0])
			{
				m_emptyChunks++;
			}
			else if (pChunk->m_bVisible)
			{
				if (!field_B8 || pChunk->field_4D)
					m_renderedChunks++;
				else
					m_occludedChunks++;
			}
			else
			{
				m_offscreenChunks++;
			}
		}

		if (!pChunk->field_1C[a] && pChunk->m_bVisible && pChunk->field_4D && pChunk->getList(a) >= 0)
		{
			result++;
			field_24.push_back(pChunk);
		}
	}

	Mob* pMob = m_pMinecraft->m_pMobPersp;

	float fPosX = pMob->m_posPrev.x + (pMob->m_pos.x - pMob->m_posPrev.x) * b;
	float fPosY = pMob->m_posPrev.y + (pMob->m_pos.y - pMob->m_posPrev.y) * b;
	float fPosZ = pMob->m_posPrev.z + (pMob->m_pos.z - pMob->m_posPrev.z) * b;

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
		m_totalChunks = m_offscreenChunks = m_occludedChunks = m_renderedChunks = m_emptyChunks = 0;

	Vec3 mobPos = pMob->m_posPrev + (pMob->m_pos - pMob->m_posPrev) * b;

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

		float px = pPlayer->m_posPrev.x + (pPlayer->m_pos.x - pPlayer->m_posPrev.x) * f;
		float py = pPlayer->m_posPrev.y + (pPlayer->m_pos.y - pPlayer->m_posPrev.y) * f;
		float pz = pPlayer->m_posPrev.z + (pPlayer->m_pos.z - pPlayer->m_posPrev.z) * f;

		Tesselator& t = Tesselator::instance;
		glEnable(GL_ALPHA_TEST); // Fixes for b1.7.3 terrain
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

	float px = pPlayer->m_posPrev.x + (pPlayer->m_pos.x - pPlayer->m_posPrev.x) * f;
	float py = pPlayer->m_posPrev.y + (pPlayer->m_pos.y - pPlayer->m_posPrev.y) * f;
	float pz = pPlayer->m_posPrev.z + (pPlayer->m_pos.z - pPlayer->m_posPrev.z) * f;

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

	float lineWidth = 2.0f * Minecraft::getRenderScaleMultiplier();
	if (lineWidth > range[1])
		lineWidth = range[1];

	glLineWidth(lineWidth);

	TileID tile = m_pLevel->getTile(hr.m_tilePos);
	if (tile > 0)
	{
		Tile::tiles[tile]->updateShape(
			m_pLevel,
			hr.m_tilePos);
		float posX = pPlayer->m_posPrev.x + ((pPlayer->m_pos.x - pPlayer->m_posPrev.x) * f);
		float posY = pPlayer->m_posPrev.y + ((pPlayer->m_pos.y - pPlayer->m_posPrev.y) * f);
		float posZ = pPlayer->m_posPrev.z + ((pPlayer->m_pos.z - pPlayer->m_posPrev.z) * f);
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
	if (m_noEntityRenderFrames > 0)
	{
		m_noEntityRenderFrames--;
		return;
	}

	Mob* mob = m_pMinecraft->m_pMobPersp;

	EntityRenderDispatcher::getInstance()->prepare(m_pLevel, m_pMinecraft->m_pTextures, m_pMinecraft->m_pFont, mob, m_pMinecraft->getOptions(), f);

	m_totalEntities = 0;
	m_renderedEntities = 0;
	m_culledEntities = 0;

	EntityRenderDispatcher::off = mob->m_posPrev + (mob->m_pos - mob->m_posPrev) * f;

	const EntityVector* pVec = m_pLevel->getAllEntities();
	m_totalEntities = int(pVec->size());

	for (int i = 0; i < m_totalEntities; i++)
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
			m_renderedEntities++;
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
		pExplPart->m_rCol = Mth::random();
		pExplPart->m_gCol = Mth::random();
		pExplPart->m_bCol = Mth::random();
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
		if (volume < 0.0f)
			return;
		maxDist = 256.0f;
	}

	if (maxDist * maxDist > playerDist)
		m_pMinecraft->m_pSoundEngine->play(name, pos, volume, pitch);
}

void LevelRenderer::renderSky(float alpha)
{
	if (m_pMinecraft->m_pLevel->m_pDimension->m_bFoggy)
		return;

	glDisable(GL_TEXTURE_2D);

	Vec3 sc = m_pLevel->getSkyColor(m_pMinecraft->m_pMobPersp, alpha);
	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		sc.x = (((sc.x * 30.0f) + (sc.y * 59.0f)) + (sc.z * 11.0f)) / 100.0f;
		sc.y = ((sc.x * 30.0f) + (sc.y * 70.0f)) / 100.0f;
		sc.z = ((sc.x * 30.0f) + (sc.z * 70.0f)) / 100.0f;
	}

	glColor4f(sc.x, sc.y, Mth::Min(1.0f, sc.z), 1.0f);

	Tesselator& t = Tesselator::instance;

	glDepthMask(false);
	glEnable(GL_FOG);
	glColor4f(sc.x, sc.y, sc.z, 1.0f);

	drawArrayVT(m_skyBuffer, m_skyBufferCount, sizeof(Tesselator::Vertex));

	glDisable(GL_FOG);
	glDisable(GL_ALPHA_TEST);

	// Sunrise
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float* c = m_pLevel->m_pDimension->getSunriseColor(m_pLevel->getTimeOfDay(alpha), alpha);
	if (c != nullptr && arePlanetsAvailable())
	{
		glDisable(GL_TEXTURE_2D);
		//glShadeModel(GL_SMOOTH); // I'd rather fuck up the sunrise gradient than AO, but it doesn't even look like it does that

		glPushMatrix();
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(m_pLevel->getTimeOfDay(alpha) > 0.5f ? 180 : 0, 0.0f, 0.0f, 1.0f);

		t.begin(GL_TRIANGLE_FAN);
		t.color(c[0], c[1], c[2], c[3]);
		t.vertex(0.0f, 100.0f, 0.0f);
		t.color(c[0], c[1], c[2], 0.0f);

		int steps = 16;
		for (int i = 0; i <= steps; i++)
		{
			float a = i * 3.1415927f * 2.0f / steps;
			float sin = Mth::sin(a);
			float cos = Mth::cos(a);
			t.vertex((sin * 120.0f), (cos * 120.0f), (-cos * 40.0f * c[3]));
		}

		t.draw();
		glPopMatrix();
		//glShadeModel(GL_FLAT); // fuck it, don't bother, doing this SOMEHOW breaks AO
	}

	// Sun, moon, stars
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_ONE, GL_ONE);
	glPushMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslatef(sc.x, sc.y, sc.z);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(m_pLevel->getTimeOfDay(alpha) * 360.0f, 1.0f, 0.0f, 0.0f);

	float ss = 30.0f;

	if (arePlanetsAvailable())
	{
		m_pTextures->loadAndBindTexture("terrain/sun.png");
		t.begin();
		t.vertexUV(-ss, 100.0f, -ss, 0.0f, 0.0f);
		t.vertexUV(ss, 100.0f, -ss, 1.0f, 0.0f);
		t.vertexUV(ss, 100.0f, ss, 1.0f, 1.0f);
		t.vertexUV(-ss, 100.0f, ss, 0.0f, 1.0f);
		t.draw();

		ss = 20.0f;
		m_pTextures->loadAndBindTexture("terrain/moon.png");
		t.begin();
		t.vertexUV(-ss, -100.0f, ss, 1.0f, 1.0f);
		t.vertexUV(ss, -100.0f, ss, 0.0f, 1.0f);
		t.vertexUV(ss, -100.0f, -ss, 0.0f, 0.0f);
		t.vertexUV(-ss, -100.0f, -ss, 1.0f, 0.0f);
		t.draw();
	}

	glDisable(GL_TEXTURE_2D);

	float a = m_pLevel->getStarBrightness(alpha);
	if (a > 0.0f)
	{
		glColor4f(a, a, a, a);
		drawArrayVT(m_starBuffer, m_starBufferCount, sizeof(Tesselator::Vertex));
	}

	// Dark plane
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_FOG);
	glPopMatrix();

	glColor4f(sc.x * 0.2f + 0.04f, sc.y * 0.2f + 0.04f, sc.z * 0.6f + 0.1f, 1.0f);
	glDisable(GL_TEXTURE_2D);
	drawArrayVT(m_darkBuffer, m_darkBufferCount, sizeof(Tesselator::Vertex));
	glEnable(GL_TEXTURE_2D);

	glDepthMask(true);
}

void LevelRenderer::renderClouds(float alpha)
{
	if (!areCloudsAvailable())
		return;

	if (m_pMinecraft->getOptions()->m_bFancyGraphics)
	{
		renderAdvancedClouds(alpha);
		return;
	}

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	float yPos = Mth::Lerp(m_pMinecraft->m_pMobPersp->m_posPrev.y, m_pMinecraft->m_pMobPersp->m_pos.y, alpha); // not certain if this old pos Y is used
	m_pTextures->loadAndBindTexture("environment/clouds.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec3 cloudColor = m_pLevel->getCloudColor(alpha);

	float offX = Mth::Lerp(m_pMinecraft->m_pMobPersp->m_oPos.x, m_pMinecraft->m_pMobPersp->m_pos.x, alpha) + (float(m_ticksSinceStart) + alpha) * 0.03f;
	float offZ = Mth::Lerp(m_pMinecraft->m_pMobPersp->m_oPos.z, m_pMinecraft->m_pMobPersp->m_pos.z, alpha);
	
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

	


	float yy = ((float)C_MAX_Y - yPos) + 0.33f; // 108.0f on b1.2_02, see below

	if (yy > 1.0f) {
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void LevelRenderer::renderAdvancedClouds(float alpha)
{
	glDisable(GL_CULL_FACE);

	float yOffs = //Mth::Lerp(m_pMinecraft->m_pMobPersp->m_posPrev.y, m_pMinecraft->m_pMobPersp->m_pos.y, alpha);
    m_pMinecraft->m_pMobPersp->m_posPrev.y + (m_pMinecraft->m_pMobPersp->m_pos.y - m_pMinecraft->m_pMobPersp->m_posPrev.y) * alpha;

	Tesselator& t = Tesselator::instance;
	constexpr float ss = 12.0f;
	constexpr float h = 4.0f;

	// @NOTE: Using Mth::Lerp will use incorrect logic
	float xo = (m_pMinecraft->m_pMobPersp->m_oPos.x + (m_pMinecraft->m_pMobPersp->m_pos.x - m_pMinecraft->m_pMobPersp->m_oPos.x) * alpha + ((float(m_ticksSinceStart) + alpha) * 0.03f)) / ss;
	float zo = (m_pMinecraft->m_pMobPersp->m_oPos.z + (m_pMinecraft->m_pMobPersp->m_pos.z - m_pMinecraft->m_pMobPersp->m_oPos.z) * alpha) / ss + 0.33f;

	float yy = ((float)C_MAX_Y - yOffs) + 0.33f; // 108.0f on b1.2_02, see below
    //float yy = 108.0f - yOffs + 0.33F;

	int xOffs = Mth::floor(xo / 2048);
	int zOffs = Mth::floor(zo / 2048);

	xo -= xOffs * 2048;
	zo -= zOffs * 2048;

	m_pTextures->loadAndBindTexture("environment/clouds.png");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec3 cc = m_pLevel->getCloudColor(alpha);
	float cr = cc.x;
	float cg = cc.y;
	float cb = cc.z;
    float uo;
    float vo;
    float scale;
    
	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
        uo = (cr * 30.0f + cg * 59.0f + cb * 11.0f) / 100.0f;
        vo = (cr * 30.0f + cg * 70.0f) / 100.0f;
        scale = (cr * 30.0f + cb * 70.0f) / 100.0f;
        cr = uo;
        cg = vo;
        cb = scale;
	}
    
    uo = (float)(xo * 0.0);
    vo = (float)(zo * 0.0);

	//uo = xo * 0.0f;
	//vo = zo * 0.0f;
	scale = 1.0f / 256.0f;

	uo = Mth::floor(xo) * scale;
	vo = Mth::floor(zo) * scale;

	float xoffs = xo - Mth::floor(xo);
	float zoffs = zo - Mth::floor(zo);

	constexpr int D = 8;
	constexpr int radius = 3;
	constexpr float e = 1.0f / 1024.0f;

	glScalef(ss, 1.0f, ss);

	for (int pass = 0; pass < 2; pass++)
	{
		if (pass == 0)
			glColorMask(false, false, false, false);
		else
			glColorMask(true, true, true, true);

		for (int xPos = -radius + 1; xPos <= radius; xPos++)
		{
			for (int zPos = -radius + 1; zPos <= radius; zPos++)
			{
				t.begin();
				float xx = xPos * D;
				float zz = zPos * D;
				float xp = xx - xoffs;
				float zp = zz - zoffs;

				if (yy > -h - 1.0f)
				{
					t.color(cr * 0.7f, cg * 0.7f, cb * 0.7f, 0.8f);
					t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + D), ((xx + 0.0f) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + 0.0f), (zp + D), ((xx + D) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + 0.0f), (zp + 0.0f), ((xx + D) * scale + uo), ((zz + 0.0f) * scale + vo));
					t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + 0.0f), ((xx + 0.0f) * scale + uo), ((zz + 0.0f) * scale + vo));
				}

				if (yy <= h + 1.0f)
				{
					t.color(cr, cg, cb, 0.8f);
					t.normal(0.0f, 1.0f, 0.0f);
					t.vertexUV((xp + 0.0f), (yy + h - e), (zp + D), ((xx + 0.0f) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + h - e), (zp + D), ((xx + D) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + h - e), (zp + 0.0f), ((xx + D) * scale + uo), ((zz + 0.0f) * scale + vo));
					t.vertexUV((xp + 0.0f), (yy + h - e), (zp + 0.0f), ((xx + 0.0f) * scale + uo), ((zz + 0.0f) * scale + vo));
				}

				t.color(cr * 0.9f, cg * 0.9f, cb * 0.9f, 0.8f);

				if (xPos > -1)
				{
					t.normal(-1.0f, 0.0f, 0.0f);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + i + 0.0f), (yy + 0.0f), (zp + D), ((xx + i + 0.5f) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 0.0f), (yy + h), (zp + D), ((xx + i + 0.5f) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 0.0f), (yy + h), (zp + 0.0f), ((xx + i + 0.5f) * scale + uo), ((zz + 0.0f) * scale + vo));
						t.vertexUV((xp + i + 0.0f), (yy + 0.0f), (zp + 0.0f), ((xx + i + 0.5f) * scale + uo), ((zz + 0.0f) * scale + vo));
					}
				}

				if (xPos <= 1)
				{
					t.normal(1.0f, 0.0f, 0.0f);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + i + 1.0f - e), (yy + 0.0f), (zp + D), ((xx + i + 0.5f) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 1.0f - e), (yy + h), (zp + D), ((xx + i + 0.5f) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 1.0f - e), (yy + h), (zp + 0.0f), ((xx + i + 0.5f) * scale + uo), ((zz + 0.0f) * scale + vo));
						t.vertexUV((xp + i + 1.0f - e), (yy + 0.0f), (zp + 0.0f), ((xx + i + 0.5f) * scale + uo), ((zz + 0.0f) * scale + vo));
					}
				}

				t.color(cr * 0.8f, cg * 0.8f, cb * 0.8f, 0.8f);
				if (zPos > -1)
				{
					t.normal(0.0f, 0.0f, -1.0f);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + 0.0f), (yy + h), (zp + i + 0.0f), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + h), (zp + i + 0.0f), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + 0.0f), (zp + i + 0.0f), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + i + 0.0f), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
					}
				}

				if (zPos <= 1)
				{
					t.normal(0.0f, 0.0f, 1.0f);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + 0.0f), (yy + h), (zp + i + 1.0f - e), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + h), (zp + i + 1.0f - e), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + 0.0f), (zp + i + 1.0f - e), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + i + 1.0f - e), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
					}
				}

				t.draw();
			}
		}
	}

	if (yy > 1.0f) {
		glDepthRange(0.f, 7.f);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

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

void LevelRenderer::levelEvent(Player* pPlayer, LevelEvent::ID eventId, const TilePos& pos, LevelEvent::Data data)
{
	switch (eventId)
	{
	case LevelEvent::SOUND_DOOR:
		std::string snd;
		if (Mth::random() < 0.5f)
			snd = "random.door_open";
		else
			snd = "random.door_close";

		m_pLevel->playSound(Vec3(pos) + 0.5f, snd, 1.0f, 0.9f + 0.1f * m_pLevel->m_random.nextFloat());
		break;
	}
}
