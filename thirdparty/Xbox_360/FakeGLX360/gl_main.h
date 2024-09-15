#pragma once

#include "fakeglx360.h"
#include <xtl.h>
#include <xgraphics.h>

/** based on 
 * http://quakeone.com/mh/gl_fakegl.c
 * http://fossies.org/dox/wine-1.4.1/d3dx9__36_2math_8c_source.html
 **/ 

// Speed hack for Quake 3
#define USE_VB 0

#ifndef USE_VB
#define USE_VB 0
#endif

#define XE_MAX_VERTICES	8*1024*1024
#define XE_MAX_TMUS 2

// at end of the file
class CGLImpl;
extern CGLImpl GLImpl;


/***********************************************************************
 * Vertices
 ***********************************************************************/ 
#pragma pack(push,1)
typedef struct {	
    float x, y, z, w;
    // texture 0
    float u0, v0;
    // texture 1    
    float u1, v1;
    // color
    unsigned int color;
} glVerticesFormat_t;
#pragma pack(pop)

typedef struct {
	float u, v;
} glTextCoord_t; 

typedef union {
	struct {
		unsigned char a, r, g, b;
	} u8;
	unsigned int u32;
	float f;
} glColor_t; 

typedef struct {
	struct {
#if !USE_VB
float cache[65536];
#endif
		float * data;
		int number;
		int prev;
		int size() {
			return number - prev;
		}
		void clear() {
			prev = number = 0;
		}
		int offset() {
			return prev * sizeof(glVerticesFormat_t);
		}
		void begin() {
#if !USE_VB
			data = cache;
#endif
			prev = number;
		}
		void next(int n = 1) {
			number+=n;
		}
	} vertices;

	struct {
#if !USE_VB
unsigned int cache[65536];
#endif
		unsigned int * data;
		int number;
		int prev;
		int size() {
			return number - prev;
		}
		void clear() {
			prev = number = 0;
		}
		void begin() {
#if !USE_VB
			data = cache;
#endif
			prev = number;
		}
		void next(int n = 1) {
			number+=n;
		}
	}  indices;

	glColor_t color;
	glTextCoord_t uv[2];

	void clear() {
		indices.clear();
		vertices.clear();
		color.u32 = 0;
	}
	
	GLenum primitive_type;
} xe_prim_t;

/***********************************************************************
 * Textures
 ***********************************************************************/
class GLTexture;

struct glXeSurface_t{
	// OpenGL texture id
	int glnum;
	// Used by glTexSubImage2D
	GLenum internalformat;

	// real texture
	GLTexture * teximg;

	unsigned int wrap_u;
	unsigned int wrap_v;
	unsigned int min_filter;
    unsigned int mag_filter;
    unsigned int mip_filter;
    unsigned int max_anisotropy;

	// refresh texture cache only when need
	unsigned int dirty;

	glXeSurface_t() {
		// dirty
		dirty = 0xFFFFFFFF;

		wrap_u = wrap_v = D3DTADDRESS_WRAP;
		min_filter = mag_filter = D3DTEXF_POINT;
		mip_filter = D3DTEXF_NONE;
		max_anisotropy = 1;
		teximg = NULL;
		glnum = -1; // empty
	}
};

struct glXeTmu_t{
	glXeSurface_t * boundtexture;
	int enabled;
	
	int texture_env_mode;

	int texenvdirty;
	int texparamdirty;
};

/***********************************************************************
 * Matrices
 ***********************************************************************/
 #define MAX_MATRIX_STACK 32
 
typedef enum{
	MATMODELVIEW	= 0, // register see opengl.hlsl
	MATPROJECTION	= 4,
	PARAM_CLIPPLANE_0 = 8,
} TRANSFORM_TYPE;

typedef struct
{
	int dirty;
	int stackdepth;
	TRANSFORM_TYPE usage;
	XMMATRIX stack[MAX_MATRIX_STACK];
//	matrix4x4 stack[MAX_MATRIX_STACK];
} xe_matrix_t;

/***********************************************************************
 * States
 ***********************************************************************/
enum {
	DIRTY_BLEND = 0x1,
	DIRTY_ALPHA = 0x2,
	DIRTY_DEPTH = 0x4,
	DIRTY_STENCIL = 0x8,
	DIRTY_VIEWPORT = 0x10,
	DIRTY_CULL = 0x20,
	DIRTY_FILLMODE = 0x40,
	DIRTY_SCISSOR = 0x80,
	DIRTY_ZBIAS = 0x100,
	DIRTY_CLIP = 0x200,
	DIRTY_STATE_ALL = 0xFFFFFFFF
};


