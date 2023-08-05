/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Random.hpp"

class Level;

class Feature
{
public:
	virtual ~Feature();
	virtual bool place(Level*, Random*, int, int, int) = 0;
	virtual void init(float, float, float);
};

class TreeFeature : public Feature
{
public:
	bool place(Level*, Random*, int x, int y, int z) override;
};

class BirchFeature : public Feature
{
public:
	bool place(Level*, Random*, int x, int y, int z) override;
};

class SpruceFeature : public Feature
{
public:
	bool place(Level*, Random*, int x, int y, int z) override;
};

class PineFeature : public Feature
{
public:
	bool place(Level*, Random*, int x, int y, int z) override;
};

class FlowerFeature : public Feature
{
public:
	FlowerFeature(int id);
	bool place(Level*, Random*, int x, int y, int z) override;

private:
	int m_ID;
};

class SpringFeature : public Feature
{
public:
	SpringFeature(int id);
	bool place(Level*, Random*, int x, int y, int z) override;

private:
	int m_ID;
};

class ClayFeature : public Feature
{
public:
	ClayFeature(int id, int count);
	bool place(Level*, Random*, int x, int y, int z) override;

private:
	int m_ID;
	int m_count;
};

class OreFeature : public Feature
{
public:
	OreFeature(int id, int count);
	bool place(Level*, Random*, int x, int y, int z) override;

private:
	int m_ID;
	int m_count;
};

class ReedsFeature : public Feature
{
public:
	bool place(Level*, Random*, int x, int y, int z) override;
};

