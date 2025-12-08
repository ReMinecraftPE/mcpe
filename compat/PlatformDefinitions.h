//
//  PlatformDefinitions.h
//  Minecraft
//
//  Created by Brent on 11/27/23.
//
//

#pragma once

#include "EndianDefinitions.h"

/* Apple - Mac OS X / macOS */
#ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#define MC_PLATFORM_MAC 1
#else
#define MC_PLATFORM_MAC 0
#endif

/* Apple - iPhoneOS / iOS */
#ifdef __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__
#define MC_PLATFORM_IOS 1
#else
#define MC_PLATFORM_IOS 0
#endif

/* Apple - Device Simulator */
#define MC_PLATFORM_SIMULATOR (TARGET_OS_SIMULATOR || TARGET_IPHONE_SIMULATOR || __APPLE_EMBEDDED_SIMULATOR__)

/* Google - Android */
#if (defined(ANDROID))
#define MC_PLATFORM_ANDROID 1
#else
#define MC_PLATFORM_ANDROID 0
#endif

/* Microsoft - Xbox */
#ifdef _XBOX
#define MC_PLATFORM_XBOX 1
#else
#define MC_PLATFORM_XBOX 0
#endif

/* Microsoft - Xbox One */
#if (defined (_DURANGO) || defined(_XBOX_ONE))
#define MC_PLATFORM_XBOXONE 1
#else
#define MC_PLATFORM_XBOXONE 0
#endif

/* Microsoft - Xbox 360 */
#if (defined (X360) && !MC_PLATFORM_XBOXONE)
#define MC_PLATFORM_XBOX360 1
#else
#define MC_PLATFORM_XBOX360 0
#endif

/* Microsoft - Original Xbox */
#if (defined (_XBOX) && !MC_PLATFORM_XBOX360 && !MC_PLATFORM_XBOXONE && !defined(WINDOWS_STORE_RT))
#define MC_PLATFORM_XBOXOG 1
#else
#define MC_PLATFORM_XBOXOG 0
#endif

/* Microsoft - Windows PC */
#if (defined (_WIN32) && !MC_PLATFORM_XBOX && !defined(WINDOWS_PHONE_8) && !defined(WINDOWS_STORE_RT))
#define MC_PLATFORM_WINPC 1
#else
#define MC_PLATFORM_WINPC 0
#endif
