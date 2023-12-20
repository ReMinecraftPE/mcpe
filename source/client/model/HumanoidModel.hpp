/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Model.hpp"

class HumanoidModel : public Model
{
public:
	HumanoidModel(float a, float b);
	// @TODO - No xrefs: void render(HumanoidModel* a, float f);

	void onGraphicsReset() override;
	void render(float, float, float, float, float, float) override;
	void setupAnim(float, float, float, float, float, float) override;
	void setBrightness(float) override;

public:
	bool field_20 = false;
	ModelPart m_head, m_body, m_arm1, m_arm2, m_leg1, m_leg2;
	bool field_234;
	bool field_235;
	bool m_bSneaking;
	bool field_237;
};

