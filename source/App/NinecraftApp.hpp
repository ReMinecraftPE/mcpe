#pragma once

#include "Minecraft.hpp"
#include "Level.hpp"
#include "Tile.hpp"

//@TYPO: This is probably meant to say "MinecraftApp". Still not fixed in V0.3.0 though so not sure
class NinecraftApp : public Minecraft
{
public:
	virtual ~NinecraftApp();

	bool handleBack(bool) override;
	void init() override;
	void update() override;
	void onGraphicsReset() override;
	void teardown();

	void updateStats();
	void initGLStates();

private:
	int field_DBC = 0;
	bool field_DC0 = 1;
	int m_fps = 0;

	static bool _hasInitedStatics;
};

