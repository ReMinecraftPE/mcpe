#pragma once

#include "HumanoidModel.hpp"
#include "Font.hpp"

class EntityRenderDispatcher;

class EntityRenderer
{
public:
	EntityRenderer();
	void bindTexture(const std::string& file);
	Font* getFont();
	void init(EntityRenderDispatcher* d);
	static void render(const AABB&, float, float, float);
	static void renderFlat(const AABB&);

	virtual void render(Entity*, float, float, float, float, float) = 0;
	virtual void onGraphicsReset();

public:
	float field_4 = 0.0f;
	float field_8 = 1.0f;
	EntityRenderDispatcher* m_pDispatcher = nullptr;

	// @HUH: Why is there a HumanoidModel here? There's another
	// in HumanoidMobRenderer...
	HumanoidModel m_model;
};
