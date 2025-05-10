#ifdef _WIN32
#include <al.h>
#include <alc.h>
#pragma comment( lib, "OpenAl32.lib" )
#elif defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

// Error Checking
#define AL_ERROR_CHECK() AL_ERROR_CHECK_MANUAL(alGetError())
#define AL_ERROR_CHECK_MANUAL(val) \
	{ \
		ALenum __err = val; \
		if (__err != AL_NO_ERROR) \
		{ \
			LOG_E("(%s:%i) OpenAL Error: %s", __FILE__, __LINE__, alGetString(__err)); \
			assert(!"An OpenAL error occurred!"); \
		} \
	}
#define AL_ERROR_SILENCE() { while ( alGetError() ); }