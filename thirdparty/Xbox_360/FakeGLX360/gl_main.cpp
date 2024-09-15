#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>

#include "fakeglx360.h"
#include "gl_main.h"

CGLImpl GLImpl;

D3DRECT msaa_4xaa[] = {
	{   0, 0,  320, 720 },
	{ 320, 0,  640, 720 },
	{ 640, 0,  960, 720 },
	{ 960, 0, 1280, 720 }
};

static LPDIRECT3DTEXTURE9 pFrontBuffer;
static LPDIRECT3DTEXTURE9 pResolveBuffer;
static LPDIRECT3DSURFACE9 pRenderTarget;
static LPDIRECT3DSURFACE9 pDepthStencilTarget;

void CGLImpl::Init()
{
	use_aa = 1;// not working

	// The device-creation presentation params with reasonable defaults
	D3DPRESENT_PARAMETERS d3dpp =
	{
		1280,               // BackBufferWidth;
		720,                // BackBufferHeight;
		D3DFMT_A8R8G8B8,    // BackBufferFormat;
		1,                  // BackBufferCount;
		D3DMULTISAMPLE_NONE,// MultiSampleType;
		0,                  // MultiSampleQuality;
		D3DSWAPEFFECT_FLIP, // SwapEffect;
		NULL,               // hDeviceWindow;
		FALSE,              // Windowed;
		TRUE,               // EnableAutoDepthStencil;
		D3DFMT_D24S8,       // AutoDepthStencilFormat;
		0,                  // Flags;
		0,                  // FullScreen_RefreshRateInHz;
		D3DPRESENT_INTERVAL_DEFAULT, // FullScreen_PresentationInterval;
	};

	if (use_aa) {
		//d3dpp.BackBufferFormat = ( D3DFORMAT )MAKESRGBFMT( D3DFMT_A8R8G8B8 );
		d3dpp.BackBufferCount = 0;
		d3dpp.EnableAutoDepthStencil = FALSE;
		d3dpp.DisableAutoBackBuffer = TRUE;
		d3dpp.DisableAutoFrontBuffer = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	}

	XDKGlGetScreenSize((int*)&d3dpp.BackBufferWidth, (int*)&d3dpp.BackBufferHeight);

	HRESULT hr;
	// Create Direct3D
    LPDIRECT3D9 pD3D = Direct3DCreate9( D3D_SDK_VERSION );

    // Create the D3D device
    if( FAILED( hr = pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                         NULL,
										 &d3dpp, &device ) ) )
    {
        printf( "Could not create D3D device!\n" );
        DebugBreak();
    }

    pD3D->Release();

	// init aa surface
	if (use_aa) {
		D3DSURFACE_PARAMETERS params = {0};
		int tile_w = 1280/4;
		int tile_h = 720;

		// render target
		params.Base = 0;
		device->CreateRenderTarget( tile_w, tile_h, D3DFMT_X8R8G8B8, D3DMULTISAMPLE_4_SAMPLES, 0, 0, &pRenderTarget, &params );

		// stencil+depth surface
		params.Base = pRenderTarget->Size / GPU_EDRAM_TILE_SIZE;
		params.HierarchicalZBase = D3DHIZFUNC_GREATER_EQUAL;
		device->CreateDepthStencilSurface( tile_w, tile_h, D3DFMT_D24S8, D3DMULTISAMPLE_4_SAMPLES, 0, 0, &pDepthStencilTarget, &params );

		device->CreateTexture( 1280, 720, 1, 0,
			( D3DFORMAT )MAKESRGBFMT( D3DFMT_LE_X8R8G8B8 ),
			D3DPOOL_DEFAULT,
			&pFrontBuffer,
			NULL );

		device->CreateTexture( 1280, 720, 1, 0,
			( D3DFORMAT )MAKESRGBFMT( D3DFMT_X8R8G8B8 ),
			D3DPOOL_DEFAULT,
			&pResolveBuffer,
			NULL );
	}

	// init stuff
	InitTextures();
	InitializeMatrices();
	InitStates();

	
	state.render_height = d3dpp.BackBufferHeight;
	state.render_width = d3dpp.BackBufferWidth;

#if USE_VB
	// create vb and stuff
	device->CreateVertexBuffer( 
		1024*1024*8,// 8 Mo
		0, 
		NULL,
		NULL, 
		&pVbGL, 
		NULL 
	);

	device->CreateIndexBuffer(
		1024*1024*8,// 8 Mo
		0,
		D3DFMT_INDEX32,
		NULL,
		&pIbGL,
		NULL);

	// refresh ib/vb cache
	GLImpl.pVbGL->Lock(0, 8*1024*1024, (void**)&GLImpl.prim.vertices.data, 0);
	//GLImpl.pVbGL->Unlock();
	GLImpl.pIbGL->Lock(0, 8*1024*1024, (void**)&GLImpl.prim.indices.data, 0);
	//GLImpl.pIbGL->Unlock();
#endif

	if (use_aa) {
		device->SetRenderTarget( 0, pRenderTarget );
		device->SetDepthStencilSurface( pDepthStencilTarget );
	}

	//device->SynchronizeToPresentationInterval();
	if (use_aa == 0) {
		device->GetFrontBuffer(&pFrontBuffer);
	}
}

