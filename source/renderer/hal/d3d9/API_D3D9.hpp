#pragma once

#define WIN32_LEAN_AND_MEAN

#ifdef _XBOX
#include <xtl.h>
#else
// Must always include Winsock2.h before windows.h
#include <winsock2.h>
#endif

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif
#include <d3d9.h>

#ifndef D3DUSAGE_DYNAMIC
#define D3DUSAGE_DYNAMIC 0x0
#endif