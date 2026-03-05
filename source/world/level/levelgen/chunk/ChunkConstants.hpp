#pragma once
#include "compat/LegacyCPP.hpp"

class ChunkConstants
{
public:
	// The size of a chunk in the X and Z directions
	static constexpr int XZ_SIZE = 16;

	// The size of a chunk in the Y direction
	static constexpr int Y_SIZE = 128;

	// Number of tiles per row of a chunk
	static constexpr int TILE_COUNT_PER_ROW = XZ_SIZE * XZ_SIZE;

	// Number of tiles in a whole chunk
	static constexpr int TILE_COUNT = TILE_COUNT_PER_ROW * Y_SIZE;

	// The size of a chunk mesh in all axis
	static constexpr int MESH_XYZ_SIZE = XZ_SIZE;

	// Number of tiles per row in a chunk mesh
	static constexpr int MESH_TILE_COUNT_PER_ROW = MESH_XYZ_SIZE * MESH_XYZ_SIZE;

	// Number of tiles in a chunk mesh
	static constexpr int MESH_TILE_COUNT = MESH_XYZ_SIZE * MESH_XYZ_SIZE * MESH_XYZ_SIZE;

	// How many meshes each chunk should be split by their Y-axis.
	static constexpr int MESH_COUNT = TILE_COUNT / MESH_TILE_COUNT;
};
