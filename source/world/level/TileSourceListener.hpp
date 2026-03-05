#pragma once
#include "world/tile/Tile.hpp"

class TileSource;
class TileEntity;

class TileSourceListener
{
public:
	virtual ~TileSourceListener() {}

public:
	void onSourceCreated(TileSource* source) {}
	void onSourceDestroyed(TileSource* source) {}
	void onTilesDirty(TileSource* source, int, int, int, int, int, int) {}
	void onAreaChanged(TileSource& source, const TilePos&, const TilePos&) {}
	void onTileChanged(TileSource* source, const TilePos&, FullTile, FullTile, int) {}
	void onBrightnessChanged(TileSource& source, const TilePos& pos) { onAreaChanged(source, pos, pos); }
	void onTileEntityChanged(TileSource& source, TileEntity& tileEntity) {}
	void onTileEntityRemoved(TileSource& source, std::unique_ptr<TileEntity>& tileEntity) {}
	void onTileEvent(TileSource* source, int, int, int, int, int) {}
};
