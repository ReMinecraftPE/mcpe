/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Model.hpp"

class QuadrupedModel : public Model
{
public:
	QuadrupedModel(int, float);
	~QuadrupedModel();

	void render(float, float, float, float, float, float) override;
	void setupAnim(float, float, float, float, float, float) override;

protected:
	friend class PigModel;
	ModelPart m_head;
	ModelPart m_hair;
	ModelPart m_body;
	ModelPart m_leg1;
	ModelPart m_leg2;
	ModelPart m_leg3;
	ModelPart m_leg4;
	float field_28C;
	float field_290;
};
