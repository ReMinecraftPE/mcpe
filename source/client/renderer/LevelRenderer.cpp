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
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/GlobalConstantBuffers.hpp"
#include "renderer/ShaderConstants.hpp"
#include "renderer/RenderContextImmediate.hpp"

#if MCE_GFX_API_OGL
#include "thirdparty/GL/GL.hpp"
#endif

#include "world/tile/LeafTile.hpp"
#include "world/tile/GrassTile.hpp"

#include "Fog.hpp"
#include "Lighting.hpp"

TerrainLayer renderLayerToTerrainLayerMap[Tile::RENDER_LAYERS_COUNT] = {
	/*RENDER_LAYER_DOUBLE_SIDED*/               TERRAIN_LAYER_DOUBLE_SIDED,
	/*RENDER_LAYER_BLEND*/                      TERRAIN_LAYER_BLEND,
	/*RENDER_LAYER_OPAQUE*/                     TERRAIN_LAYER_OPAQUE,
	/*RENDER_LAYER_OPTIONAL_ALPHATEST*/         TERRAIN_LAYER_ALPHATEST_SINGLE_SIDE, // Shouldn't be single-sided, but this
	/*RENDER_LAYER_ALPHATEST*/                  TERRAIN_LAYER_ALPHATEST_SINGLE_SIDE, // breaks leaves and glass blocks.
	/*RENDER_LAYER_SEASONS_OPAQUE*/             TERRAIN_LAYER_OPAQUE_SEASONS,
	/*RENDER_LAYER_SEASONS_OPTIONAL_ALPHATEST*/ TERRAIN_LAYER_ALPHATEST_SEASONS
};

LevelRenderer::Materials::Materials()
{
	MATERIAL_PTR(common, shadow_back);
	MATERIAL_PTR(common, shadow_front);
	MATERIAL_PTR(common, shadow_overlay);
	MATERIAL_PTR(common, shadow_image_overlay);
	MATERIAL_PTR(common, stars);
	MATERIAL_PTR(common, skyplane);
	MATERIAL_PTR(common, sun_moon);
	MATERIAL_PTR(common, sunrise);
	MATERIAL_PTR(common, selection_overlay);
	MATERIAL_PTR(common, selection_overlay_opaque);
	MATERIAL_PTR(common, selection_overlay_double_sided);
	MATERIAL_PTR(common, selection_box);
	MATERIAL_PTR(common, cracks_overlay);
	MATERIAL_PTR(common, cracks_overlay_tile_entity);
	MATERIAL_PTR(switchable, clouds);
}

bool LevelRenderer::_areCloudsAvailable = false; // false because 0.1 didn't have them
bool LevelRenderer::_arePlanetsAvailable = false; // false because 0.1 didn't have them

LevelRenderer::LevelRenderer(Minecraft* pMC, Textures* pTexs)
{
	m_posPrev = -9999.0f;
	m_destroyProgress = 0.0f;
	m_noEntityRenderFrames = 2;
	m_totalEntities = 0;
	m_renderedEntities = 0;
	m_culledEntities = 0;
	m_cullStep = 0;
	m_totalChunks = 0;
	m_offscreenChunks = 0;
	m_occludedChunks = 0;
	m_renderedChunks = 0;
	m_emptyChunks = 0;
	field_68 = 0;
	m_xMinChunk = 0;
	m_yMinChunk = 0;
	m_zMinChunk = 0;
	m_xMaxChunk = 0;
	m_yMaxChunk = 0;
	m_zMaxChunk = 0;
	m_pLevel = nullptr;
	m_pDimension = nullptr;
	m_chunks = nullptr;
	m_sortedChunks = nullptr;
	m_chunksLength = 0;
	m_pTileRenderer = nullptr;
	m_xChunks = 0;
	m_yChunks = 0;
	m_zChunks = 0;
	m_chunkLists = 0;
	m_bOcclusionCheck = false; // getOpenGLCapabilities().hasOcclusionChecks()
	m_lastViewDistance = -1;
	m_ticksSinceStart = 0;
	m_fogBrO = 0.0f;
	m_fogBr = 0.0f;

	m_pMinecraft = pMC;
	m_pTextures = pTexs;

	_initResources();
	RenderChunk::InitMaterials();
	m_initTime = getTimeS();
}

void LevelRenderer::_buildSkyMesh()
{
	Tesselator& t = Tesselator::instance;

#if 0

	// 12-vertex MCPE sky

	constexpr float angleStep = 6.2832f * 0.1f;
	constexpr float currentSin = 0.0f;
	constexpr float currentCos = 1.0f;
	constexpr float radius = 2000.0f;
	constexpr float yy = 128.0f;

	t.begin(mce::PRIMITIVE_MODE_TRIANGLE_LIST, 12);

	t.color(Color::BLACK);
	t.vertex(0.0f, yy, 0.0f);

	for (int step = 0; step <= 10; step++)
	{
		float angle = -((float)step * angleStep);

		currentSin = sin(angle);
		currentCos = cos(angle);

		t.color(Color::WHITE);
		t.vertex(currentCos * radius, yy, -(radius * currentSin));
	}

	// this shit is fucked
	t.beginIndices(0);
	for (int i = 0; i <= 10; i++)
		t.triangle(0, i, i % 10 + 1);

	m_skyMesh = t.end();

#elif defined(FEATURE_GFX_SHADERS)

	constexpr float angleStep = 6.2832f * 0.1f;
	constexpr float yy = 128.0f;

	// calculates the radius of the sky based on the fogStart
	// PE/Bedrock do not have this
	float fogStart = Fog::nextState.fogStartZ;
	float radius = fogStart * 26.375f; // magic number determined to match Java thru eyeballing

	Color topColor = Color::BLACK;
	Color bottomColor = Color::WHITE;

	t.begin(mce::PRIMITIVE_MODE_TRIANGLE_LIST, 30);

	for (int step = 0; step < 10; step++)
	{
		float angle = -((float)step * angleStep);
		float nextAngle = -((float)(step + 1) * angleStep);

		t.color(topColor);
		t.vertex(0.0f, yy, 0.0f);

		t.color(bottomColor);
		t.vertex(cos(angle) * radius, yy, -(radius * sin(angle)));

		t.color(bottomColor);
		t.vertex(cos(nextAngle) * radius, yy, -(radius * sin(nextAngle)));
	}

	m_skyMesh = t.end();

#else

	constexpr int s = 128;
	constexpr int d = 256 / s + 2;

	t.begin(324);
	float yy = 16.0f;

	for (int xx = -s * d; xx <= s * d; xx += s)
	{
		for (int zz = -s * d; zz <= s * d; zz += s)
		{
			t.vertex(xx + 0, yy, zz + 0);
			t.vertex(xx + s, yy, zz + 0);
			t.vertex(xx + s, yy, zz + s);
			t.vertex(xx + 0, yy, zz + s);
		}
	}

	m_skyMesh = t.end();

	// This code is almost the same, ugly

	t.begin(324);
	yy = -16.0f;

	for (int xx = -s * d; xx <= s * d; xx += s)
	{
		for (int zz = -s * d; zz <= s * d; zz += s)
		{
			t.vertex(xx + s, yy, zz + 0);
			t.vertex(xx + 0, yy, zz + 0);
			t.vertex(xx + 0, yy, zz + s);
			t.vertex(xx + s, yy, zz + s);
		}
	}

	m_darkMesh = t.end();

#endif
}

