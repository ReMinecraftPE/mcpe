/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "IInputHolder.hpp"

class CustomInputHolder : public IInputHolder
{
public:
	CustomInputHolder(IMoveInput*, ITurnInput*, IBuildInput*);

	IMoveInput* getMoveInput() override;
	ITurnInput* getTurnInput() override;
	IBuildInput* getBuildInput() override;

	void setInputs(IMoveInput*, ITurnInput*, IBuildInput*);


private:
	IMoveInput* m_pMoveInput;
	ITurnInput* m_pTurnInput;
	IBuildInput* m_pBuildInput;
};

