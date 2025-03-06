#pragma once

#include "MobRenderer.hpp"
#include "world/entity/Sheep.hpp"

class SheepRenderer : public MobRenderer
{
public:
	SheepRenderer(Model*, Model*, float);
	~SheepRenderer();
protected:
	virtual int prepareArmor(Mob* mob, int, float) override;
};