void LevelRenderer::_buildStarsMesh()
{
	Random random = Random(10842L); // is there any good reason why this specifically is a long?
	Tesselator& t = Tesselator::instance;
	t.begin(3160);

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
		}
	}

	m_starsMesh = t.end();
}

void LevelRenderer::_buildSunAndMoonMeshes()
{

}

void LevelRenderer::_buildShadowVolume()
{

}

void LevelRenderer::_buildShadowOverlay()
{

}

void LevelRenderer::_initResources()
{
	_buildSkyMesh();
	_buildStarsMesh();
	_buildSunAndMoonMeshes();
	_buildShadowVolume();
	_buildShadowOverlay();
}

void LevelRenderer::_renderSunrise(float alpha)
{
	Tesselator& t = Tesselator::instance;

	const float* c = m_pLevel->m_pDimension->getSunriseColor(m_pLevel->getTimeOfDay(alpha), alpha);
	if (c != nullptr && arePlanetsAvailable())
	{
		MatrixStack::Ref matrix = MatrixStack::World.push();
		matrix->rotate(90.0f, Vec3::UNIT_X);
		matrix->rotate(m_pLevel->getTimeOfDay(alpha) > 0.5f ? 180 : 0, Vec3::UNIT_Z);

		int steps = 16;

		t.begin(mce::PRIMITIVE_MODE_TRIANGLE_STRIP, steps * 2);

		for (int i = 0; i <= steps; i++)
		{
			float a = i * 3.1415927f * 2.0f / steps;
			float sin = Mth::sin(a);
			float cos = Mth::cos(a);

			t.color(c[0], c[1], c[2], c[3]);
			t.vertex(0.0f, 100.0f, 0.0f);

			t.color(c[0], c[1], c[2], 0.0f);
			t.vertex((sin * 120.0f), (cos * 120.0f), (-cos * 40.0f * c[3]));
		}

		// @HAL: sky.vertex shader will not work here, it ignores the vertex colors
		t.draw(m_materials.sunrise);
	}
}

void LevelRenderer::_renderSolarSystem(float alpha)
{
	MatrixStack::Ref planetMtx = MatrixStack::World.push();

	_renderSunAndMoon(alpha);
	_renderStars(alpha);
}

void LevelRenderer::_renderSunAndMoon(float alpha)
{
	Tesselator& t = Tesselator::instance;

	Matrix& matrix = MatrixStack::World.getTop();

	Vec3 p = Vec3::ZERO;

	currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color::WHITE;

	matrix.translate(p);
	matrix.rotate(0.0f, Vec3::UNIT_Z);
	matrix.rotate(m_pLevel->getTimeOfDay(alpha) * 360.0f, Vec3::UNIT_X);

	if (arePlanetsAvailable())
	{
		float ss = 30.0f;
		m_pTextures->loadAndBindTexture("terrain/sun.png");
		t.begin(4);
		t.vertexUV(-ss, 100.0f, -ss, 0.0f, 0.0f);
		t.vertexUV(ss, 100.0f, -ss, 1.0f, 0.0f);
		t.vertexUV(ss, 100.0f, ss, 1.0f, 1.0f);
		t.vertexUV(-ss, 100.0f, ss, 0.0f, 1.0f);
		t.draw(m_materials.sun_moon);

		ss = 20.0f;
		m_pTextures->loadAndBindTexture("terrain/moon.png");
		t.begin(4);
		t.vertexUV(-ss, -100.0f, ss, 1.0f, 1.0f);
		t.vertexUV(ss, -100.0f, ss, 0.0f, 1.0f);
		t.vertexUV(ss, -100.0f, -ss, 0.0f, 0.0f);
		t.vertexUV(-ss, -100.0f, -ss, 1.0f, 0.0f);
		t.draw(m_materials.sun_moon);
	}
}

void LevelRenderer::_renderStars(float alpha)
{
	float a = m_pLevel->getStarBrightness(alpha);
	if (a > 0.0f)
	{
		currentShaderColor = Color(a, a, a);
		m_starsMesh.render(m_materials.stars);
	}
}

void LevelRenderer::_renderTileShadow(Tile* tt, const Vec3& pos, TilePos& tilePos, float pow, float r, const Vec3& oPos)
{
	Tesselator& t = Tesselator::instance;
	if (!tt->isCubeShaped()) return;

	float a = (pow - (pos.y - ((float)tilePos.y + oPos.y)) / 2.0f) * 0.5f * m_pLevel->getBrightness(tilePos);
	if (a < 0.0f)
		return;
	if (a > 1.0f) a = 1.0f;

	t.color(1.0f, 1.0f, 1.0f, a);
	float x0 = (float)tilePos.x + tt->m_aabb.min.x + oPos.x;
	float x1 = (float)tilePos.x + tt->m_aabb.max.x + oPos.x;
	float y0 = (float)tilePos.y + tt->m_aabb.min.y + oPos.y + (1.0f / 64.0f);
	float z0 = (float)tilePos.z + tt->m_aabb.min.z + oPos.z;
	float z1 = (float)tilePos.z + tt->m_aabb.max.z + oPos.z;
	float u0 = ((pos.x - x0) / 2.0f / r + 0.5f);
	float u1 = ((pos.x - x1) / 2.0f / r + 0.5f);
	float v0 = ((pos.z - z0) / 2.0f / r + 0.5f);
	float v1 = ((pos.z - z1) / 2.0f / r + 0.5f);
	t.vertexUV(x0, y0, z0, u0, v0);
	t.vertexUV(x0, y0, z1, u0, v1);
	t.vertexUV(x1, y0, z1, u1, v1);
	t.vertexUV(x1, y0, z0, u1, v0);
}

void LevelRenderer::_recreateTessellators()
{

}

