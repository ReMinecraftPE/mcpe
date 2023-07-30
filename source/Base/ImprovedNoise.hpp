#pragma once

#include "Random.hpp"
#include "Synth.hpp"

// note: appears to maybe inherit from a class called Synth?
// the only purpose that it serves I guess is to provide
// a virtual getValue
class ImprovedNoise : public Synth
{
public:
	float getValue(float, float) override;

	ImprovedNoise();
	ImprovedNoise(Random* pRandom);

	void init(Random* pRandom);
	float getValue(float, float, float);
	float noise(float, float, float);
	float grad(int, float, float, float);
	float grad2(int, float, float);
	float lerp(float prog, float a, float b);
	float fade(float x); // inlined in the code
	void add(float* a2, float a3, float a4, float a5, int a6, int a7, int a8, float a9, float a10, float a11, float a12);

public:
	float m_offsetX;
	float m_offsetY;
	float m_offsetZ;
	int m_permutation[512];
};

