#pragma once
#include "client/renderer/VisibilityExtimator.hpp"

class ChunkSource;
class ChunkViewSource;
class TileRenderer;
class Tesselator;
class TileSource;
class RenderChunk;
struct TileQueue;

struct RangeIndices
{
	int m_start;
	int m_count;
};

class RenderChunkBuilder
{
	friend class RenderChunk;

private:
	ChunkViewSource* m_localSource;
	TileRenderer* m_tileTessellator;
	bool m_useVisibilityExtimator;
	bool m_unk9;
	bool m_unk10;
	float m_unk12;
	VisibilityNode m_visibilityNode;
	RangeIndices m_renderLayerRanges[10];
	TileQueue* m_tileQueue;
	Tesselator* m_tessellator;

public:
	RenderChunkBuilder(ChunkSource& mainSource, Tesselator* tessellator);
	~RenderChunkBuilder();

public:
	void build(RenderChunk& renderChunk, bool transparentLeaves);
	void trim();

private:
	VisibilityExtimator* _getVisibilityHelper();
	void _initializeRebuildData(TileSource& region, RenderChunk& renderChunk);
	bool _seesSkyDirectly(RenderChunk& renderChunk, TileSource& region);
	void _checkPropagatedBrightness(TileSource& source, const TilePos& pos);
	void _checkAllDark(TileSource& source, const TilePos& pos);
	bool _sortTiles(TileSource& region, RenderChunk& renderChunk, VisibilityExtimator* visibilityHelper, bool transparentLeaves);
	bool _tessellateQueues(RenderChunk& renderChunk, TileSource& region);
	void _buildRanges();
};
