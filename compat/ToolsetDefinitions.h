#pragma once

//
//  ToolsetDefinitions.h
//  Minecraft
//
//  Created by Brent on 1/7/26.
//
//

#pragma once

/* Microsoft - Xbox Development Kit (XDK) */
/* Xbox, Xbox 360 */
#if (defined(_XBOX))
#define MC_SDK_XDK 1
#else
#define MC_SDK_XDK 0
#endif

/* Free60 Project - libXenon */
/* Xbox 360 */
#if (defined(XENON))
#define MC_SDK_LIBXENON 1
#else
#define MC_SDK_LIBXENON 0
#endif
