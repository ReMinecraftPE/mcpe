/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Material.hpp"
#include "world/item/Tool.hpp"

Material::Material() :
	m_bFlammable(false), m_bMineable(true), m_toolMask(Tool::NONE), m_requiredToolLevel(0)
{
}

Material::Material(bool bFlammable) :
	m_bFlammable(bFlammable), m_bMineable(true), m_toolMask(Tool::NONE), m_requiredToolLevel(0)
{
}

Material::~Material()
{
}

Material
* Material::air,
* Material::dirt,
* Material::wood,
* Material::stone,
* Material::metal,
* Material::water,
* Material::lava,
* Material::leaves,
* Material::plant,
* Material::sponge,
* Material::cloth,
* Material::fire,
* Material::sand,
* Material::decoration,
* Material::glass,
* Material::explosive,
* Material::coral,
* Material::ice,
* Material::topSnow,
* Material::snow,
* Material::cactus,
* Material::clay,
* Material::vegetable,
* Material::portal,
* Material::cake,
* Material::web;

void Material::initMaterials()
{
	air        = new GasMaterial();

	dirt       = (new Material())
		->setToolTypes(Tool::SHOVEL);

	wood       = (new Material(true))
		->setToolTypes(Tool::HATCHET);

	stone      = (new Material())
		->setToolTypes(Tool::PICKAXE)
		->setNotAlwaysDestroyable();

	metal      = (new Material())
		->setNotAlwaysDestroyable();

	water      = new LiquidMaterial();

	lava       = new LiquidMaterial();

	leaves     = new Material(true);

	plant      = new DecorationMaterial();

	sponge     = new Material();

	cloth      = new Material(true);

	fire       = new GasMaterial();

	sand       = (new Material())
		->setToolTypes(Tool::SHOVEL);

	decoration = new DecorationMaterial();

	glass      = new Material();

	explosive  = new Material(true);

	coral      = new Material();

	ice        = new Material();

	topSnow    = (new DecorationMaterial())
		->setToolTypes(Tool::SHOVEL)
		->setNotAlwaysDestroyable();

	snow       = (new Material())
		->setToolTypes(Tool::SHOVEL)
		->setNotAlwaysDestroyable();

	cactus     = new Material();

	clay       = (new Material())
		->setToolTypes(Tool::SHOVEL);

	vegetable  = new Material();

	portal     = new Material();

	cake       = new Material();

	web        = (new Material())
		->setNotAlwaysDestroyable();
}

void Material::teardownMaterials()
{
	if (air)        delete air;
	if (dirt)       delete dirt;
	if (wood)       delete wood;
	if (stone)      delete stone;
	if (metal)      delete metal;
	if (water)      delete water;
	if (lava)       delete lava;
	if (leaves)     delete leaves;
	if (plant)      delete plant;
	if (sponge)     delete sponge;
	if (cloth)      delete cloth;
	if (fire)       delete fire;
	if (sand)       delete sand;
	if (decoration) delete decoration;
	if (glass)      delete glass;
	if (explosive)  delete explosive;
	if (coral)      delete coral;
	if (ice)        delete ice;
	if (topSnow)    delete topSnow;
	if (snow)       delete snow;
	if (cactus)     delete cactus;
	if (clay)       delete clay;
	if (vegetable)  delete vegetable;
	if (portal)     delete portal;
	if (cake)       delete cake;
	if (web)        delete web;
}

bool Material::isLiquid() const
{
	return false;
}

bool Material::isMineable() const
{
	return m_bMineable;
}

bool Material::letsWaterThrough() const
{
	if (isLiquid())
		return false;

	return !isSolid();
}

bool Material::isSolid() const
{
	return true;
}

bool Material::isFlammable() const
{
	return m_bFlammable;
}

Material* Material::setNotAlwaysDestroyable()
{
	m_bMineable = false;
	return this;
}

Material* Material::setToolTypes(unsigned int toolMask, int toolLevel)
{
	m_toolMask |= toolMask;
	m_requiredToolLevel = toolLevel;
	return this;
}

bool Material::blocksLight() const
{
	return true;
}

bool Material::blocksMotion() const
{
	return true;
}

bool GasMaterial::isSolid() const
{
	return false;
}

bool GasMaterial::blocksLight() const
{
	return false;
}

bool GasMaterial::blocksMotion() const
{
	return false;
}

bool DecorationMaterial::isSolid() const
{
	return false;
}

bool DecorationMaterial::blocksLight() const
{
	return false;
}

bool DecorationMaterial::blocksMotion() const
{
	return false;
}

bool LiquidMaterial::isLiquid() const
{
	return true;
}

bool LiquidMaterial::isSolid() const
{
	return false;
}

bool LiquidMaterial::blocksMotion() const
{
	return false;
}
