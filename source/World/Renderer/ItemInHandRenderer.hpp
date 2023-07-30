#pragma once

#include "ItemInstance.hpp"
#include "TileRenderer.hpp"

class Minecraft;

class ItemInHandRenderer
{
public:
	ItemInHandRenderer(Minecraft* pMC);
	void itemPlaced();
	void itemUsed();
	void render(float f);
	void renderItem(ItemInstance*);
	void renderScreenEffect(float f);
	void renderFire(float f);
	void renderTex(float f, int tex);
	void tick();

public:
	int field_0 = -1;
	ItemInstance m_ItemInstance;
	Minecraft* m_pMinecraft;
	int field_18 = 0;
	float field_1C = 0.0f;
	float field_20 = 0.0f;
	TileRenderer m_tileRenderer;
};

