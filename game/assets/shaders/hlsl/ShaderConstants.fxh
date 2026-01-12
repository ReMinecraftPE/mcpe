// These [aren't but] should be grouped in a way that they require the least amount of updating (world data in one, model data in another, part of model data in another one, etc)

#ifdef SHADER_TYPE_FRAGMENT
#if defined(_DIRECT3D9) && !defined(_XBOX)
Texture2D TEXTURE_0 : register ( s0 );
Texture2D TEXTURE_1 : register ( s1 );
Texture2D TEXTURE_2 : register ( s2 );

// Make sure this thing is actually getting bound
sampler TextureSampler0 : register( s3 );
sampler TextureSampler1 : register( s4 );
sampler TextureSampler2 : register( s5 );
#else
Texture2D TEXTURE_0 : register ( t0 );
Texture2D TEXTURE_1 : register ( t1 );
Texture2D TEXTURE_2 : register ( t2 );

// Make sure this thing is actually getting bound
sampler TextureSampler0 : register( s0 );
sampler TextureSampler1 : register( s1 );
sampler TextureSampler2 : register( s2 );
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

#define VS_MAIN_BEGIN PS_Input main( VS_Input VSInput ) { PS_Input PSInput; 
#define VS_MAIN_END return PSInput; }
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

#define VS_MAIN_BEGIN void main( in VS_Input VSInput, out PS_Input PSInput ) {
#define VS_MAIN_END }
#define PS_MAIN_BEGIN void main( in PS_Input PSInput, out PS_Output PSOutput ) {
#define PS_MAIN_END }

#endif // !defined(_DIRECT3D9)


cbuffer RenderChunkConstants : register(b1)
{
	//float2 VIEWPORT_DIMENSION;
	//float3 VIEW_POS;

	float3 CHUNK_ORIGIN;
}

cbuffer EntityConstants : register (b2)
{
	float4 OVERLAY_COLOR;
	float4 TILE_LIGHT_COLOR;
	float4 CHANGE_COLOR;
	float2 UV_ANIM;
	float2 UV_OFFSET;
	float2 UV_ROTATION;
	float2 GLINT_UV_SCALE;
}

cbuffer PerFrameConstants : register (b3)
{
	float3 VIEW_DIRECTION; // unneeded
	float TIME; // unneeded

	float3 VIEW_POS;
	float FAR_CHUNKS_DISTANCE;

	float4 FOG_COLOR;

	float2 FOG_CONTROL;

	float RENDER_DISTANCE;
}

cbuffer WorldConstants : register (b4)
{
	float4x4 WORLDVIEWPROJ;
	float4x4 WORLD;
}

cbuffer ShaderConstants : register (b5)
{
	float4 CURRENT_COLOR;
	float4 DARKEN;
}

cbuffer WeatherConstants : register (b5) {
	float4	POSITION_OFFSET;
	float4	VELOCITY;
	float4	ALPHA;
	float4	VIEW_POSITION;
	float4	SIZE_SCALE;
	float4	FORWARD;
	float4	UV_INFO;
	float4  PARTICLE_BOX;
}