void LevelRenderer::_setupFog(const Entity& camera, int i)
{
#ifdef FEATURE_GFX_SHADERS
	if (i != 1)
		return;
#endif

	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	mce::FogStateDescription& desc = Fog::nextState;
	GameRenderer& gameRenderer = *m_pMinecraft->m_pGameRenderer;
	float renderDistance = gameRenderer.m_renderDistance;

	if (camera.isUnderLiquid(Material::water))
	{
		desc.fogMode = mce::FOG_MODE_EXP;
		desc.fogDensity = 0.1f;
	}
	else if (camera.isUnderLiquid(Material::lava))
	{
		desc.fogMode = mce::FOG_MODE_EXP;
		desc.fogDensity = 2.0f;
	}
	else
	{
		desc.fogMode = mce::FOG_MODE_LINEAR;

		if (i < 0)
		{
			m_fogControl = Vec2(0.0f, 0.8f);
		}
		else
		{
			m_fogControl = Vec2(0.25f, 1.0f);
		}

		if (m_pMinecraft->m_pLevel->m_pDimension->m_bFoggy)
		{
			m_fogControl.x = 0.0f;
		}

		desc.fogStartZ = m_fogControl.x * renderDistance;
		desc.fogEndZ = m_fogControl.y * renderDistance;
	}

	Fog::updateState();

	if (desc.fogStartZ != m_lastFogState.fogStartZ || desc.fogEndZ != m_lastFogState.fogEndZ)
	{
#ifdef FEATURE_GFX_SHADERS
		// rebuild sky mesh to account for updated fog distance
		_buildSkyMesh();
#endif
		m_lastFogState = desc;
	}
}

const Color& LevelRenderer::_getFogColor() const
{
	const mce::FogStateDescription& fogState = Fog::nextState;

	return fogState.fogColor;
}

void LevelRenderer::_updateViewArea(const Entity& camera)
{
}

void LevelRenderer::_startFrame(FrustumCuller& culler, float renderDistance, float f)
{
	mce::GlobalConstantBuffers& globalBuffers = mce::GlobalConstantBuffers::getInstance();
	mce::PerFrameConstants& frame = globalBuffers.m_perFrameConstants;

	const Entity& camera = *m_pMinecraft->m_pCameraEntity;
	m_viewPos = camera.getPos(f);

	_setupFog(camera, 1);

#ifdef FEATURE_GFX_SHADERS
	Vec3 viewVector = camera.getViewVector(f);
	frame.VIEW_DIRECTION->setData(&viewVector);

	float time = getTimeS() - m_initTime;
	frame.TIME->setData(&time);

	float farChunksDistance = renderDistance - 7.0; // something minus 7, just a guess
	frame.FAR_CHUNKS_DISTANCE->setData(&farChunksDistance);

	frame.FOG_COLOR->setData(&_getFogColor());

	frame.RENDER_DISTANCE->setData(&renderDistance);

	frame.VIEW_POS->setData(&m_viewPos);

	frame.FOG_CONTROL->setData(&m_fogControl);

	frame.sync();
#endif
}

const mce::MaterialPtr& LevelRenderer::_chooseOverlayMaterial(Tile::RenderLayer layer) const
{
	switch (layer)
	{
	case Tile::RENDER_LAYER_DOUBLE_SIDED:
	case Tile::RENDER_LAYER_OPTIONAL_ALPHATEST:
	case Tile::RENDER_LAYER_ALPHATEST:
	case Tile::RENDER_LAYER_SEASONS_OPTIONAL_ALPHATEST:
		return m_materials.selection_overlay_double_sided;
	case Tile::RENDER_LAYER_OPAQUE:
	case Tile::RENDER_LAYER_SEASONS_OPAQUE:
		return m_materials.selection_overlay_opaque;
	default:
		return m_materials.selection_overlay;
	}
}

void LevelRenderer::onLowMemory()
{
	Tesselator::instance.trim();
}

void LevelRenderer::onAppResumed()
{
	onGraphicsReset();
}

void LevelRenderer::onAppSuspended()
{
	m_renderList.reset();
	
	//m_shadowVolumeMesh.reset();
	//m_shadowOverlayMesh.reset();
	m_starsMesh.reset();
	m_cloudsMesh.reset();
	m_skyMesh.reset();
	m_starsMesh.reset();
}

void LevelRenderer::deleteChunks()
{
	for (int i = 0; i < m_zChunks; i++)
	{
		for (int j = 0; j < m_yChunks; j++)
		{
			for (int k = 0; k < m_xChunks; k++)
			{
				int index = k + m_xChunks * (j + m_yChunks * i);
				delete m_chunks[index];
			}
		}
	}

	if (m_chunks)
		delete[] m_chunks;
	m_chunks = nullptr;

	if (m_sortedChunks)
		delete[] m_sortedChunks;
	m_sortedChunks = nullptr;
}

void LevelRenderer::cull(Culler* pCuller, float f)
{
	for (int i = 0; i < m_chunksLength; i++)
	{
		Chunk* pChunk = m_chunks[i];
		if (pChunk->isEmpty())
			continue;

		if (!pChunk->m_bVisible || (i + m_cullStep & 15) == 0)
		{
			pChunk->cull(pCuller);
		}
	}

	m_cullStep++;
}

void LevelRenderer::allChanged()
{
	deleteChunks();

	LeafTile* pLeaves = (LeafTile*)Tile::leaves;

	pLeaves->m_bTransparent = m_pMinecraft->getOptions()->m_bFancyGraphics;
	pLeaves->m_TextureFrame = !pLeaves->m_bTransparent + pLeaves->field_74;

	TileRenderer::m_bFancyGrass = m_pMinecraft->getOptions()->m_bFancyGrass;
	TileRenderer::m_bBiomeColors = m_pMinecraft->getOptions()->m_bBiomeColors;

	m_lastViewDistance = m_pMinecraft->getOptions()->m_iViewDistance;

	int dist = 64 << (3 - m_lastViewDistance);
	if (dist > 400)
		dist = 400;

	m_xChunks = dist / 16 + 1;
	m_yChunks = 8;
	m_zChunks = dist / 16 + 1;

	m_chunksLength = m_yChunks * m_xChunks * m_zChunks;
	LOG_I("chunksLength: %d", m_chunksLength);
	m_chunks = new Chunk* [m_chunksLength];
	m_sortedChunks = new Chunk* [m_chunksLength];

	m_xMinChunk = 0;
	m_yMinChunk = 0;
	m_zMinChunk = 0;

	m_dirtyChunks.clear();
	//m_renderableTileEntities.clear();

	m_xMaxChunk = m_xChunks;
	m_yMaxChunk = m_yChunks;
	m_zMaxChunk = m_zChunks;

	int count = 0, id = 0;

	// These are actually Chunk coordinates that get converted to Tile coordinates
	TilePos cp(0, 0, 0);
	for (cp.x = 0; cp.x < m_xChunks; cp.x++)
	{
		if (m_yChunks <= 0)
			continue;

		for (cp.y = 0; cp.y < m_yChunks; cp.y++)
		{
			for (cp.z = 0; cp.z < m_zChunks; cp.z++)
			{
				int index = (cp.z * m_yChunks + cp.y) * m_xChunks + cp.x;

				Chunk* pChunk = new Chunk(m_pLevel, cp * 16, 16, id + m_chunkLists);

				if (m_bOcclusionCheck)
					pChunk->m_occlusionId = 0; // m_occlusionCheckIds.get(count)

				pChunk->m_bOcclusionQuerying = false;
				pChunk->m_bOcclusionVisible = true;
				pChunk->m_bVisible = true;
				pChunk->m_id = count++;
				pChunk->setDirty();
				m_chunks[index] = pChunk;
				m_sortedChunks[index] = pChunk;

				id += 3;
				m_dirtyChunks.push_back(pChunk);
			}
		}
	}

	if (m_pLevel)
	{
		Mob* pMob = m_pMinecraft->m_pCameraEntity;
		if (pMob)
		{
			resortChunks(pMob->m_pos);

			std::sort(&m_sortedChunks[0], &m_sortedChunks[m_chunksLength], DistanceChunkSorter(*pMob));
		}
	}

	m_noEntityRenderFrames = 2;
}

