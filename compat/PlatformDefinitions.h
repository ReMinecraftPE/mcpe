//
//  PlatformDefinitions.h
//  Minecraft
//
//  Created by Brent on 11/27/23.
//
//

#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#define MC_TARGET_OS_SIMULATOR (TARGET_OS_SIMULATOR || TARGET_IPHONE_SIMULATOR)
#define MC_TARGET_OS_IOS (TARGET_OS_IPHONE && (TARGET_OS_IOS || !defined(TARGET_OS_IOS)))
#define MC_TARGET_OS_MAC (TARGET_OS_MAC)
