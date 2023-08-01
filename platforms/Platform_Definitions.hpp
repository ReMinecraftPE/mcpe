#include <SoundSystem.hpp>

//				Sound Systems
//-----------------------------------------

// Windows Sound System (Direct Sound)
#if 1
	#include "windows/SoundSystem_windows.hpp"
	#define SOUND_SYSTEM_TYPE SoundSystemWindows
#endif

//-----------------------------------------