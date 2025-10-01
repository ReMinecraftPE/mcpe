/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Random.hpp"
#include "world/level/TilePos.hpp"

class Level;

class Feature
{
public:
	virtual ~Feature();
	virtual bool place(Level*, Random*, const TilePos& pos) = 0;
	virtual void init(float, float, float);
};

class TreeFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class BirchFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class SpruceFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class PineFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class FlowerFeature : public Feature
{
public:
	FlowerFeature(TileID id);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	TileID m_ID;
};

class SpringFeature : public Feature
{
public:
	SpringFeature(TileID id);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	TileID m_ID;
};

class ClayFeature : public Feature
{
public:
	ClayFeature(TileID id, int count);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	TileID m_ID;
	int m_count;
};

class OreFeature : public Feature
{
public:
	OreFeature(TileID id, int count);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	TileID m_ID;
	int m_count;
};

class ReedsFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class VegetationFeature : public Feature
{
public:
	VegetationFeature(TileID id, TileData data, int count = 128);
	bool place(Level*, Random*, const TilePos& pos) override;

private:
	TileID m_ID;
	TileData m_data;
	int m_count;
};

class CactusFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};

class PumpkinFeature : public Feature
{
public:
	bool place(Level*, Random*, const TilePos& pos) override;
};