void LevelRenderer::resortChunks(const TilePos& pos)
{
	TilePos tp(pos - 8);
	m_xMinChunk = INT_MIN;
	m_yMinChunk = INT_MIN;
	m_zMinChunk = INT_MIN;
	m_xMaxChunk = INT_MAX;
	m_yMaxChunk = INT_MAX;
	m_zMaxChunk = INT_MAX;

	int blkCount = m_xChunks * 16;
	int blkCntHalf = blkCount / 2;

	for (int fx = 0; fx < m_xChunks; fx++)
	{
		int x1 = 16 * fx;
		int x2 = x1 + blkCntHalf - tp.x;
		if (x2 < 0) x2 -= blkCount - 1;
		x2 /= blkCount;
		x1 -= blkCount * x2;

		if (m_xMinChunk > x1)
			m_xMinChunk = x1;
		if (m_xMaxChunk < x1)
			m_xMaxChunk = x1;

		for (int fz = 0; fz < m_zChunks; fz++)
		{
			int z1 = 16 * fz;
			int z2 = z1 + blkCntHalf - tp.z;
			if (z2 < 0) z2 -= blkCount - 1;
			z2 /= blkCount;
			z1 -= blkCount * z2;

			if (m_zMinChunk > z1)
				m_zMinChunk = z1;
			if (m_zMaxChunk < z1)
				m_zMaxChunk = z1;

			for (int fy = 0; fy < m_yChunks; fy++)
			{
				int y1 = 16 * fy;
				if (m_yMinChunk > y1)
					m_yMinChunk = y1;
				if (m_yMaxChunk < y1)
					m_yMaxChunk = y1;

				Chunk* pChunk = m_chunks[(fz * m_yChunks + fy) * m_xChunks + fx];
				bool wasDirty = pChunk->isDirty();
				pChunk->setPos(TilePos(x1, y1, z1));

				if (!wasDirty && pChunk->isDirty())
					m_dirtyChunks.push_back(pChunk);
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
	mce::Mesh::clearGlobalBuffers();
	_initResources();
	_recreateTessellators();
	allChanged();
}

void LevelRenderer::renderLineBox(const AABB& aabb, const mce::MaterialPtr& material, float lineWidth) const
{
#if MCE_GFX_API_OGL
	// Maximize Line Width
	glEnable(GL_LINE_SMOOTH);

	float range[2];
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, range);

	if (lineWidth > range[1])
		lineWidth = range[1];

	glLineWidth(lineWidth);
#endif

	Tesselator& t = Tesselator::instance;

	t.begin(mce::PRIMITIVE_MODE_LINE_STRIP, 5);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.draw(material);
	t.begin(mce::PRIMITIVE_MODE_LINE_STRIP, 5);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.draw(material);
	t.begin(mce::PRIMITIVE_MODE_LINE_LIST, 5);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.draw(material);
}

void LevelRenderer::checkQueryResults(int a, int b)
{
}

void LevelRenderer::renderSameAsLast(TerrainLayer layer, float alpha, bool fog)
{
	m_renderList.render(layer, fog);
}

int LevelRenderer::renderChunks(int start, int end, Tile::RenderLayer layer, float alpha, bool fog)
{
	field_24.clear();

	int result = 0;
	for (int i = start; i < end; i++)
	{
		Chunk* pChunk = m_sortedChunks[i];
		if (layer == Tile::RENDER_LAYER_OPAQUE)
		{
			m_totalChunks++;
			if (pChunk->m_empty[Tile::RENDER_LAYER_OPAQUE])
			{
				m_emptyChunks++;
			}
			else if (pChunk->m_bVisible)
			{
				if (!m_bOcclusionCheck || pChunk->m_bOcclusionVisible)
					m_renderedChunks++;
				else
					m_occludedChunks++;
			}
			else
			{
				m_offscreenChunks++;
			}
		}

		if (!pChunk->m_empty[layer] && pChunk->m_bVisible && pChunk->m_bOcclusionVisible && pChunk->getList(layer) >= 0)
		{
			result++;
			field_24.push_back(pChunk);
		}
	}

	Mob* pMob = m_pMinecraft->m_pCameraEntity;

	Vec3 fPos = pMob->m_posPrev + (pMob->m_pos - pMob->m_posPrev) * alpha;

	m_renderList.clear();
	m_renderList.init(fPos);

	for (int i = 0; i < int(field_24.size()); i++)
	{
		Chunk* pChk = field_24[i];
		m_renderList.addR(pChk->getRenderChunk(layer), renderLayerToTerrainLayerMap[layer], fog);
	}

	renderSameAsLast(renderLayerToTerrainLayerMap[layer], alpha, fog);
	return result;
}

void LevelRenderer::render(const Entity& camera, Tile::RenderLayer layer, float alpha, bool fog)
{
	for (int i = 0; i < 10; i++)
	{
		field_68 = (field_68 + 1) % m_chunksLength;
		Chunk* pChunk = m_chunks[field_68];

		if (!pChunk->m_bDirty)
			continue;

		std::vector<Chunk*>::iterator iter = std::find(m_dirtyChunks.begin(), m_dirtyChunks.end(), pChunk);
		if (iter != m_dirtyChunks.end())
			continue;

		m_dirtyChunks.push_back(pChunk);
	}

	if (m_pMinecraft->getOptions()->m_iViewDistance != m_lastViewDistance)
		allChanged();

	if (layer == Tile::RENDER_LAYER_OPAQUE)
		m_totalChunks = m_offscreenChunks = m_occludedChunks = m_renderedChunks = m_emptyChunks = 0;

	Vec3 cameraPos = camera.m_posPrev + (camera.m_pos - camera.m_posPrev) * alpha;

	float dX = camera.m_pos.x - m_posPrev.x, dY = camera.m_pos.y - m_posPrev.y, dZ = camera.m_pos.z - m_posPrev.z;

	if (dX * dX + dY * dY + dZ * dZ > 16.0f)
	{
		m_posPrev = camera.m_pos;

		resortChunks(camera.m_pos);
		std::sort(&m_sortedChunks[0], &m_sortedChunks[m_chunksLength], DistanceChunkSorter(camera));
	}

	if (layer == Tile::RENDER_LAYER_BLEND)
	{
		mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
		renderContext.clearStencilBuffer();
	}

	// @NOTE: m_bOcclusionCheck is always false
	if (m_bOcclusionCheck && layer == Tile::RENDER_LAYER_OPAQUE && !m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		assert(false);
		/*int c = 16;
		checkQueryResults(0, c);
		
		// @HUH: why 16?
		for (int i = 0; i < c; i++)
			m_sortedChunks[i]->m_bOcclusionVisible = true;

		int d = renderChunks(0, c, Tile::RENDER_LAYER_OPAQUE, alpha);

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
					m_chunks[i]->m_bOcclusionVisible = true;
					continue;
				}

				if (!m_chunks[i]->m_bOcclusionQuerying)
					continue;

				int roughDist = int(Mth::sqrt(m_chunks[i]->distanceToSqr(camera)) / 128.0f + 1.0f);

				if (m_ticksSinceStart % roughDist != i % roughDist)
					continue;
				
				float fXdiff, fYdiff, fZdiff;
				fXdiff = float(m_chunks[i]->m_pos.x) - cameraPos.x - lastX;
				fYdiff = float(m_chunks[i]->m_pos.y) - cameraPos.y - lastY;
				fZdiff = float(m_chunks[i]->m_pos.z) - cameraPos.z - lastZ;

				if (fXdiff != 0.0f || fYdiff != 0.0f || fZdiff != 0.0f)
				{
					glTranslatef(fXdiff, fYdiff, fZdiff);

					lastX += fXdiff;
					lastY += fYdiff;
					lastZ += fZdiff;
				}

				m_chunks[i]->renderBB();
				m_chunks[i]->m_bOcclusionQuerying = true;
			}
		}
		while (c < m_chunksLength);*/
	}
	else
	{
		renderChunks(0, m_chunksLength, layer, alpha, fog);
	}
}

const Color& LevelRenderer::setupClearColor(float f)
{
	Minecraft& mc = *m_pMinecraft;
	const Options& options = *mc.getOptions();
	Level& level = *mc.m_pLevel;
	const Entity& camera = *mc.m_pCameraEntity;

	float x1 = 1.0f - powf(1.0f / float(4 - options.m_iViewDistance), 0.25f);

	Vec3 skyColor = level.getSkyColor(camera, f), fogColorVec = level.getFogColor(f);

	Color& fogColor = Fog::nextState.fogColor;

	fogColor.r = fogColorVec.x + (skyColor.x - fogColorVec.x) * x1;
	fogColor.g = fogColorVec.y + (skyColor.y - fogColorVec.y) * x1;
	fogColor.b = fogColorVec.z + (skyColor.z - fogColorVec.z) * x1;
	fogColor.a = 1.0f;

	if (camera.isUnderLiquid(Material::water))
	{
		fogColor = Color(0.02f, 0.02f, 0.2f);
	}
	else if (camera.isUnderLiquid(Material::lava))
	{
		fogColor = Color(0.6f, 0.1f, 0.0f);
	}

	float x2 = m_fogBrO + (m_fogBr - m_fogBrO) * f;

	fogColor *= x2;

	if (options.m_bAnaglyphs)
	{
		fogColor.r = (fogColor.r * 30.0f + fogColor.g * 59.0f + fogColor.b * 11.0f) / 100.0f;
		fogColor.g = (fogColor.r * 30.0f + fogColor.g * 70.0f) / 100.0f;
		fogColor.b = (fogColor.r * 30.0f + fogColor.b * 70.0f) / 100.0f;
	}

	return fogColor;
}

void LevelRenderer::setLevel(Level* level)
{
	if (m_pLevel == level)
		return;

	if (m_pLevel)
		m_pLevel->removeListener(this);

	m_posPrev = Vec3(-9999.0f, -9999.0f, -9999.0f);

	EntityRenderDispatcher::getInstance()->setLevel(level);
	EntityRenderDispatcher::getInstance()->setMinecraft(m_pMinecraft);

	m_pLevel = level;

	delete m_pTileRenderer;
	m_pTileRenderer = new TileRenderer(Tesselator::instance, m_pLevel);

	if (level)
	{
		level->addListener(this);
	}

	allChanged();
}

void LevelRenderer::setDimension(Dimension* dimension)
{
	m_pDimension = dimension;

	if (dimension)
	{
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
		int x1 = x % m_xChunks;
		if (x1 < 0)
			x1 += m_xChunks;

		for (int y = minY; y <= maxY; y++)
		{
			int y1 = y % m_yChunks;
			if (y1 < 0)
				y1 += m_yChunks;

			for (int z = minZ; z <= maxZ; z++)
			{
				int z1 = z % m_zChunks;
				if (z1 < 0)
					z1 += m_zChunks;

				Chunk* pChunk = m_chunks[x1 + m_xChunks * (y1 + m_yChunks * z1)];
				if (pChunk->isDirty())
					continue;

				m_dirtyChunks.push_back(pChunk);
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
	const Entity& camera = *m_pMinecraft->m_pCameraEntity;
	const Level& level = *m_pMinecraft->m_pLevel;
	const Options& options = *m_pMinecraft->getOptions();

	m_fogBrO = m_fogBr;

	float bright = level.getBrightness(camera.m_pos);
	float x3 = float(3 - options.m_iViewDistance);
	float x4 = x3 / 3.0f;
	float x5 = (x4 + bright * (1.0f - x4) - m_fogBr) * 0.1f;

	m_fogBr += x5;

	m_ticksSinceStart++;
}

typedef std::vector<Chunk*> ChunkVector;
typedef ChunkVector::iterator ChunkVectorIterator;

bool LevelRenderer::updateDirtyChunks(const Entity& camera, bool b)
{
	constexpr int C_MAX = 3;
	DirtyChunkSorter dcs(camera);
	Chunk* pChunks[C_MAX] = { nullptr };
	ChunkVector* nearChunks = nullptr;

	int pendingChunkRemoved = 0;
	int pendingChunkSize = int(m_dirtyChunks.size());
	for (int i = 0; i < pendingChunkSize; i++)
	{
		Chunk* pChunk = m_dirtyChunks[i];
		if (!b)
		{
			if (pChunk->distanceToSqr(camera) > 1024.0f)
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

		if (!nearChunks)
			nearChunks = new ChunkVector;

		pendingChunkRemoved++;
		nearChunks->push_back(pChunk);
		m_dirtyChunks[i] = nullptr;
	}

	if (nearChunks)
	{
		if (nearChunks->size() > 1)
			std::sort(nearChunks->begin(), nearChunks->end(), dcs);

		for (int i = int(nearChunks->size()) - 1; i >= 0; i--)
		{
			(*nearChunks)[i]->rebuild();
			(*nearChunks)[i]->setClean();
		}

		SAFE_DELETE(nearChunks);
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
	for (; nr4 < int(m_dirtyChunks.size()); nr4++)
	{
		Chunk* pChunk = m_dirtyChunks[nr4];
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
			m_dirtyChunks[nr3] = pChunk;

		nr3++;
	}

	if (nr4 > nr3)
		m_dirtyChunks.erase(m_dirtyChunks.begin() + nr3, m_dirtyChunks.end());

	return pendingChunkRemoved + nr2 == pendingChunkSize;
}

void LevelRenderer::renderCracks(const Entity& camera, const HitResult& hr, int mode, const ItemInstance* inventoryItem, float a)
{
	// @BUG: possible leftover from Minecraft Classic? This is overridden anyways
	//currentShaderColor = Color(1.0f, 1.0f, 1.0f, (Mth::sin(float(getTimeMs()) / 100.0f) * 0.2f + 0.4f) * 0.5f);

	if (mode == 0)
	{
		if (m_destroyProgress > 0.0f)
		{
			m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
			currentShaderColor = Color::WHITE;
			currentShaderDarkColor = Color(1.0f, 1.0f, 1.0f, 0.5f);

			MatrixStack::Ref matrix = MatrixStack::World.push();

			Tile* pTile = nullptr;
			TileID tile = m_pLevel->getTile(hr.m_tilePos);
			if (tile > 0)
				pTile = Tile::tiles[tile];

			float px = camera.m_posPrev.x + (camera.m_pos.x - camera.m_posPrev.x) * a;
			float py = camera.m_posPrev.y + (camera.m_pos.y - camera.m_posPrev.y) * a;
			float pz = camera.m_posPrev.z + (camera.m_pos.z - camera.m_posPrev.z) * a;

			Tesselator& t = Tesselator::instance;

			t.begin(0);
			t.setOffset(-px, -py, -pz);
			t.noColor();
			if (!pTile)
				pTile = Tile::rock;

			m_pTileRenderer->tesselateInWorld(pTile, hr.m_tilePos, 240 + int(m_destroyProgress * 10.0f));

			t.draw(m_materials.cracks_overlay);
			t.setOffset(0, 0, 0);
		}
	}
    else if (inventoryItem != nullptr)
	{
         float br = Mth::sin((float)getTimeMs() / 100.0f) * 0.2f + 0.8f;
		 currentShaderColor = Color(br, br, br, Mth::sin((float)getTimeMs() / 200.0f) * 0.2f + 0.5f);
		 m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
         TilePos tp = hr.m_tilePos.relative(hr.m_hitSide);
	}
}

void LevelRenderer::renderHitSelect(const Entity& camera, const HitResult& hr, int mode, const ItemInstance* inventoryItem, float a)
{
	if (mode != 0) return;

	m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

	Tile* pTile = nullptr;
	TileID tileID = m_pLevel->getTile(hr.m_tilePos);
	if (tileID > 0)
		pTile = Tile::tiles[tileID];

	currentShaderColor = Color(0.65f, 0.65f, 0.65f, 0.65f);

	MatrixStack::Ref matrix = MatrixStack::World.push();

	float px = camera.m_posPrev.x + (camera.m_pos.x - camera.m_posPrev.x) * a;
	float py = camera.m_posPrev.y + (camera.m_pos.y - camera.m_posPrev.y) * a;
	float pz = camera.m_posPrev.z + (camera.m_pos.z - camera.m_posPrev.z) * a;

	Tesselator& t = Tesselator::instance;
	t.begin(0);
	t.setOffset(-px, -py, -pz);
	t.noColor();
	if (!pTile)
		pTile = Tile::rock;

	m_pTileRenderer->tesselateInWorld(pTile, hr.m_tilePos);

	Tile::RenderLayer renderLayer = pTile->getRenderLayer();
	const mce::MaterialPtr& material = _chooseOverlayMaterial(renderLayer);

	t.draw(material);
	t.setOffset(0, 0, 0);
}

void LevelRenderer::renderHitOutline(const Entity& camera, const HitResult& hr, int mode, const ItemInstance* inventoryItem, float a)
{
	if (mode != 0 || hr.m_hitType != 0)
		return;

	currentShaderColor = Color(0.0f, 0.0f, 0.0f, 0.4f);

	float lineWidth = 2.0f * Minecraft::getRenderScaleMultiplier();

	TileID tile = m_pLevel->getTile(hr.m_tilePos);
	if (tile > 0)
	{
		Tile::tiles[tile]->updateShape(
			m_pLevel,
			hr.m_tilePos);
		float posX = camera.m_posPrev.x + ((camera.m_pos.x - camera.m_posPrev.x) * a);
		float posY = camera.m_posPrev.y + ((camera.m_pos.y - camera.m_posPrev.y) * a);
		float posZ = camera.m_posPrev.z + ((camera.m_pos.z - camera.m_posPrev.z) * a);
		AABB aabb, tileAABB = Tile::tiles[tile]->getTileAABB(m_pLevel, hr.m_tilePos);
		aabb.min.y = tileAABB.min.y - 0.002f - posY;
		aabb.max.y = tileAABB.max.y + 0.002f - posY;
		aabb.min.z = tileAABB.min.z - 0.002f - posZ;
		aabb.max.z = tileAABB.max.z + 0.002f - posZ;
		aabb.min.x = tileAABB.min.x - 0.002f - posX;
		aabb.max.x = tileAABB.max.x + 0.002f - posX;
		renderLineBox(aabb, m_materials.selection_box, lineWidth);
	}
}

void LevelRenderer::tileChanged(const TilePos& pos)
{
	setDirty(pos - 1, pos + 1);
}

extern int t_keepPic;

void LevelRenderer::takePicture(TripodCamera* pCamera, Entity* pOwner)
{
	Mob* pOldMob = m_pMinecraft->m_pCameraEntity;
	bool bOldDontRenderGui = m_pMinecraft->getOptions()->m_bDontRenderGui;
	bool bOldThirdPerson = m_pMinecraft->getOptions()->m_bThirdPerson;

#ifdef ENH_CAMERA_NO_PARTICLES
	extern bool g_bDisableParticles;
	g_bDisableParticles = true;
#endif

	m_pMinecraft->m_pCameraEntity = pCamera;
	m_pMinecraft->getOptions()->m_bDontRenderGui = true;
	m_pMinecraft->getOptions()->m_bThirdPerson = false; // really from the perspective of the camera
	m_pMinecraft->m_pGameRenderer->render(0.0f);
	m_pMinecraft->m_pCameraEntity = pOldMob;
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

	if (m_pMinecraft->m_pCameraEntity->distanceToSqr_inline(pos) > maxDist)
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
	float playerDist = m_pMinecraft->m_pCameraEntity->distanceToSqr(pos);

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

void LevelRenderer::renderLevel(const Entity& camera, FrustumCuller& culler, float renderDistance, float f)
{
	if (!m_pLevel)
		return;

	ParticleEngine& particleEngine = *m_pMinecraft->m_pParticleEngine;
	Textures& textures = *m_pMinecraft->m_pTextures;
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	_updateViewArea(camera);
	_startFrame(culler, renderDistance, f);
	Fog::enable();

	cull(&culler, f);
	updateDirtyChunks(camera, false);

	// TODO[v0.6.1]: what is (this+4)+63 (byte)?
	prepareAndRenderClouds(camera, f);

	_setupFog(camera, 0);
	Fog::enable();

	bool fog = m_pDimension->isNaturalDimension();

	textures.loadAndBindTexture(C_TERRAIN_NAME);
	Lighting::turnOff();
	render(camera, Tile::RENDER_LAYER_SEASONS_OPAQUE, f, fog);
	render(camera, Tile::RENDER_LAYER_OPAQUE, f, fog);
	render(camera, Tile::RENDER_LAYER_DOUBLE_SIDED, f, fog);

	render(camera, Tile::RENDER_LAYER_ALPHATEST, f, fog);
	Lighting::turnOn();

	renderEntities(camera.getPos(f), &culler, f);
	particleEngine.renderLit(camera, f);
	Lighting::turnOff();
	particleEngine.render(camera, f);

	_setupFog(camera, 0);

	renderContext.setShadeMode(mce::SHADE_MODE_SMOOTH);

	if (camera.isUnderLiquid(Material::water))
	{
		//renderWeather(f);
		RenderChunk::SetUnderwater(true);
	}
	else
	{
		RenderChunk::SetUnderwater(false);
	}

	textures.loadAndBindTexture(C_TERRAIN_NAME);
	render(camera, Tile::RENDER_LAYER_BLEND, f, fog);

	renderContext.setShadeMode(mce::SHADE_MODE_FLAT);

	//renderNameTags(f);

	if (!camera.isUnderLiquid(Material::water))
	{
		//renderWeather(f);
	}

	// Was after renderCracks in GameRenderer
	Fog::disable();
}

void LevelRenderer::renderEntities(Vec3 pos, Culler* culler, float f)
{
	if (m_noEntityRenderFrames > 0)
	{
		m_noEntityRenderFrames--;
		return;
	}

	const Mob* camera = m_pMinecraft->m_pCameraEntity;

	EntityRenderDispatcher::getInstance()->prepare(m_pLevel, m_pMinecraft->m_pTextures, m_pMinecraft->m_pFont, camera, m_pMinecraft->getOptions(), f);

	m_totalEntities = 0;
	m_renderedEntities = 0;
	m_culledEntities = 0;

	EntityRenderDispatcher::off = camera->m_posPrev + (camera->m_pos - camera->m_posPrev) * f;

	const EntityVector* pVec = m_pLevel->getAllEntities();
	m_totalEntities = int(pVec->size());

	for (int i = 0; i < m_totalEntities; i++)
	{
		const Entity* entity = (*pVec)[i];
		if (!entity->shouldRender(pos))
			continue;

		if (!culler->isVisible(entity->m_hitbox))
			continue;

		if (m_pMinecraft->m_pCameraEntity == entity && !m_pMinecraft->getOptions()->m_bThirdPerson)
			continue;

		if (m_pLevel->hasChunkAt(entity->m_pos))
		{
			m_renderedEntities++;
			EntityRenderDispatcher::getInstance()->render(*entity, f);
		}
	}
}

void LevelRenderer::renderShadow(const Entity& entity, const Vec3& pos, float r, float pow, float a)
{
	Textures& textures = *m_pMinecraft->m_pTextures;

	textures.setClampToEdge(true);
	textures.loadAndBindTexture("misc/shadow.png");
	textures.setClampToEdge(false);

	Vec3 ePos(entity.m_posPrev + (entity.m_pos - entity.m_posPrev) * a);
	ePos.y -= entity.m_heightOffset; // We gotta do this so the renderer can correctly determine if there's a tile below the entity
	ePos.y += entity.getShadowHeightOffs();

	TilePos tpMin(ePos - r);
	TilePos tpMax(ePos.x + r, ePos.y, ePos.z + r);
	Vec3 ePosO(pos - ePos);

	Tesselator& tt = Tesselator::instance;
	tt.begin(0);
	TilePos tp(tpMin);
	for (tp.x = tpMin.x; tp.x <= tpMax.x; tp.x++)
	{
		for (tp.y = tpMin.y; tp.y <= tpMax.y; tp.y++)
		{
			for (tp.z = tpMin.z; tp.z <= tpMax.z; tp.z++)
			{
				TileID t = m_pLevel->getTile(tp.below());
				if (t > 0 && m_pLevel->getRawBrightness(tp) > 3)
				{
					_renderTileShadow(Tile::tiles[t],
						Vec3(pos.x, pos.y - entity.m_heightOffset + entity.getShadowHeightOffs(), pos.z), tp,
						pow, r,
						Vec3(ePosO.x, ePosO.y - entity.m_heightOffset + entity.getShadowHeightOffs(), ePosO.z)
					);
				}
			}
		}
	}
	tt.draw(m_materials.shadow_image_overlay);
}

void LevelRenderer::renderSky(const Entity& camera, float alpha)
{
	if (m_pMinecraft->m_pLevel->m_pDimension->m_bFoggy)
		return;

	Vec3 sc = m_pLevel->getSkyColor(camera, alpha);
	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		sc.x = (((sc.x * 30.0f) + (sc.y * 59.0f)) + (sc.z * 11.0f)) / 100.0f;
		sc.y = ((sc.x * 30.0f) + (sc.y * 70.0f)) / 100.0f;
		sc.z = ((sc.x * 30.0f) + (sc.z * 70.0f)) / 100.0f;
	}

	// called again a few lines down, no min in Java, why is it here?
	//currentShaderColor = Color(sc.x, sc.y, Mth::Min(1.0f, sc.z), 1.0f);

	Tesselator& t = Tesselator::instance;

	{
		Fog::enable();
		// @TODO: can we avoid rebuilding the mesh every time the fog updates?
		//float fogStart = Fog::nextState.fogStartZ;

		//MatrixStack::Ref matrix = MatrixStack::World.push();
		//matrix->scale(Vec3(fogStart, 0.0f, fogStart));

		currentShaderColor = Color(sc.x, sc.y, sc.z);
		m_skyMesh.render(m_materials.skyplane);

		Fog::disable();
	}

	_renderSunrise(alpha);
	_renderSolarSystem(alpha);
	
	Fog::enable();

	if (m_darkMesh.isValid())
	{
		currentShaderColor = Color(sc.x * 0.2f + 0.04f, sc.y * 0.2f + 0.04f, sc.z * 0.6f + 0.1f);
		m_darkMesh.render(m_materials.skyplane);
	}
}

void LevelRenderer::prepareAndRenderClouds(const Entity& camera, float f)
{
	GameRenderer& gameRenderer = *m_pMinecraft->m_pGameRenderer;
	float renderDistance = gameRenderer.m_renderDistance;
	float fov = gameRenderer.getFov(f);

	MatrixStack::Ref projMtx = MatrixStack::Projection.pushIdentity();
	projMtx->setPerspective(fov, float(Minecraft::width) / float(Minecraft::height), 0.05f, renderDistance * 512.0f);

	MatrixStack::Ref viewMtx = MatrixStack::View.push();
	_setupFog(camera, 0);

	Fog::enable();

	Fog::updateRange(renderDistance * 0.2f, renderDistance * 0.75f);
	renderSky(camera, f);

	Fog::updateRange(renderDistance * 4.2f * 0.6f, renderDistance * 4.2f);
	renderClouds(camera, f);

	Fog::updateRange(renderDistance * 0.6f, renderDistance);

	Fog::disable();

	_setupFog(camera, 1);
}

void LevelRenderer::renderClouds(const Entity& camera, float alpha)
{
	if (!areCloudsAvailable())
		return;

	if (m_pMinecraft->getOptions()->m_bFancyGraphics)
	{
		renderAdvancedClouds(alpha);
		return;
	}

	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	float yPos = Mth::Lerp(camera.m_posPrev.y, camera.m_pos.y, alpha); // not certain if this old pos Y is used
	float yy = ((float)C_MAX_Y - yPos) + 0.33f; // 108.0f on b1.2_02, see below

	if (yy > 1.0f)
	{
		renderContext.setDepthRange(0.0f, 1.0f);
	}

	m_pTextures->loadAndBindTexture("environment/clouds.png");

	Vec3 cloudColor = m_pLevel->getCloudColor(alpha);

	float offX = Mth::Lerp(camera.m_oPos.x, camera.m_pos.x, alpha) + (float(m_ticksSinceStart) + alpha) * 0.03f;
	float offZ = Mth::Lerp(camera.m_oPos.z, camera.m_pos.z, alpha);
	
	int dx2048 = Mth::floor(offX / 2048.0f);
	int dz2048 = Mth::floor(offZ / 2048.0f);

	offX -= float(dx2048 * 2048);
	offZ -= float(dz2048 * 2048);

	Tesselator& t = Tesselator::instance;

	float fYPos = ((float)C_MAX_Y - yPos) + 0.33f;
	offX /= 2048.0f;
	offZ /= 2048.0f;
	t.begin(1024);
	t.color(cloudColor.x, cloudColor.y, cloudColor.z, 0.8f);

	constexpr int incr = 16 * 2;
	constexpr int maxX = C_MAX_CHUNKS_X * 16;
	constexpr int maxZ = C_MAX_CHUNKS_Z * 16;
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
	t.draw(m_materials.clouds);

	if (yy > 1.0f)
	{
		renderContext.setDepthRange(0.0f, 0.7f);
	}
	
	currentShaderColor = Color::WHITE;
}

void LevelRenderer::renderAdvancedClouds(float alpha)
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	float yOffs = //Mth::Lerp(m_pMinecraft->m_pCameraEntity->m_posPrev.y, m_pMinecraft->m_pCameraEntity->m_pos.y, alpha);
    m_pMinecraft->m_pCameraEntity->m_posPrev.y + (m_pMinecraft->m_pCameraEntity->m_pos.y - m_pMinecraft->m_pCameraEntity->m_posPrev.y) * alpha;

	Tesselator& t = Tesselator::instance;
	constexpr float ss = 12.0f;
	constexpr float h = 4.0f;

	// @NOTE: Using Mth::Lerp will use incorrect logic
	float xo = (m_pMinecraft->m_pCameraEntity->m_oPos.x + (m_pMinecraft->m_pCameraEntity->m_pos.x - m_pMinecraft->m_pCameraEntity->m_oPos.x) * alpha + ((float(m_ticksSinceStart) + alpha) * 0.03f)) / ss;
	float zo = (m_pMinecraft->m_pCameraEntity->m_oPos.z + (m_pMinecraft->m_pCameraEntity->m_pos.z - m_pMinecraft->m_pCameraEntity->m_oPos.z) * alpha) / ss + 0.33f;

	float yy = ((float)C_MAX_Y - yOffs) + 0.33f; // 108.0f on b1.2_02, see below
    //float yy = 108.0f - yOffs + 0.33F;

	int xOffs = Mth::floor(xo / 2048);
	int zOffs = Mth::floor(zo / 2048);

	xo -= xOffs * 2048;
	zo -= zOffs * 2048;

	if (yy > 1.0f)
	{
		renderContext.setDepthRange(0.0f, 1.0f);
	}

	m_pTextures->loadAndBindTexture("environment/clouds.png");

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

	Matrix& matrix = MatrixStack::View.getTop();
	matrix.scale(Vec3(ss, 1.0f, ss));

	for (int pass = 0; pass < 2; pass++)
	{
		if (pass == 0)
		{
#if MCE_GFX_API_OGL
			glColorMask(false, false, false, false);
#endif
		}
		else
		{
#if MCE_GFX_API_OGL
			glColorMask(true, true, true, true);
#endif
		}

		for (int xPos = -radius + 1; xPos <= radius; xPos++)
		{
			for (int zPos = -radius + 1; zPos <= radius; zPos++)
			{
				t.begin(0);
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
					t.normal(Vec3::NEG_UNIT_X);

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
					t.normal(Vec3::UNIT_X);

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
					t.normal(Vec3::NEG_UNIT_Z);

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
					t.normal(Vec3::UNIT_Z);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + 0.0f), (yy + h), (zp + i + 1.0f - e), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + h), (zp + i + 1.0f - e), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + 0.0f), (zp + i + 1.0f - e), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + i + 1.0f - e), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
					}
				}

				t.draw(m_materials.clouds);
			}
		}
	}

	if (yy > 1.0f)
	{
		renderContext.setDepthRange(0.0f, 0.7f);
	}
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

		m_dirtyChunks.push_back(pChunk);
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
