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