#pragma once

#include "MobRenderer.hpp"

class HumanoidMobRenderer : public MobRenderer
{
public:
	HumanoidMobRenderer(HumanoidModel*, float);
	virtual void additionalRendering(Mob*, float) override;
	virtual void onGraphicsReset() override;

	void renderHand();

public:
	HumanoidModel* m_pHumanoidModel;
};

