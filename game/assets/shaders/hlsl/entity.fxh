#ifdef _DIRECT3D9
// D3D9 does not support custom semantic names, which is very unfortunate
#define PS_FOG_COLOR COLOR1
#define LIGHT COLOR2
#define PS_OVERLAY_COLOR COLOR3
#endif
