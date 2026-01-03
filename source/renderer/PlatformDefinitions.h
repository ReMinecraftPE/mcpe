#include "GameMods.hpp"

/*
GFX-API-Specific Settings (all 0 by default)
	MCE_GFX_ROW_MAJOR               // Transposes column-major matrices to row-major before being passed to the GFX API
	MCE_GFX_DEPTH_ZERO_TO_ONE       // Tells GLM to ensure the depth of matrices is between 0.0f and 1.0f
	MCE_GFX_LEFT_HANDED             // Tells GLM to use left-handed matrices
	MCE_GFX_DYNAMIC_TEXTURE_STAGING // Indicates that the GFX API requires staging to occur before an existing texture can be modified on the GPU.
*/

#if MCE_GFX_API_OGL
#define MCE_GFX_API OGL
#define MCE_GFX_API_DIR ogl
#endif

#if MCE_GFX_API_D3D9
#define MCE_GFX_API D3D9
#define MCE_GFX_API_DIR d3d9
#endif

#if MCE_GFX_API_D3D11
#define MCE_GFX_API D3D11
#define MCE_GFX_API_DIR d3d11
#define MCE_GFX_DEPTH_ZERO_TO_ONE 1
#define MCE_GFX_DYNAMIC_TEXTURE_STAGING 1
#endif

#if MCE_GFX_API_GX1
#define MCE_GFX_API GX1
#define MCE_GFX_API_DIR gx1
#endif

#if MCE_GFX_API_GX2
#define MCE_GFX_API GX2
#define MCE_GFX_API_DIR gx2
#endif

#if MCE_GFX_API_NULL || !defined(MCE_GFX_API)
#define MCE_GFX_API Null
#define MCE_GFX_API_DIR null
#endif

#define _MCE_GFX_CLASS_MANUA(className, api) className ## api
#define _MCE_GFX_CLASS_MANUAL(className, api) _MCE_GFX_CLASS_MANUA(className, api)
#define MCE_GFX_CLASS(className) _MCE_GFX_CLASS_MANUAL(className, MCE_GFX_API)

// Macros are cursed
#define _STR(x) #x
#define STR(x) _STR(x)

#define _MCE_GFX_CLASS_FILE(apiDir, className, fileExtension) STR(renderer/hal/apiDir/className.fileExtension)
#define _MCE_GFX_CLASS_HEADER_MANUAL(apiDir, className) _MCE_GFX_CLASS_FILE(apiDir, className, hpp)
#define MCE_GFX_CLASS_HEADER(className) _MCE_GFX_CLASS_HEADER_MANUAL(MCE_GFX_API_DIR, MCE_GFX_CLASS(className))

#define _MCE_GFX_CLASS_IMPL_MANUAL(apiDir, className) _MCE_GFX_CLASS_FILE(apiDir, className, cpp)
#define MCE_GFX_CLASS_IMPL(className) _MCE_GFX_CLASS_IMPL_MANUAL(MCE_GFX_API_DIR, MCE_GFX_CLASS(className))

#ifdef FEATURE_GFX_SHADERS
#define MCE_GFX_CLASS_SHADER(className) MCE_GFX_CLASS(className)
#define MCE_GFX_CLASS_HEADER_SHADER(className) MCE_GFX_CLASS_HEADER(className)
#define MCE_GFX_CLASS_FIXED(className) _MCE_GFX_CLASS_MANUA(className, Null)
#define MCE_GFX_CLASS_HEADER_FIXED(className) _MCE_GFX_CLASS_HEADER_MANUAL(null, MCE_GFX_CLASS_FIXED(className))
#else
#define MCE_GFX_CLASS_SHADER(className) _MCE_GFX_CLASS_MANUA(className, Null)
#define MCE_GFX_CLASS_HEADER_SHADER(className) _MCE_GFX_CLASS_HEADER_MANUAL(null, MCE_GFX_CLASS_SHADER(className))
#define MCE_GFX_CLASS_FIXED(className) MCE_GFX_CLASS(className)
#define MCE_GFX_CLASS_HEADER_FIXED(className) MCE_GFX_CLASS_HEADER(className)
#endif