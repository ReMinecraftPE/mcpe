/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Random.hpp"
#include "common/Utils.hpp"
#include "world/level/TilePos.hpp"
#include <array>
#include <vector>
#include <memory>

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

class FancyTreeFeature : public Feature
{
public:
	~FancyTreeFeature() = default;
	void generateBranchesAndTrunk();
	void crossection(int x, int y, int z, float radius, uint8_t majorAxis, int blockId);
	float treeShape(int offset);
	float foliageShape(int layerOffset);
	void foliageCluster(int x, int y, int z);
	void limb(const TilePos& start, const TilePos& end, int blockId);
	bool trimBranches(int heightOffset);
	void makeTrunk();
	int checkLine(TilePos& startPos, TilePos& endPos);
	bool checkLocation();
	void init(float density, float widthScale, float foliageDensity) override;
	bool place(Level*, Random*, const TilePos& pos) override;
	
	// Helper methods for axis-based coordinate access
	static inline int& getAxisCoord(TilePos& pos, uint8_t axis)
	{
		switch (axis)
		{
			case 0: return pos.x;
			case 1: return pos.y;
			case 2: return pos.z;
			default: return pos.x;
		}
	}
	
	static inline int getAxisCoord(const TilePos& pos, uint8_t axis)
	{
		switch (axis)
		{
			case 0: return pos.x;
			case 1: return pos.y;
			case 2: return pos.z;
			default: return pos.x;
		}
	}
private:
	static const uint8_t axisConversionArray[6];
	Random m_rnd;
	Level* m_pLevel;
	TilePos m_origin = TilePos();
	int m_height = 0;
	int m_trunkHeight;
	double m_trunkHeightScale = 0.618;
	double m_branchDensity = 1.0;
	double m_branchSlope = 0.381;
	double m_widthScale = 1.0;
	double m_foliageDensity = 1.0;
	int m_trunkWidth = 1;
	int m_heightVariance = 12;
	int m_foliageHeight = 4;
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


