#pragma once

#include "EntityRenderer.hpp"
#include "ItemInstance.hpp"
#include "TileRenderer.hpp"

class ItemRenderer : public EntityRenderer
{
public:
	ItemRenderer();

	void render(Entity*, float, float, float, float, float) override;
	void blitRect(Tesselator&, int, int, int, int, int);

	static void blit(int, int, int, int, int, int);
	static void renderGuiItem(Font*, Textures*, ItemInstance*, int, int, bool);

public:
	Random m_random;
	static TileRenderer* tileRenderer;
};

