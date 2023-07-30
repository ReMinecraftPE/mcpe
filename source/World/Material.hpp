#pragma once

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
		*cake;

public:
	bool m_bFlammable = false;
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

