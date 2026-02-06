#ifdef _DIRECT3D9
// D3D9 does not support custom semantic names, which is very unfortunate
#define PS_FOG_COLOR COLOR1
#define LIGHT COLOR2
#define PS_OVERLAY_COLOR COLOR3
#endif

struct PS_Input
{
    float4 position : SV_Position;

    float4 light : LIGHT;
    float4 fogColor : PS_FOG_COLOR;

#ifndef COLOR_BASED
    float2 uv : TEXCOORD_0;
#endif

#ifdef USE_VERTEX_COLORS
	float4 color : COLOR;
#endif

#ifdef USE_OVERLAY
    float4 overlayColor : PS_OVERLAY_COLOR;
#endif

#ifdef GLINT
	float2 layer1UV : UV_1;
	float2 layer2UV : UV_2;
#endif
};
