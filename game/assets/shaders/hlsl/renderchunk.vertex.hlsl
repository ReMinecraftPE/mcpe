#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD_0;
    //float2 uv1 : TEXCOORD_1;
};


struct PS_Input
{
    float4 position : SV_Position;
    lpfloat4 color : COLOR;
    snorm float2 uv0 : TEXCOORD_0;
    //snorm float2 uv1 : TEXCOORD_1;
#ifdef FOG
    float4 fogColor : FOG_COLOR;
#endif
};


static const float rA = 1.0;
static const float rB = 1.0;
static const float3 UNIT_Y = float3( 0, 1, 0 );
static const float DIST_DESATURATION = 56.0 / 255.0; //WARNING this value is also hardcoded in the water color, don'tchange


VS_MAIN_BEGIN
    PSInput.uv0 = VSInput.uv0;
    //PSInput.uv1 = VSInput.uv1;
    PSInput.color = VSInput.color;
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position.xyz, 1 ) );


#if defined(FOG) || defined(NEAR_WATER)
#ifdef FANCY
    float3 relPos = VIEW_POS - ( VSInput.position.xyz + CHUNK_ORIGIN );
    float cameraDepth = length( relPos );
#else
    float cameraDepth = PSInput.position.z;
#endif
#endif

    ///// apply fog

#ifdef FOG
    float len = cameraDepth / RENDER_DISTANCE;
#ifdef ALLOW_FADE
    len += CURRENT_COLOR.r;
#endif

    PSInput.fogColor.rgb = FOG_COLOR.rgb;
    PSInput.fogColor.a = clamp( ( len - FOG_CONTROL.x ) / ( FOG_CONTROL.y - FOG_CONTROL.x ), 0.0, 1.0 );
#endif

    ///// water magic
#ifdef NEAR_WATER
#ifdef FANCY  /////enhance water
    float F = dot( normalize( relPos ), UNIT_Y );

    //if both NEAR_WATER and FOG are defined, it's forced fog and we're underwater
#ifndef FOG  //over water
    F = 1.0 - max( F, 0.1 );
#else //under water
    PSInput.color.a = 0.7; //disable fake transparency and be transparent
#endif

    F = 1.0 - lerp( F*F*F*F, 1.0, min( 1.0, cameraDepth / FAR_CHUNKS_DISTANCE ) );

    PSInput.color.rg -= float2( float( F * DIST_DESATURATION ).xx );

    float4 depthColor = float4( PSInput.color.rgb * 0.5, 1.0 );
    float4 traspColor = float4( PSInput.color.rgb * 0.45, 0.8 );
    float4 surfColor = float4( PSInput.color.rgb, 1.0 );

    float4 nearColor = lerp( traspColor, depthColor, PSInput.color.a );
    PSInput.color = lerp( surfColor, nearColor, F );
#else
    PSInput.color.a = PSInput.position.z / FAR_CHUNKS_DISTANCE + 0.5;
#endif //FANCY
#endif
VS_MAIN_END
