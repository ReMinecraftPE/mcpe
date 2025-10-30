/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/LegacyCPP.hpp"

class Material
{
public:
	Material();
	Material(bool bFlammable);
	virtual ~Material();

	static void initMaterials();
	static void teardownMaterials();

	virtual bool isLiquid() const;
	virtual bool letsWaterThrough() const;
	virtual bool isSolid() const;
	virtual bool blocksLight() const;
	virtual bool blocksMotion() const;
	virtual bool isFlammable() const;

public:
	static Material
		*air,
		*dirt,
		*wood,
		*stone,
		*metal,
		*water,
		*lava,
		*leaves,
		*plant,
		*sponge,
		*cloth,
		*fire,
		*sand,
		*decoration,
		*glass,
		*explosive,
		*coral,
		*ice,
		*topSnow,
		*snow,
		*cactus,
		*clay,
		*vegetable,
		*portal,
		*cake,
		*web;

public:
	bool m_bFlammable;
};

class GasMaterial : public Material
{
	bool isSolid() const override;
	bool blocksLight() const override;
	bool blocksMotion() const override;
};

class LiquidMaterial : public Material
{
	bool isLiquid() const override;
	bool isSolid() const override;
	bool blocksMotion() const override;
};

class DecorationMaterial : public Material
{
	bool isSolid() const override;
	bool blocksLight() const override;
	bool blocksMotion() const override;
};

