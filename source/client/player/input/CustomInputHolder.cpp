/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CustomInputHolder.hpp"

CustomInputHolder::CustomInputHolder(IMoveInput* pMoveInput, ITurnInput* pTurnInput, IBuildInput* pBuildInput)
{
	setInputs(pMoveInput, pTurnInput, pBuildInput);
}

IMoveInput* CustomInputHolder::getMoveInput()
{
	return m_pMoveInput;
}

ITurnInput* CustomInputHolder::getTurnInput()
{
	return m_pTurnInput;
}

IBuildInput* CustomInputHolder::getBuildInput()
{
	return m_pBuildInput;
}

void CustomInputHolder::setInputs(IMoveInput* pMoveInput, ITurnInput* pTurnInput, IBuildInput* pBuildInput)
{
	m_pMoveInput = pMoveInput;
	m_pTurnInput = pTurnInput;
	m_pBuildInput = pBuildInput;
}
