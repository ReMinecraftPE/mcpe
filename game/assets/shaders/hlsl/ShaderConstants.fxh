// These [aren't but] should be grouped in a way that they require the least amount of updating (world data in one, model data in another, part of model data in another one, etc)

Texture2D TEXTURE_0 : register ( t0 );
Texture2D TEXTURE_1 : register ( t1 );
Texture2D TEXTURE_2 : register ( t2 );

// Make sure this thing is actually getting bound
sampler TextureSampler0 : register( s0 );
sampler TextureSampler1 : register( s1 );
sampler TextureSampler2 : register( s2 );

#ifdef LOW_PRECISION
#define lpfloat min16float
#define lpfloat4 min16float4
#else
#define lpfloat float
#define lpfloat4 float4
#endif


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