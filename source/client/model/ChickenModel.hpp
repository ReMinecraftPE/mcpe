/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Model.hpp"

class ChickenModel : public Model
{
public:
	ChickenModel();
	~ChickenModel();
	void render(float, float, float, float, float, float) override;
	void setupAnim(float, float, float, float, float, float) override;

private:
	ModelPart m_head;
	//ModelPart m_hair;
	ModelPart m_body;
	ModelPart m_leg1;
	ModelPart m_leg2;
	ModelPart m_wing1;
	ModelPart m_wing2;
	ModelPart m_beak;
	ModelPart m_wattle;
};
