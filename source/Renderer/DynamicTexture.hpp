/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Textures.hpp"
class Textures; // in case we are being included from Textures. We don't need a complete type

// Essentially, the way these work is by patching themselves into terrain.png with a
// call to glTexSubImage2D

class DynamicTexture
{
public:
	virtual void tick() = 0;
	virtual void bindTexture(Textures*);

	DynamicTexture(int a2);
	virtual ~DynamicTexture();

protected:
	int m_textureIndex = 0;
	int m_textureSize = 1;
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
	int field_40C = 0;
	int field_410 = 0;
	float* m_data1 = nullptr;
	float* m_data2 = nullptr;
	float* m_data3 = nullptr;
	float* m_data4 = nullptr;
};

class WaterSideTexture : public DynamicTexture
{
public:
	WaterSideTexture();
	~WaterSideTexture();

	void tick() override;

public:
	int field_40C = 0;
	int field_410 = 0;
	int field_414 = 0;
	float* m_data1 = nullptr;
	float* m_data2 = nullptr;
	float* m_data3 = nullptr;
	float* m_data4 = nullptr;
};
