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
	HumanoidModel(float modelSize = 0.0f, float yOffset = 0.0f);
	// @TODO - No xrefs: void render(HumanoidModel* a, float f);

	void onGraphicsReset() override;
	void render(float, float, float, float, float, float) override;
	void setupAnim(float, float, float, float, float, float) override;
	void setBrightness(float) override;

public:
	ModelPart m_head, m_hat, m_body, m_armRight, m_armLeft, m_legRight, m_legLeft;
	bool m_bHoldingLeftHand;
	bool m_bHoldingRightHand;
	bool m_bSneaking;
};

