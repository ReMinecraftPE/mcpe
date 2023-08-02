/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundData.hpp"

// --------------------------------------------------------------------
// WARNING! If you have an error here it is most likely because you did
// not grab the sound data from a working mcpe01_canada.apk.
// 
// Check the readme for a guide on how to extract game sounds from the
// mcpe01_canada.apk file.
// --------------------------------------------------------------------
#ifndef USE_PCM_FILES
#include "../../sound_data/sounds.h"
#else
// Declare them here
SoundDesc SA_cloth1;
SoundDesc SA_cloth2;
SoundDesc SA_cloth3;
SoundDesc SA_cloth4;
SoundDesc SA_grass1;
SoundDesc SA_grass2;
SoundDesc SA_grass3;
SoundDesc SA_grass4;
SoundDesc SA_gravel1;
SoundDesc SA_gravel2;
SoundDesc SA_gravel3;
SoundDesc SA_gravel4;
SoundDesc SA_sand1;
SoundDesc SA_sand2;
SoundDesc SA_sand3;
SoundDesc SA_sand4;
SoundDesc SA_stone1;
SoundDesc SA_stone2;
SoundDesc SA_stone3;
SoundDesc SA_stone4;
SoundDesc SA_wood1;
SoundDesc SA_wood2;
SoundDesc SA_wood3;
SoundDesc SA_wood4;
SoundDesc SA_click;
SoundDesc SA_explode;
SoundDesc SA_splash;
#endif