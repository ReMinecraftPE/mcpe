/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RenderChunk.hpp"
#include "common/Util.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"

bool RenderChunk::_isUnderwater;
mce::MaterialPtr RenderChunk::fogMaterialMap[10];
mce::MaterialPtr RenderChunk::materialMap[10];
mce::MaterialPtr RenderChunk::fadingSeasonsAlphaChunkMaterial;
mce::MaterialPtr RenderChunk::fadingSeasonsChunksMaterial;
mce::MaterialPtr RenderChunk::fadingChunksMaterial;

void RenderChunk::_init()
{
	m_lastRebuilt = 0.0;
}

const mce::MaterialPtr& RenderChunk::_chooseMaterial(TerrainLayer layer, double a, bool fog)
{
	if (layer < TERRAIN_LAYERS_MIN || layer > TERRAIN_LAYERS_MAX)
		throw std::out_of_range("Invalid TerrainLayer");

	double diff = a - m_lastRebuilt;

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
	if (fog) map = fogMaterialMap;
	else     map = materialMap;

	return map[layer];
}

RenderChunk::RenderChunk(const TilePos& pos, mce::Mesh& mesh)
	: m_pos(pos)
	, m_mesh(mesh)
{
	_init();
}

void RenderChunk::render(TerrainLayer layer, double a, bool fog)
{
	currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color::WHITE;
	m_mesh.render(_chooseMaterial(layer, a, fog));

}

void RenderChunk::reset()
{
	m_mesh.reset();
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