void CGLImpl::Swap() {
#if USE_VB
	pVbGL->Unlock();	
	pIbGL->Unlock();	
#endif
	// d3dpresent
	device->Present(NULL, NULL, NULL, NULL);

	device->SetIndices(NULL);
	device->SetStreamSource(0, 0, 0, 0);

#if USE_VB
	// refresh ib/vb cache
	pVbGL->Lock(0, 8*1024*1024, (void**)&GLImpl.prim.vertices.data, D3DLOCK_NOOVERWRITE);	
	pIbGL->Lock(0, 8*1024*1024, (void**)&GLImpl.prim.indices.data, D3DLOCK_NOOVERWRITE);
	
	device->SetStreamSource(0, GLImpl.pVbGL, 0, sizeof(glVerticesFormat_t));
#endif

	// reset
	ResetMatrixDirty();
	prim.clear();

	state.dirty = 0xFFFFFFFF;
}

void CGLImpl::BeginTiling() {
	const D3DVECTOR4 clearColor = { 0.f, 0.f, 0.f, 1.f };
	device->SetRenderTarget( 0, pRenderTarget );
	device->SetDepthStencilSurface( pDepthStencilTarget );
	device->BeginTiling( 0, ARRAYSIZE(msaa_4xaa), msaa_4xaa, &clearColor, 1, 0 );
	device->SetPredication( 0 );
	//device->BeginZPass( 0 );
}

void CGLImpl::EndTiling() {
	const D3DVECTOR4 clearColor = { 0.f, 0.f, 0.f, 1.f };
	///device->EndZPass();
	device->SetPredication( 0 );
	device->EndTiling( D3DRESOLVE_RENDERTARGET0 |
		D3DRESOLVE_ALLFRAGMENTS |
		D3DRESOLVE_CLEARRENDERTARGET |
		D3DRESOLVE_CLEARDEPTHSTENCIL,
		NULL, pFrontBuffer,
		&clearColor, 1.0f, 0L, NULL );

	device->SetIndices(NULL);
	device->SetStreamSource(0, 0, 0, 0);

	device->SynchronizeToPresentationInterval();
	/*
    // Resolve the rendered scene back to the front buffer.
    device->Resolve( D3DRESOLVE_RENDERTARGET0 | D3DRESOLVE_CLEARRENDERTARGET,
                           NULL,
                           pFrontBuffer,
                           NULL,
                           0, 0,
                           &clearColor,
                           1.0f, 0, NULL );
*/
    // Swap to the current front buffer, so we can see it on screen.
    device->Swap( pFrontBuffer, NULL );

	// reset states
	ResetMatrixDirty();
	prim.clear();

	state.dirty = 0xFFFFFFFF;
}

extern "C" void XDKQ3BeginFrame()
{
	if (GLImpl.use_aa == 0)
		return;

	GLImpl.BeginTiling();
}

//-------------------------------------------------------------------------------------
//	Init opengl
//-------------------------------------------------------------------------------------
void XDKGlInit() {
	GLImpl.Init();
}

//-------------------------------------------------------------------------------------
//	Present
//-------------------------------------------------------------------------------------
void XDKGlDisplay() {	
	if (GLImpl.use_aa) {
		GLImpl.EndTiling();
	} else {
		GLImpl.Swap();
	}
}

//-------------------------------------------------------------------------------------
//	Start a frame
//-------------------------------------------------------------------------------------
void XDKGlBeginFrame() {
	// d3dwait
#if USE_VB
	// refresh ib/vb cache
	GLImpl.pVbGL->Lock(0, 8*1024*1024, (void**)&GLImpl.prim.vertices.data, 0);	
	GLImpl.pIbGL->Lock(0, 8*1024*1024, (void**)&GLImpl.prim.indices.data, 0);
#endif
	
	// d3dpresent
	GLImpl.device->SynchronizeToPresentationInterval();

	GLImpl.device->Swap(pFrontBuffer, NULL);

	// reset
	GLImpl.ResetStates();
	GLImpl.ResetMatrixDirty();
	GLImpl.prim.clear();
}

//-------------------------------------------------------------------------------------
//	End a frame
//-------------------------------------------------------------------------------------
void XDKGlEndFrame() {
	const static D3DVECTOR4 clear_color = { 0, 0, 0, 0 };
	// Resolve the final image to the front buffer.
	GLImpl.device->Resolve(	D3DRESOLVE_RENDERTARGET0, 
						NULL, 
						pFrontBuffer, 
						NULL, 0, 0,
						&clear_color, 1.0f, 0, NULL );
}

//-------------------------------------------------------------------------------------
//	Get screen size
//-------------------------------------------------------------------------------------
void XDKGlGetScreenSize(int * width, int * height) {
	XVIDEO_MODE VideoMode;
	XGetVideoMode( &VideoMode );
	//*width = VideoMode.dwDisplayWidth;
	//*height = VideoMode.dwDisplayHeight;
	*width = 1280;
	*height = 720;
}

void xe_gl_error(const char * format, ...)
{
	printf(format);
}
void xe_gl_log(const char * format, ...)
{
	printf(format);
}

extern "C" void XDKSetGamma(const unsigned char* gammaTable)
{
		D3DGAMMARAMP gammaRamp;

		for(int i = 0; i < 256; i++ ) 
		{
			WORD value = gammaTable[i];
			value = value + (value << 8); // * 257
			gammaRamp.red[i] = value;
			gammaRamp.green[i] = value;
			gammaRamp.blue[i] = value;
		}

		GLImpl.device->SetGammaRamp(0, 0, &gammaRamp);
}