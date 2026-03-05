/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <stdexcept>
#include "RenderChunk.hpp"
#include "common/Util.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "client/renderer/renderer/Tesselator.hpp"
#include "client/renderer/RenderChunkBuilder.hpp"
#include "renderer/ShaderConstants.hpp"
#include "world/level/levelgen/chunk/ChunkSource.hpp"
#include "world/level/levelgen/chunk/ChunkViewSource.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"
#include "world/level/levelgen/chunk/Bounds.hpp"
#include "world/level/TileSource.hpp"
#include "world/Facing.hpp"

bool RenderChunk::_isUnderwater;
mce::MaterialPtr RenderChunk::fogMaterialMap[10];
mce::MaterialPtr RenderChunk::materialMap[10];
mce::MaterialPtr RenderChunk::fadingSeasonsAlphaChunkMaterial;
mce::MaterialPtr RenderChunk::fadingSeasonsChunksMaterial;
mce::MaterialPtr RenderChunk::fadingChunksMaterial;

void RenderChunk::_init()
{
	m_unk296 = 0.0;
}

void RenderChunk::_init(RenderChunk& other)
{
    _move(other);
    _init();
}

RenderChunk::RenderChunk(const TilePos& pos, mce::Mesh& mesh)
	: m_pos(pos)
	, m_mesh(mesh)
{
	_init();
}

const mce::MaterialPtr& RenderChunk::_chooseMaterial(TerrainLayer layer, double a, bool fog)
{
	assert(layer >= TERRAIN_LAYERS_MIN && layer <= TERRAIN_LAYERS_MAX);
    
	double diff = a - m_unk296;
    
	if (diff < 1.2)
	{
		currentShaderColor.r = (1.2 - diff) * 0.2;
		switch (layer)
		{
            case TERRAIN_LAYER_SEASONS_FAR:
                return fadingSeasonsChunksMaterial;
            case TERRAIN_LAYER_SEASONS_FAR_ALPHATEST:
                return fadingSeasonsAlphaChunkMaterial;
            default:
                return fadingChunksMaterial;
		}
	}
    
	mce::MaterialPtr* map;
	if (fog) 
		map = fogMaterialMap;
	else
		map = materialMap;
    
	return map[layer] ? map[layer] : materialMap[layer];
}

void RenderChunk::_move(RenderChunk& other)
{
    m_pos = other.m_pos;
    m_mesh = other.m_mesh;
}

void RenderChunk::rebuild(bool transparentLeaves)
{
	m_builder->build(*this, transparentLeaves);
}

void RenderChunk::startRebuild(std::unique_ptr<RenderChunkBuilder>& builder)
{
	m_builder = std::move(builder);

	m_builder->m_localSource->move(m_pos - 1, m_pos + ChunkConstants::MESH_XYZ_SIZE + 1);

	m_builder->m_useVisibilityExtimator = m_unk199;
	m_unk199 = false;

	m_builder->m_visibilityNode = m_visibilityNode;

	_changeState(BS_UNK0, BS_UNK1);
}

void RenderChunk::endRebuild()
{
	m_builder->m_localSource->clear();

	if (isRebuildState(BS_UNK1))
	{
		Tesselator* tessellator = m_builder->m_tessellator;
		if (tessellator->isTesselating())
		{
			m_mesh = tessellator->end();
			tessellator->setOffset(0.0f, 0.0f, 0.0f);
			m_builder->trim();
		}
		else
		{
			m_mesh.reset();
		}

		m_unk284 = m_builder->m_unk12;
		m_unk288 = m_builder->m_unk9;
		m_unk280 = m_builder->m_unk10;

		// TODO
	}
	else if (isRebuildState(BS_UNK2))
	{
		m_builder->m_tessellator->clear();
		m_unk199 |= m_builder->m_useVisibilityExtimator;
		_changeState(BS_UNK2, BS_UNK0);
	}
}

void RenderChunk::reset()
{
	m_unk272 = false;
	m_unk184 = false;
	m_unk280 = false;
	m_unk296 = 0.0;
	m_mesh.reset();
}

void RenderChunk::makeReadyAsEmpty()
{
	m_mesh.reset();
	m_unk280 = true;
	m_unk288 = false;
	m_unk284 = 15.0f;
	m_visibilityNode = VisibilityNode(true);
	m_unk184 = true;
	_changeState(BS_UNK0, BS_UNK3);
}

void RenderChunk::setPos(const TilePos& pos)
{
	if (m_pos == pos)
		return;

	reset();

	const int xzCenter = ChunkConstants::XZ_SIZE / 2;

	m_pos = pos;
	m_centerPos = pos + xzCenter;

	m_aabb = AABB(m_pos, static_cast<float>(ChunkConstants::XZ_SIZE));

	for (int i = 0; i < Facing::COUNT; i++)
		m_neighborPositions[i] = m_centerPos + (Facing::DIRECTION[i] * xzCenter);

	TilePos min = (pos - 1) * TilePos(1, 0, 1);
	TilePos max = (pos + 1 + ChunkConstants::XZ_SIZE) * TilePos(1, 0, 1);
	m_bounds = Bounds(min, max, ChunkConstants::XZ_SIZE, false);
}

