#pragma once

#include "Model.hpp"

class SpiderModel : public Model
{
public:
	SpiderModel();
	~SpiderModel();

	void render(float time, float r, float bob, float y_rot, float x_rot, float scale) override;
	void setupAnim(float time, float r, float bob, float y_rot, float x_rot, float scale) override;

private:
	ModelPart m_head;
	ModelPart m_body0;
	ModelPart m_body1;
	ModelPart m_leg0;
	ModelPart m_leg1;
	ModelPart m_leg2;
	ModelPart m_leg3;
	ModelPart m_leg4;
	ModelPart m_leg5;
	ModelPart m_leg6;
	ModelPart m_leg7;
};