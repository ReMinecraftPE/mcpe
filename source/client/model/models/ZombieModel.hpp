#pragma once

#include "HumanoidModel.hpp"

class ZombieModel : public HumanoidModel
{
public:
	ZombieModel();
	~ZombieModel();
	void setupAnim(float, float, float, float, float, float) override;
};
