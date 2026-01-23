// These [aren't but] should be grouped in a way that they require the least amount of updating (world data in one, model data in another, part of model data in another one, etc)

#ifdef SHADER_TYPE_FRAGMENT
#if defined(_DIRECT3D9)
texture TEXTURE_0 : register ( t0 );
texture TEXTURE_1 : register ( t1 );
texture TEXTURE_2 : register ( t2 );

// Make sure this thing is actually getting bound
sampler2D TextureSampler0 : register( s0 ) = 
sampler_state
{
  Texture = (TEXTURE_0);
};
sampler2D TextureSampler1 : register( s1 ) = 
sampler_state
{
  Texture = (TEXTURE_1);
};
sampler2D TextureSampler2 : register( s2 ) = 
sampler_state
{
  Texture = (TEXTURE_2);
};

#define sampleTex0 tex2D
#define sampleTex1 tex2D
#define sampleTex2 tex2D
#else
Texture2D TEXTURE_0 : register ( t0 );
Texture2D TEXTURE_1 : register ( t1 );
Texture2D TEXTURE_2 : register ( t2 );

// Make sure this thing is actually getting bound
sampler TextureSampler0 : register( s0 );
sampler TextureSampler1 : register( s1 );
sampler TextureSampler2 : register( s2 );

#define sampleTex0 TEXTURE_0.Sample
#define sampleTex1 TEXTURE_1.Sample
#define sampleTex2 TEXTURE_2.Sample
#endif
#endif

#if defined(LOW_PRECISION) && !defined(_DIRECT3D9)
#define lpfloat min16float
#define lpfloat4 min16float4
#else
#define lpfloat float
#define lpfloat4 float4
#endif

#ifdef _DIRECT3D9

#define CBUFFER_BEGIN(name, reg) cbuffer name {
#define CBUFFER_END }

#define VS_MAIN_BEGIN PS_Input main( VS_Input VSInput ) { PS_Input PSInput; 
#define VS_MAIN_END float2 __d3d9offset = (__D3D9_OFFSET_X, __D3D9_OFFSET_Y); PSInput.position.xy += __d3d9offset.xy * PSInput.position.w; return PSInput; }
#define PS_MAIN_BEGIN PS_Output main( PS_Input PSInput ) { PS_Output PSOutput; 
#define PS_MAIN_END return PSOutput; }

#define R8G8B8A8_SNORM_UNSUPPORTED

// D3D9 has OpenGL-like render state alpha testing
#define discard PSOutput.color = float4(0,0,0,0); return PSOutput
#define snorm

// System-Value Semantics were introduced in D3D10
// Pixel Shaders
#define SV_Position POSITION
#define SV_Target COLOR
#define SV_Depth DEPTH

#else // !defined(_DIRECT3D9)

#define CBUFFER_BEGIN(name, reg) cbuffer name : register ( b##reg ) {
#define CBUFFER_END }

#define VS_MAIN_BEGIN void main( in VS_Input VSInput, out PS_Input PSInput ) {
#define VS_MAIN_END }
#define PS_MAIN_BEGIN void main( in PS_Input PSInput, out PS_Output PSOutput ) {
#define PS_MAIN_END }

#endif // !defined(_DIRECT3D9)


CBUFFER_BEGIN(RenderChunkConstants, 1)
	//float2 VIEWPORT_DIMENSION;
	//float3 VIEW_POS;

	float3 CHUNK_ORIGIN;
CBUFFER_END

CBUFFER_BEGIN(EntityConstants, 2)
	float4 OVERLAY_COLOR;
	float4 TILE_LIGHT_COLOR;
	float4 CHANGE_COLOR;
	float2 UV_ANIM;
	float2 UV_OFFSET;
	float2 UV_ROTATION;
	float2 GLINT_UV_SCALE;
CBUFFER_END

CBUFFER_BEGIN(PerFrameConstants, 3)
	float3 VIEW_DIRECTION; // unneeded
	float TIME; // unneeded

	float3 VIEW_POS;
	float FAR_CHUNKS_DISTANCE;

	float4 FOG_COLOR;

	float2 FOG_CONTROL;

	float RENDER_DISTANCE;
CBUFFER_END

CBUFFER_BEGIN(WorldConstants, 4)
	float4x4 WORLDVIEWPROJ;
	float4x4 WORLD;
CBUFFER_END

CBUFFER_BEGIN(ShaderConstants, 5)
	float4 CURRENT_COLOR;
	float4 DARKEN;
CBUFFER_END

CBUFFER_BEGIN(WeatherConstants, 5)
	float4	POSITION_OFFSET;
	float4	VELOCITY;
	float4	ALPHA;
	float4	VIEW_POSITION;
	float4	SIZE_SCALE;
	float4	FORWARD;
	float4	UV_INFO;
	float4  PARTICLE_BOX;
CBUFFER_END
