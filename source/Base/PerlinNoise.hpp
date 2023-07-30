/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ImprovedNoise.hpp"

class PerlinNoise : public Synth
{
public:
	PerlinNoise(int nOctaves);
	PerlinNoise(Random*, int nOctaves);
	virtual ~PerlinNoise();
	void init(int nOctaves);

	float getValue(float, float) override;
	float getValue(float, float, float);

	float* getRegion(float*, int, int, int, int, float, float, float);
	float* getRegion(float*, float, float , float, int, int, int, float, float, float);

private:
	ImprovedNoise** m_pImprovedNoise;
	int m_nOctaves;
	Random m_random;
	Random* m_pRandom; // random for seeding, I assume
};

