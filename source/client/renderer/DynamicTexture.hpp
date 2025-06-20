/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Textures.hpp"
#include "common/Random.hpp"
class Textures; // in case we are being included from Textures. We don't need a complete type

// Essentially, the way these work is by patching themselves into terrain.png with a
// call to glTexSubImage2D

class DynamicTexture
{
public:
	virtual void tick() = 0;
	virtual bool bindTexture(Textures*);

	DynamicTexture(int a2);
	virtual ~DynamicTexture();

protected:
	int m_textureIndex;
	int m_textureSize;
	uint8_t m_pixels[1024];

	friend class Textures;
};

class WaterTexture : public DynamicTexture
{
public:
	WaterTexture();
	~WaterTexture();

	void tick() override;

public:
	int field_40C;
	int field_410;
	float* m_data1;
	float* m_data2;
	float* m_data3;
	float* m_data4;
};

class WaterSideTexture : public DynamicTexture
{
public:
	WaterSideTexture();
	~WaterSideTexture();

	void tick() override;

public:
	int field_40C;
	int field_410;
	int field_414;
	float* m_data1;
	float* m_data2;
	float* m_data3;
	float* m_data4;
};

class LavaTexture : public DynamicTexture
{
public:
	LavaTexture();
	~LavaTexture();

	void tick() override;

public:
	int field_14;
	int field_18;
	float* m_data1;
	float* m_data2;
	float* m_data3;
	float* m_data4;
};

class LavaSideTexture : public DynamicTexture
{
public:
	LavaSideTexture();
	~LavaSideTexture();

	void tick() override;

public:
	int field_14;
	int field_18;
	int field_1C;
	float* m_data1;
	float* m_data2;
	float* m_data3;
	float* m_data4;
};

class FireTexture : public DynamicTexture
{
public:
	FireTexture(int);
	~FireTexture();

	void tick() override;

public:
	float* m_data1;
	float* m_data2;
	Random m_random;
};
