#pragma once
#include "Dimension.hpp"
#include "TestChunkSource.hpp"
#include "RandomLevelSource.hpp"
#include "ChunkCache.hpp"

class LargeFeatureVtable;


class LargeFeature
{
public:
	
	int field_4;
	Random* m_random;
	LargeFeatureVtable* vtable;
	BYTE gap10[2496];
	LargeFeature();
	~LargeFeature();
	unsigned int apply(ChunkSource* a2, Level* a3, int a4, int a5, unsigned __int8* a6, int a7);
};
class LargeFeatureVtable
{
public:
	int dtor1;
	int dtor0;
	int(__fastcall* apply)(LargeFeature*, ChunkSource*, Level*, int, int, uint8_t*, int);
	int(__fastcall* field_C)(LargeFeature*, Level*, int, int, int, int, uint8_t*, int);


};

LargeFeature::LargeFeature() {

	Random* v2; // r4
	int v3; // r0
	int v4; // r1
	int v5; // r3
	v2 = m_random;
	v3 = getTimeMs();
	v4 = 1;
	v5 = 1;
	vtable->dtor0 = 8;

	m_random->mti = 1;
	m_random->rseed = v3;
	m_random->mt[0] = v3;
	do
	{
		v2->mt[v5] = v5 + 1812433253 * (*(&v2->rseed + v5) ^ (*(&v2->rseed + v5) >> 30));
		v5 = ++v4;
	} while (v4 <= 623);
	m_random->mti = v5;



};
LargeFeature::~LargeFeature()
{
	// ... (implementation of the destructor)
	delete this;
}
struct LargeCaveFeature
{
	LargeFeature x[314];
	unsigned int addFeature(Level* a2, int a3, int a4, int a5, int a6, unsigned __int8* a7, int a8);

	int addroom(int a2, int a3, unsigned __int8* a4, float a5, float a6, float a7);
	unsigned int __fastcall addTunnel(int a2, int a3, unsigned __int8* a4, float a5, float a6, float a7, float a8, float a9, float a10, int a11, int a12, float a13);
};
