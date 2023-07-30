#pragma once

#include "Model.hpp"

class HumanoidModel : public Model
{
public:
	HumanoidModel(float a, float b);
	void _logGraphics();
	// @TODO - No xrefs: void render(HumanoidModel* a, float f);

	void onGraphicsReset() override;
	void render(float, float, float, float, float, float) override;
	void renderHorrible(float, float, float, float, float, float) override;
	void setupAnim(float, float, float, float, float, float) override;

public:
	// @TODO: swap armL and armR.. Steve punches with the right hand.
	Cube m_head, m_body, m_armL, m_armR, m_legL, m_legR;
	bool field_10BC = false;
	bool field_10BD = false;
	bool field_10BE = false;
};

