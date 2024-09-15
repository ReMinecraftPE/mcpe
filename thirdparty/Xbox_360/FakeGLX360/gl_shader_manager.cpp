#include <xtl.h>
#include "gl_shader_manager.h"
#include "gl_main.h"

static char buffer[16384];
#define WRITE(x, ...) p+=sprintf(p, x "\n", __VA_ARGS__)

GlShaderManager shader;

GlShaderManager::GlShaderManager() : Dirty(0xFFFFFFFF) {
	D3DVERTEXELEMENT9 VertexElements[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		//{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, 32, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    GLImpl.device->CreateVertexDeclaration( VertexElements, &v_decl );

	texture_enabled = 0;
}

void GlShaderManager::ClearCache() {
	for (PSCache::iterator iter = psCache.begin(); iter != psCache.end(); ++iter)	{
		iter->second->Release();
	}
	for (VSCache::iterator iter = vsCache.begin(); iter != vsCache.end(); ++iter)	{
		iter->second->Release();
	}

	psCache.clear();
	vsCache.clear();
}

void GlShaderManager::ApplyShader() {
	D3DPixelShader * ps = NULL;
	D3DVertexShader * vs = NULL;

	// look in shader cache
	PSCache::iterator ps_it = psCache.find(Texenv.u32);
	if (ps_it != psCache.end()) {
		ps = ps_it->second;
	}

	VSCache::iterator vs_it = vsCache.find(1);
	if (vs_it != vsCache.end()) {
		vs = vs_it->second;
	}

	// create shaders
	if (ps == NULL) {
		// Compile pixel shader.
		ID3DXBuffer* pPixelShaderCode;
		ID3DXBuffer* pPixelErrorMsg;
		char * code = PsGenerator();
		HRESULT hr = D3DXCompileShader( code, 
			(UINT)strlen( code ),
			NULL, 
			NULL, 
			"main", 
			"ps_2_0", 
			0, 
			&pPixelShaderCode, 
			&pPixelErrorMsg,
			NULL );
		if( FAILED(hr) )
		{
			if( pPixelErrorMsg )
			{	
				OutputDebugString( (char*)pPixelErrorMsg->GetBufferPointer() );
#if _DEBUG
				DebugBreak();
#endif
			}
		}

		OutputDebugStringA( code );

		// Create pixel shader.
		GLImpl.device->CreatePixelShader( (DWORD*)pPixelShaderCode->GetBufferPointer(), 
			&ps );

		// add to cache
		psCache[Texenv.u32] = ps;
	}

	// create shaders
	if (vs == NULL) {
		// Compile pixel shader.
		ID3DXBuffer* pPixelShaderCode;
		ID3DXBuffer* pPixelErrorMsg;
		char * code = VsGenerator();
		HRESULT hr = D3DXCompileShader( code, 
			(UINT)strlen( code ),
			NULL, 
			NULL, 
			"main", 
			"vs_2_0", 
			0, 
			&pPixelShaderCode, 
			&pPixelErrorMsg,
			NULL );
		if( FAILED(hr) )
		{
			if( pPixelErrorMsg )
			{	
				OutputDebugString( (char*)pPixelErrorMsg->GetBufferPointer() );
#if _DEBUG
				DebugBreak();
#endif
			}
		}

		OutputDebugStringA( code );

		// Create pixel shader.
		GLImpl.device->CreateVertexShader( 
			(DWORD*)pPixelShaderCode->GetBufferPointer(), 
			&vs );

		// add to cache
		vsCache[1] = vs;
	}

	// apply shader
	GLImpl.device->SetVertexDeclaration( v_decl );
	GLImpl.device->SetVertexShader( vs );
	GLImpl.device->SetPixelShader( ps );
}

void GlShaderManager::SetTexenv(int tmu, int env) {
	Texenv.u8[tmu] = env;
	Dirty = 0xFFFFFFFF;
}

/**

case GL_MODULATE:
v = 2;

case GL_DECAL:
v = 3;

case GL_BLEND:
v = 4;

case GL_REPLACE:
v = 1;

*/
char * GlShaderManager::PsGenerator()
{
	char *p = buffer;
	WRITE(" sampler2D texture0 : register(s0);     ");
	WRITE(" sampler2D texture1 : register(s1);     ");
//	WRITE(" bool bClip : register(b0);     ");

	WRITE(" struct PixelShaderInput    ");
	WRITE(" {														    ");
	WRITE(" 	float2 uv0: TEXCOORD0;			// texture 0 uv    ");
	WRITE(" 	float2 uv1: TEXCOORD1;			// texture 1 uv    ");
	WRITE(" 	float4 color: COLOR;			// color    ");
//	WRITE(" 	float1 clip: TEXCOORD2;			// clip    ");
	WRITE(" };    ");

	WRITE(" float4 main(PixelShaderInput input): COLOR { ");
	WRITE(" float4 output; ");

//	WRITE(" if(bClip)	");
//	WRITE("		clip(input.clip.x); ");

	if (GLImpl.tmus[0].enabled && GLImpl.tmus[0].boundtexture) {
		switch(Texenv.u8[0]) {
		case TENV_REPLACE: // replace
			WRITE(" output = tex2D(texture0, input.uv0); ");
			break;
		case TENV_DECAL: // GL_DECAL
			WRITE(" float4 t = tex2D(texture0, input.uv0); ");
			WRITE(" float4 p = input.color; ");
			WRITE(" output = float4(1.0 - t.a * p.rgb + t.a, p.a);");
			break;
		case TENV_BLEND: // GL_BLEND
			WRITE(" float4 t = tex2D(texture0, input.uv0); ");
			WRITE(" float4 p = input.color; ");
			WRITE("	output =  float4((1.0 - t.rgb) * p.rgb + t.rgb, p.a * t.a);"); 
			break;
		case TENV_ADD: // GL_ADD
			WRITE(" output = tex2D(texture0, input.uv0) + input.color; ");
			break;
		default:
		case TENV_MODULATE: // modulate
			WRITE(" output = input.color * tex2D(texture0, input.uv0); ");
			break;
		}
	} else {
		WRITE(" output = input.color; ");
	}

	WRITE(" return output; ");
	WRITE("} ");

	return buffer;
}
char * GlShaderManager::VsGenerator()
{
	char *p = buffer;

	WRITE(" float4x4 modelview_matrix : register (c%d);   ", MATMODELVIEW);
	WRITE(" float4x4 projection_matrix : register (c%d);  ", MATPROJECTION);
	//WRITE(" float4 znear : register (c%d);  ", 16);
	//WRITE(" float4 zfar : register (c%d);  ", 17);
	//WRITE(" float4 clipplane: register (c%d);			  ", PARAM_CLIPPLANE_0);
	WRITE("                                               ");
	WRITE(" struct VS_IN                                  ");
	WRITE(" {                                             ");
	WRITE("     float4 position : POSITION;               ");
	WRITE("     float2 texcoord0 : TEXCOORD0;             ");
	WRITE("     float2 texcoord1 : TEXCOORD1;             ");
	WRITE("     float4 color0   : COLOR0;                 ");
	WRITE(" };                                            ");
	WRITE("                                               ");
	WRITE(" struct VS_OUT                                 ");
	WRITE(" {                                             ");
	WRITE("     float4 position : POSITION;               ");
	WRITE("     float2 texcoord0 : TEXCOORD0;             ");
	WRITE("     float2 texcoord1 : TEXCOORD1;             ");
	WRITE("     float4 color0   : COLOR0;                 ");
	//WRITE(" 	float1 clip: TEXCOORD2;			// clip    ");
	WRITE(" };                                            ");
	WRITE("                                               ");
	WRITE(" VS_OUT main( VS_IN In )                       ");
	WRITE(" {                                             ");
	WRITE("     VS_OUT Out;                               ");	
	WRITE("     Out.position = mul(mul(projection_matrix, modelview_matrix), In.position);  ");
	WRITE("     Out.color0 = In.color0;                   ");
	WRITE("     Out.texcoord0 = In.texcoord0;             ");
	WRITE("     Out.texcoord1 = In.texcoord1;             ");
	//WRITE("     Out.position.z = 1-(Out.position.z*2);	  ");
	//WRITE("		Out.position.z = (1+Out.position.z)/2;	  ");
	//WRITE("		Out.position.z = (Out.position.z)/2;	  ");
	//WRITE("		Out.clip.x = dot(Out.position, clipplane) ;	");
	//WRITE("		Out.position.z = 0.5*(zfar-znear)*Out.position.z + 0.5 * (zfar+znear) ;	  ");
	WRITE("     return Out;                               ");
	WRITE(" }                                             ");

	return buffer;
}