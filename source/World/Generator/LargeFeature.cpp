#include "Largefeature.h"
#include "../source/World/Level.hpp"
#include "../source/App/Minecraft.hpp"
unsigned int LargeFeature::apply(ChunkSource* a2, Level* a3, int a4, int a5, unsigned __int8* a6, int a7) {
	int v9; // r5
	int v10; // r0
	int v11; // r3
	int v12; // r2
	unsigned int result; // r0
	int v14; // r9
	int v15; // r8
	int v16; // r0
	unsigned long* v17; // r1
	int v18; // r3
	int v19; // r2
	unsigned int v20; // r0
	LargeFeatureVtable** v21; // r6
	unsigned int v22; // r0
	int v23; // [sp+14h] [bp-4Ch]
	int v24; // [sp+1Ch] [bp-44h]
	unsigned int v26; // [sp+24h] [bp-3Ch]
	int v27; // [sp+28h] [bp-38h]
	unsigned int v28; // [sp+30h] [bp-30h]
	v9 = field_4;
	v10 = a3->getSeed();
	v11 = 1;
	v12 = 1;
	m_random->mti = 1;
	m_random->rseed = v10;
	m_random->mt[0] = v10;
	
	do
	{
		++v11;
		m_random->mt[v12] = v12
			+ 1812433253 * (*(&m_random->rseed + v12) ^ (*(&m_random->rseed + v12) >> 30));
		v12 = v11;
	} while (v11 <= 623);
	m_random->mti = v11;
	v28 = 2 * (m_random->genrand_int32() >> 2) + 1;
	result = m_random->genrand_int32();
	v23 = a4 - v9;
	v26 = 2 * (result >> 2) + 1;
	if (a4 - v9 <= a4 + v9) {
		v24 = a5 + v9;
		v27 = (a5 - v9) * v26 + v23 * v28;
		do
		{
			if (a5 - v9 <= v24) {
				v14 = v27;
				v15 = a5 - v9;
				do {
					v16 = a3->getSeed();
					m_random->mti = 1;
					v17 = &m_random->mt[1];
					v18 = 2;
					v19 = 1;
					
					v20 = v16 ^ v14;
					m_random->rseed = v20;
					m_random->mt[0] = v20;
					while (1)
					{
						v21 = &vtable;
					
						++v17; //just fix this
						
						v22 = v19 + 1812433253 * (v20 ^ (v20 >> 30));
						v19 = v18;
						v21[3] = (LargeFeatureVtable*)v22; //fix that variable if wrong
						if (v18 > 623) 
							break;
						v20 = *(v17 - 1); // too
						++v18;
					}
					m_random->mti = v18;
					result = vtable->field_C(this, a3, v23, v15++, a4, a5, a6, a7); 
					v14 += v26;
					
				}
				while (v15 <= v24);
			}
			v27 += v28;
			++v23;
		}
		while (v23 <= a4 + v9);
	}
	return result;

}
unsigned int __fastcall LargeCaveFeature::addTunnel(int a2, int a3, unsigned __int8* a4, float a5, float a6, float a7, float a8, float a9, float a10, int a11, int a12, float a13) {
	int v18; // s28
	unsigned int v21; // r0
	int v22; // r1
	float v23; // s27
	float v24; // s28
	uint32_t v25; // r2
	unsigned long* v26; // r0
	int i; // r3
	uint32_t* v28; // r5
	uint32_t v29; // r2
	unsigned int v30; // r5
	unsigned int v31; // r1
	int v32; // r3
	unsigned int v33; // r3
	unsigned int result; // r0
	float v35; // s23
	float v36; // s24
	float v37; // s25
	float v38; // s17
	float v39; // s21
	float v40; // s19
	float v41; // s17
	float v42; // r0
	int v43; // r7
	int v44; // r5
	int v45; // r6
	float v46; // r0
	int v47; // r11
	float v48; // r0
	int v49; // r5
	int v50; // s15
	int v51; // r9
	float v52; // s12
	float v53; // s13
	float v54; // s12
	float v55; // s13
	float v56; // s12
	float v57; // s13
	float v58; // s15
	float v59; // s21
	float v60; // s15
	float v61; // s15
	int v62; // r6
	int v63; // r0
	int v64; // r5
	int v65; // r1
	unsigned int v66; // r3
	int v67; // r10
	int v68; // r1
	float v69; // s15
	float v70; // s8
	unsigned __int8* v71; // r12
	int v72; // r4
	float v73; // s15
	float v74; // s9
	int v75; // r3
	unsigned __int8* v76; // r2
	int v77; // r6
	float v78; // s15
	int v79; // r0
	int v80; // r7
	bool v81; // zf
	int v82; // r2
	BOOL v83; // r2
	BOOL v84; // r9
	BOOL v85; // r9
	int v86; // r5
	unsigned int v87; // r0
	int v88; // r5
	int v89; // r1
	int v90; // [sp+40h] [bp-AB0h]
	int v91; // [sp+44h] [bp-AACh]
	int v92; // [sp+48h] [bp-AA8h]
	unsigned int v93; // [sp+4Ch] [bp-AA4h]
	unsigned int v94; // [sp+50h] [bp-AA0h]
	unsigned int v95; // [sp+54h] [bp-A9Ch]
	unsigned int v96; // [sp+58h] [bp-A98h]
	unsigned int v97; // [sp+5Ch] [bp-A94h]
	unsigned int v98; // [sp+60h] [bp-A90h]
	signed int v99; // [sp+64h] [bp-A8Ch]
	int v100; // [sp+64h] [bp-A8Ch]
	unsigned int v101; // [sp+68h] [bp-A88h]
	unsigned __int8* v102; // [sp+6Ch] [bp-A84h]
	int v103; // [sp+70h] [bp-A80h]
	int v104; // [sp+74h] [bp-A7Ch]
	unsigned int v105; // [sp+7Ch] [bp-A74h]
	int v106; // [sp+88h] [bp-A68h]
	int v107; // [sp+88h] [bp-A68h]
	unsigned __int8* v108; // [sp+88h] [bp-A68h]
	int v109; // [sp+90h] [bp-A60h]
	int v111; // [sp+98h] [bp-A58h]
	int v113; // [sp+A0h] [bp-A50h]
	int v114; // [sp+A8h] [bp-A48h]
	Random v115; // [sp+C0h] [bp-A30h] BYREF
	v111 = a2;
	v18 = 16 * a3 + 8;
	v21 = x[0].m_random->genrand_int32();
	v22 = 1;
	v23 = (float)(16 * a2 + 8);
	v24 = (float)v18;
	v25 = v21 >> 1;
	v26 = &v115.mt[1];
	v115.rseed = v25;
	v115.mt[0] = v25;
	v115.mti = 1;
	for (i = 2; ; ++i)
	{
		v28 = &v115.rseed + v22;
		++v26;
		v29 = v22 + 1812433253 * (v25 ^ (v25 >> 30));
		v22 = (int)i;
		v28[1] = v29;
		if ((int)i > 623)
			break;
		v25 = *(v26 - 1);
	}
	v115.mti = (int)i;
	if (a12 <= 0)
	{
		v86 = this->x[0].field_4;
		v87 = v115.genrand_int32();
		v88 = 16 * (v86 - 1);
		v89 = (v88 + 3) & (v88 >> 31);
		if (v88 >= 0)
			v89 = v88;
		a12 = v88 - v87 % (v89 >> 2);
	}
	if (a11 == -1)
	{
		v103 = 1;
		v30 = (unsigned int)a12 >> 31;
		a11 = a12 / 2;
	}
	else
	{
		v103 = 0;
		v30 = (unsigned int)a12 >> 31;
	}
	v31 = v115.genrand_int32() % ((int)(v30 + a12) >> 1);
	v32 = (a12 + 3) & (a12 >> 31);
	if (a12 >= 0)
		v32 = a12;
	v105 = v31 + (v32 >> 2);
	v33 = v115.genrand_int32() % 6;
	result = a11;
	v101 = v33;
	if (a11 < a12)
	{
		v35 = 0.0;
		v36 = 0.0;
		v114 = -16 * a3 + 1;
		v91 = a12 - a11;
		while (1)
		{
			v37 = sinf((float)((float)a11 * 3.1416) / (float)a12);
			v38 = cosf(a10);
			a6 = a6 + sinf(a10);
			a5 = a5 + (float)(v38 * cosf(a9));
			a7 = a7 + (float)(v38 * sinf(a9));
			if (v101)
				v39 = a10 * 0.7;
			else
				v39 = a10 * 0.92;
			a10 = v39 + (float)(v36 * 0.1);
			a9 = a9 + (float)(v35 * 0.1);
			v98 = v115.genrand_int32();
			v97 = v115.genrand_int32();
			v96 = v115.genrand_int32();
			v95 = v115.genrand_int32();
			v94 = v115.genrand_int32();
			result = v115.genrand_int32();
			v93 = result;
			if (v103)
				goto LABEL_30;
			if (a11 == v105 && a8 > 1.0) {
				v59 = a10 / 3.0;
				v60 = (double)v115.genrand_int32() * 2.32830644e-10;
				LargeCaveFeature::addTunnel(v111, a3, a4, a5, a6, a7, (float)(v60 * 0.5) + 0.5, a9 + (float)(3.1416 * 0.5), v59, a11, a12, 1.0);
				v61 = (double)v115.genrand_int32() * 2.32830644e-10;
				return LargeCaveFeature::addTunnel(
					v111,
					a3,
					a4,
					a5,
					a6,
					a7,
					(float)(v61 * 0.5) + 0.5,
					a9 + (float)(3.1416 * 0.5),
					v59,
					a11,
					a12,
					1.0);
			}
			result = v115.genrand_int32();
			if (result << 30) {
			LABEL_30:
				if ((float)((float)((float)((float)(a7 - v24) * (float)(a7 - v24))
					+ (float)((float)(a5 - v23) * (float)(a5 - v23)))
					- (float)((float)v91 * (float)v91)) > (float)((float)((float)(a8 + 2.0) + 16.0)
						* (float)((float)(a8 + 2.0) + 16.0))) return result;
				v40 = (float)(a8 * v37) + 1.5;
				if ((float)((float)(v23 - 16.0) + (float)(v40 * -2.0)) <= a5
					&& (float)((float)(v40 * -2.0) + (float)(v24 - 16.0)) <= a7
					&& (float)((float)(v23 + 16.0) + (float)(v40 + v40)) >= a5
					&& (float)((float)(v40 + v40) + (float)(v24 + 16.0)) >= a7) {
					v41 = v40 * a13;
					v42 = floorf(a5 - v40);
					v43 = ((int)v42 + -16 * v111 - 1) & ~(((int)v42 + -16 * v111 - 1) >> 31);
					v44 = (int)floorf(a5 + v40) + -16 * v111 + 1;
					if (v44 >= 16)
						v44 = 16;
					v106 = (int)floorf(a6 - v41) - 1;
					v45 = (int)floorf(a6 + v41) + 1;
					if (v45 >= 120)
						v45 = 120;
					v46 = floorf(a7 - v40);
					v47 = ((int)v46 + -16 * a3 - 1) & ~(((int)v46 + -16 * a3 - 1) >> 31);
					v48 = floorf(a7 + v40);
					v99 = v44;
					v49 = v106;
					v104 = v45;
					if (v106 < 1)
						v49 = 1;
					v107 = v49;
					v50 = (int)v48;
					result = v99;
					v51 = v50 + v114;
					if (v50 + v114 >= 16)
						v51 = 16;
					if (v50 + v114 >= 16) {
						v51 = 16;
					}
					if (v99 > v43) {
						v62 = v43;
						v63 = v49 - 1;
						v92 = (16 * v43 + v47) << 7;
						do {
							if (v47 < v51) {
								v64 = v92;
								v65 = v47;
								v90 = v51 - 1; 
								do {
									v66 = v104 + 1;
									if (v63 <= v104 + 1) {
										v67 = v51;
										do {
											if (v66 <= 0x7F) {
												v82 = a4[v66 + v64];
												if (v82 == Tile::water->m_ID)
													v83 = 1;
												else
													v83 = v82 == Tile::calmWater->m_ID;
												if (v63 == v66)
													v84 = 0;
												else
													v84 = v62 != v43;
												if (v84 && (v47 != v65 ? (v85 = v99 - 1 != v62) : (v85 = 0), v85 && v90 != v65)) {
													v66 = v107 - 1;
													if (v83)
														goto LABEL_31;

												}
												else {
													--v66;
													if (v83)
														goto LABEL_31;
												}
											}
											else
											{
												--v66;
											}
											

										}
										while (v63 <= (int)v66);
										v51 = v67;
										
									}
									++v65;
									v64 += 128;
								}
								while (v65 != v51);
							}
							++v62;
							v92 += 2048;
						}
						while (v62 != v99);
						v102 = &a4[128 * (16 * v43 + v47) + v104];
						v100 = 0;
						v68 = v107;
						v109 = v62 - v43;
						v113 = v43 + 16 * v111;
						v108 = a4;
						do {
							if (v47 < v51) {
								v69 = (float)((float)((float)(v113 + v100) + 0.5) - a5) / v40;
								v70 = v69 * v69;
								v71 = v102;
								v72 = 0;
								do {
									v73 = (float)((float)((float)(16 * a3 + v47 + v72) + 0.5) - a7) / v40;
									v74 = v73 * v73;
									if ((float)(v70 + (float)(v73 * v73)) < 1.0) {
										v75 = v104 - 1;
										if (v68 <= v104 - 1) {
											v76 = v71;
											v77 = 0; 
											do {
												v78 = (float)((float)((float)v75 + 0.5) - a6) / v41;
												if (v78 > -0.7 && (float)(v74 + (float)(v70 + (float)(v78 * v78))) < 1.0) {
													v79 = *v76;
													v80 = Tile::grass->m_ID;
													if (v80 == v79)
														v77 = 1;
													if (Tile::rock->m_ID == v79 || Tile::dirt->m_ID == v79 || v79 == v80) {
														if (v75 > 9)
														{
															*v76 = 0;
															if (v77)
															{
																if (*(v76 - 1) == Tile::dirt->m_ID)
																	*(v76 - 1) = Tile::grass->m_ID;
															}
														}
														else
														{
															*v76 = Tile::lava->m_ID;
														}
													}
												}
												--v75;
												--v76;
											}
											while (v68 <= v75);
										}
									}
									++v72;
									v71 += 128;
								}
								while (v72 != v51 - v47);
							}
							result = v100 + 1;
							v81 = ++v100 == v109;
							v102 += 2048;
						}
						while (!v81);
						a4 = v108;
					}
					if (v103)
						return result;
				}

			}
		LABEL_31:
			result = v91 - 1;
			++a11;
			--v91;
			if (a11 == a12)
				return result;
			v52 = (double)v98 * 2.32830644e-10;
			v53 = (double)v97 * 2.32830644e-10;
			v54 = v52 - v53;
			v55 = (double)v96 * 2.32830644e-10;
			v36 = (float)((float)(v54 * v55) + (float)(v54 * v55)) + (float)(v36 * 0.9);
			v56 = (double)v95 * 2.32830644e-10;
			v57 = (double)v94 * 2.32830644e-10;
			v58 = (double)v93 * 2.32830644e-10;
			v35 = (float)((float)((float)(v56 - v57) * v58) * 4.0) + (float)(v35 * 0.75);
		}
	}
	return result;
}
int LargeCaveFeature::addroom(int a2, int a3, unsigned __int8* a4, float a5, float a6, float a7) {
	float v11;
	v11 = (double)x->m_random->genrand_int32() * 2.32830644e-10;
	return LargeCaveFeature::addTunnel(a2, a3, a4, a5, a6, a7, (float)(v11 * 6.0) + 1.0, 0.0, 0.0, -1, -1, 0.5);
}
unsigned int LargeCaveFeature::addFeature(Level* a2, int a3, int a4, int a5, int a6, unsigned __int8* a7, int a8) {
	Random* v8; // r4
	unsigned int v10; // r7
	unsigned int result; // r0
	unsigned int v12; // r1
	float v13; // s18
	unsigned int v14; // r5
	float v15; // s19
	float v16; // s20
	signed int v17; // r7
	signed int v18; // r5
	unsigned int v19; // s25
	unsigned int v20; // s26
	float v21; // s15
	float v22; // s14
	float v23; // s15
	float v24; // s15
	float v25; // [sp+Ch] [bp-8Ch]
	float v26; // [sp+10h] [bp-88h]
	unsigned int v27; // [sp+2Ch] [bp-6Ch]
	int v28; // [sp+2Ch] [bp-6Ch]
	unsigned int v29; // [sp+30h] [bp-68h]
	int v30; // [sp+30h] [bp-68h]
	v8 = x->m_random;
	v10 = x->m_random->genrand_int32();
	v29 = v8->genrand_int32();
	v27 = v8->genrand_int32();
	result = v8->genrand_int32();
	if (!(result % 0xF)) {
		v12 = v29 % (v10 % 0x28 + 1) + 1;
		result = v27 / v12;
		v30 = v27 % v12;
		if ((int)(v27 % v12) > 0) {
			v28 = 0;
			do {
				v13 = (float)(int)((v8->genrand_int32() & 0xF) + 16 * a3);
				v14 = v8->genrand_int32();
				v15 = (float)(int)(v8->genrand_int32() % (v14 + 8 * (v14 / 0x78) - ((v14 / 0x78) << 7) + 8));
				v16 = (float)(int)((v8->genrand_int32() & 0xF) + 16 * a4);
				if (v8->genrand_int32() << 30) {
					v17 = 1;
				}
				else {
					addroom(a5, a6, a7, v13, v15, v16);
					v17 = (v8->genrand_int32() & 3) + 1;
				}
				v18 = 0;
				do {
					++v18;
					v19 = v8->genrand_int32();
					v20 = v8->genrand_int32();
					v21 = (double)v8->genrand_int32() * 2.32830644e-10;
					v22 = (double)v8->genrand_int32() * 2.32830644e-10;
					v25 = (float)(v21 + v21) + v22;
					v23 = (double)v19 * 2.32830644e-10;
					v26 = (float)(3.1416 * v23) + (float)(3.1416 * v23);
					v24 = (double)v20 * 2.32830644e-10;
					result = LargeCaveFeature::addTunnel(a5, a6, a7, v13, v15, v16, v25, v26, (float)((float)(v24 - 0.5) + (float)(v24 - 0.5)) * 0.125, 0, 0, 1.0);

				}
				while (v17 > v18);
				++v28;
			}
			while (v28 != v30);
		}
		return result;
	}

}