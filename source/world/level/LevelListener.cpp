/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelListener.hpp"

void LevelListener::tileBrightnessChanged(const TilePos& pos)
{
	tileChanged(pos);
}
