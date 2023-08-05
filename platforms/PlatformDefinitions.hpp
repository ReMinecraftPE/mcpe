#include <SoundSystem.hpp>


//				Sound Systems
//-----------------------------------------

// Uncomment to enable OtherSoundSystemHere
//#define OtherSoundSystemHere

// OtherSoundSystemHere Sound System
#if defined(OtherSoundSystemHere)
	#include "SomePlatformHere/SoundSystem_OtherSoundSystemHere.hpp"
	#define SOUND_SYSTEM_TYPE SoundSystem_OtherSoundSystemHere

// Windows Sound System (Direct Sound)
#elif defined _WIN32
	#include "windows/SoundSystem_windows.hpp"
	#define SOUND_SYSTEM_TYPE SoundSystemWindows
#else
// Not implemented
	#define SOUND_SYSTEM_TYPE SoundSystem
#endif

//-----------------------------------------