typedef struct {
	// blending
	unsigned int blend_enable;
	unsigned int blend_op;
	unsigned int blend_src;
	unsigned int blend_dst;
	
	// Alpha test
	unsigned int alpha_test_enabled;
	unsigned int alpha_test_func;
	unsigned int alpha_test_ref;
	
	// Depth
	unsigned int z_enable;
	unsigned int z_mask;
	unsigned int z_func;
		
	// stencil
	unsigned int stencil_enabled;		// Xe_SetStencilEnable
	unsigned int stencil_func_face;		// Xe_SetStencilFunc
	unsigned int stencil_func;			// Xe_SetStencilFunc
	unsigned int stencil_op_face;		// Xe_SetStencilOp
	unsigned int stencil_op_fail;		// Xe_SetStencilOp
	unsigned int stencil_op_zfail;		// Xe_SetStencilOp
	unsigned int stencil_op;				// Xe_SetStencilOp
	unsigned int stencil_ref_face;		// Xe_SetStencilRef
	unsigned int stencil_ref;			// Xe_SetStencilRef
	unsigned int stencil_mask_face;		// Xe_SetStencilMask
	unsigned int stencil_mask;			// Xe_SetStencilMask
	unsigned int stencil_write_face;		// Xe_SetStencilWriteMask
	unsigned int stencil_write_mask;		// Xe_SetStencilWriteMask
	
	// polygon offset
	unsigned int polygon_offset_enabled;
	float zoffset;
	float woffset;
	
	// scissor
	unsigned int scissor_enabled;
	unsigned int scissor_h;
	unsigned int scissor_w;
	unsigned int scissor_x;
	unsigned int scissor_y;	
	
	// other
	unsigned int fill_mode_front;
	unsigned int fill_mode_back;

	// cull
	unsigned int cull_order;
	unsigned int cull_enabled;
	unsigned int cull_face;

	// color mask
	unsigned int color_mask;

	// viewport		
	D3DVIEWPORT9 viewport;
	
	// clear
	unsigned int clear_color;
	float clear_depth_value;
	unsigned int clear_stencil_value;
	
	// clip planes
	float clipplane[6][4];
	unsigned int clipplane_enabled;
	
	int render_width;
	int render_height;

	// Reupload all states
	unsigned int dirty;
	
} xe_states_t;

enum {
	XE_ENV_MODE_DISABLED = 0,
	XE_ENV_MODE_REPLACE,
	XE_ENV_MODE_MODULATE,
	XE_ENV_MODE_ADD,		// not implemented
	XE_ENV_MODE_DECAL,		// not implemented
	XE_ENV_MODE_BLEND,		// not implemented
	XE_ENV_MAX
};

D3DRECT msaa_4xaa[];

class CGLImpl {
public:
	void Init();
	void Swap();
	void Begin();
	void End();
	void BeginTiling();
	void EndTiling();
	
	void SubmitVertexes();

	void InitTexture(glXeSurface_t *tex);
	glXeSurface_t *CGLImpl::AllocTexture(void);

public:
	void InitStates();
	void UpdateStates();
	void ResetStates();
	void InitTextures();
	void CGLImpl::ReleaseTextures (void);

	void InitializeMatrices();
	void InitializeMatrix(xe_matrix_t *m);
	void CheckDirtyMatrix(xe_matrix_t *m);
	void ResetMatrixDirty();
	void ApplyTextures();

public:	
	xe_prim_t prim;
	xe_states_t state;
	xe_matrix_t projection_matrix;
	xe_matrix_t modelview_matrix;

	XMMATRIX depth_fix;
	
	//	
	int use_aa;
	int current_tmu;
	glXeSurface_t * surfaces;
	glXeTmu_t tmus[XE_MAX_TMUS];

public:	
	D3DDevice* device;
private:
public:
	D3DVertexShader * pVertexShader;
	D3DPixelShader * pPixelColorShader;
	D3DPixelShader * pPixelModulateShader;
	D3DPixelShader * pPixelModulateShader2;
	D3DPixelShader * pPixelTextureShader;
	D3DPixelShader * pCurrentPs;
	D3DPixelShader * pCurrentTexturedPs;
	D3DVertexBuffer * pVbGL;
	D3DIndexBuffer * pIbGL;
};

extern CGLImpl GLImpl;


class GLTexture {
private:
	LPDIRECT3DTEXTURE9 tex;
	struct {
		D3DLOCKED_RECT rect;	
		D3DSURFACE_DESC desc;
		D3DFORMAT format;
		D3DFORMAT uncompressed_format;
		int uncompressed_pitch;
		DWORD XgFlags;
		BYTE * data;
		int compressed;
		int tiled;
		int pPitch;
		int base_width;
		int base_height;
	} _info;
public:
	GLTexture(int width, int height, D3DFORMAT format);
	~GLTexture();
	void GetLevelDesc(int level, D3DSURFACE_DESC *pDesc);
	void setTexture(int sampler);

	void lockTexture(int lvl);
	void unlockTexture(int lvl);

	BYTE * getData();
	int getPitch();
};

/***********************************************************************
 * Global
 ***********************************************************************/
enum {
	TENV_MODULATE,
	TENV_DECAL,
	TENV_BLEND,
	TENV_REPLACE,
	TENV_ADD
};

/***********************************************************************
 * Utils
 ***********************************************************************/
void xe_gl_error(const char * format, ...);
void xe_gl_log(const char * format, ...);

extern "C" void XDKGlInit();
extern "C" void XDKGlDisplay();
extern "C" void XDKGlBeginFrame();
extern "C" void XDKGlEndFrame();
extern "C" void XDKGlGetScreenSize(int * width, int * height);

