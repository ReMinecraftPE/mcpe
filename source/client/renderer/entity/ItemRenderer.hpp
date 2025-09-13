/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "EntityRenderer.hpp"
#include "../TileRenderer.hpp"
#include "world/item/ItemInstance.hpp"

class ItemRenderer : public EntityRenderer
{
public:
	ItemRenderer();

	void render(Entity* entity, const Vec3& pos, float rot, float a) override;
	void blitRect(Tesselator&, int, int, int, int, int);

	static void blit(int, int, int, int, int, int);
	static void renderGuiItem(Font*, Textures*, ItemInstance*, int, int, bool);
	static void renderGuiItemOverlay(Font*, Textures*, ItemInstance*, int, int);

public:
	Random m_random;
	static TileRenderer* tileRenderer;
};