TilePos RenderChunk::getFacing(int i) const
{
	return m_centerPos + (Facing::DIRECTION[i] * ChunkConstants::XZ_SIZE);
}

RenderChunk::DataState RenderChunk::getDataState(TileSource& source)
{
	Bounds::Iterator iter = m_bounds.begin();

	while (!iter.end())
	{
		const Vec3Int32& pos = iter.pos();

		LevelChunk* chunk = source.getChunkSource().getExistingChunk(ChunkPos(pos.x, pos.z));
		if (!chunk)
			return DS_WONT_LOAD;

		if (chunk->getState() <= CS_POST_PROCESSING)
			return DS_LOADING;

		iter++;
	}

	return DS_LOADED;
}

bool RenderChunk::isRebuildState(State state) const
{
	return m_state.load(std::memory_order_acquire) == state;
}

bool RenderChunk::isDirty() const
{
	return isRebuildState(BS_UNK0);
}

bool RenderChunk::setDirty(bool dirty)
{
	if (!isRebuildState(BS_UNK0) || _tryChangeState(BS_UNK4, BS_UNK0) || _tryChangeState(BS_UNK3, BS_UNK0))
	{
		m_dirty |= dirty;
		return true;
	}

	_tryChangeState(BS_UNK1, BS_UNK2);
	return false;
}

bool RenderChunk::isPending() const
{
	return isRebuildState(BS_UNK0) || isRebuildState(BS_UNK1) || isRebuildState(BS_UNK2);
}

bool RenderChunk::hasImmediateChange() const
{
	return m_dirty;
}

float RenderChunk::distanceToSqr(const Entity* entity)
{
	return entity->m_pos.distanceToSqr(m_centerPos);
}

void RenderChunk::updateDistanceFromPlayer(const Vec3& playerPos)
{
	m_distanceFromPlayerSquared = Vec3(m_centerPos).distanceToSqr(playerPos);
}

void RenderChunk::render(TerrainLayer layer, double a, bool fog)
{
	currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color::WHITE;
	m_mesh.render(_chooseMaterial(layer, a, fog));
}

void RenderChunk::_InitLayers(mce::MaterialPtr* materials, const std::string& suffix)
{
	materials[TERRAIN_LAYER_OPAQUE]                = GET_MATERIAL_PTR(switchable, "terrain_opaque"            + suffix);
	materials[TERRAIN_LAYER_OPAQUE_SEASONS]        = GET_MATERIAL_PTR(switchable, "terrain_opaque_seasons"    + suffix);
	materials[TERRAIN_LAYER_BLEND]                 = GET_MATERIAL_PTR(switchable, "terrain_blend"             + suffix);
	materials[TERRAIN_LAYER_ALPHATEST]             = GET_MATERIAL_PTR(switchable, "terrain_alpha"             + suffix);
	materials[TERRAIN_LAYER_ALPHATEST_SINGLE_SIDE] = GET_MATERIAL_PTR(switchable, "terrain_alpha_single_side" + suffix);
	materials[TERRAIN_LAYER_ALPHATEST_SEASONS]     = GET_MATERIAL_PTR(switchable, "terrain_alpha_seasons"     + suffix);
	materials[TERRAIN_LAYER_DOUBLE_SIDED]          = GET_MATERIAL_PTR(switchable, "terrain_doubleside"        + suffix);
	materials[TERRAIN_LAYER_FAR]                   = GET_MATERIAL_PTR(switchable, "terrain_far"               + suffix);
	materials[TERRAIN_LAYER_SEASONS_FAR]           = GET_MATERIAL_PTR(switchable, "terrain_seasons_far"       + suffix);
	materials[TERRAIN_LAYER_SEASONS_FAR_ALPHATEST] = GET_MATERIAL_PTR(switchable, "terrain_seasons_far_alpha" + suffix);
}

void RenderChunk::InitMaterials()
{
	fadingChunksMaterial            = GET_MATERIAL_PTR(switchable, "terrain_fading_in");
	fadingSeasonsChunksMaterial     = GET_MATERIAL_PTR(switchable, "terrain_seasons_fading_in");
	fadingSeasonsAlphaChunkMaterial = GET_MATERIAL_PTR(switchable, "terrain_seasons_fading_in_alpha");

	_InitLayers(materialMap, Util::EMPTY_STRING);
	_InitLayers(fogMaterialMap, "_fog");
}

void RenderChunk::SetUnderwater(bool isUnderwater)
{
	if (_isUnderwater == isUnderwater)
		return;

	_isUnderwater = isUnderwater;

	std::string materialName = isUnderwater ? "terrain_blend_below" : "terrain_blend";
	materialMap[TERRAIN_LAYER_BLEND] = GET_MATERIAL_PTR(switchable, materialName);

	materialName = isUnderwater ? "terrain_blend_below_fog" : "terrain_blend_fog";
	fogMaterialMap[TERRAIN_LAYER_BLEND] = GET_MATERIAL_PTR(switchable, materialName);
}
