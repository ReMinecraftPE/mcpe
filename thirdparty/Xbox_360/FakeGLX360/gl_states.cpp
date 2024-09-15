#include "gl_main.h"
#include <assert.h>

void glShadeModel (GLenum mode)
{
	// ?
}

/***********************************************************************
 * Scissor
 ***********************************************************************/
void glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
	GLImpl.state.scissor_x = x;
	GLImpl.state.scissor_y = y;
	GLImpl.state.scissor_w = width;
	GLImpl.state.scissor_h = height;
	GLImpl.state.dirty |= DIRTY_SCISSOR;
}

/***********************************************************************
 * Clear
 ***********************************************************************/
void glClear (GLbitfield mask)
{
	DWORD flags = 0;

	if (mask & GL_COLOR_BUFFER_BIT) flags |= D3DCLEAR_TARGET;
	if (mask & GL_DEPTH_BUFFER_BIT) flags |= D3DCLEAR_ZBUFFER;
	if (mask & GL_STENCIL_BUFFER_BIT) flags |= D3DCLEAR_STENCIL;

	//GLImpl.UpdateStates();

	if (GLImpl.use_aa) {
		// todo
		// const D3DVECTOR4 clearColor = { 0.f, 0.f, 0.f, 1.f };
		// GLImpl.device->BeginTiling( 0, ARRAYSIZE(g_tiles), g_tiles, &clearColor, 1, 0 );
		// GLImpl.device->BeginTiling( 0, 4, msaa_4xaa, &clearColor, 1, 0 );
		GLImpl.device->Clear(
			0L, NULL, flags,
			GLImpl.state.clear_color, 
			GLImpl.state.clear_depth_value, 
			GLImpl.state.clear_stencil_value & 0x000000FF 
		);
	} else {
		GLImpl.device->Clear(
			0L, NULL, flags,
			GLImpl.state.clear_color, 
			GLImpl.state.clear_depth_value, 
			GLImpl.state.clear_stencil_value & 0x000000FF 
		);
	}
}

void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	unsigned int c = D3DCOLOR_ARGB((int)(alpha * 255), (int)(red * 255), (int)(green * 255), (int)(blue * 255));
	GLImpl.state.clear_color = c;
}

void glClearDepth(GLclampd depth)
{
	GLImpl.state.clear_depth_value = (float)depth;
	//glClear(GL_DEPTH_BUFFER_BIT);
}

void glClearStencil(GLint s)
{
	GLImpl.state.clear_stencil_value = s;
	//glClear(GL_STENCIL_BUFFER_BIT);
}

/***********************************************************************
 * Cull
 ***********************************************************************/
void glFrontFace (GLenum mode)
{
	GLImpl.state.cull_order = mode;
	GLImpl.state.dirty |= DIRTY_CULL;
}
void glCullFace (GLenum mode)
{
	GLImpl.state.cull_face = mode;
	GLImpl.state.dirty |= DIRTY_CULL;
}
/***********************************************************************
 * Depth
 ***********************************************************************/
static D3DCMPFUNC ConvertComparison(GLenum comparison)
{
    D3DCMPFUNC d3dComp = D3DCMP_ALWAYS;
    switch (comparison)
    {
      case GL_NEVER:    d3dComp = D3DCMP_NEVER;        break;
      case GL_ALWAYS:   d3dComp = D3DCMP_ALWAYS;       break;
      case GL_LESS:     d3dComp = D3DCMP_LESS;         break;
      case GL_LEQUAL:   d3dComp = D3DCMP_LESSEQUAL;    break;
      case GL_EQUAL:    d3dComp = D3DCMP_EQUAL;        break;
      case GL_GREATER:  d3dComp = D3DCMP_GREATER;      break;
      case GL_GEQUAL:   d3dComp = D3DCMP_GREATEREQUAL; break;
      case GL_NOTEQUAL: d3dComp = D3DCMP_NOTEQUAL;     break;
    }

    return d3dComp;
}

void glDepthFunc (GLenum func)
{
	GLImpl.state.z_func = ConvertComparison(func);
	GLImpl.state.dirty |= DIRTY_DEPTH;
}

void glDepthMask (GLboolean flag)
{
	GLImpl.state.z_mask = (flag == GL_TRUE) ? 1 : 0;
	GLImpl.state.dirty |= DIRTY_DEPTH;
}

void glAlphaFunc (GLenum func, GLclampf ref)
{
	GLImpl.state.alpha_test_func = ConvertComparison(func);
	GLImpl.state.alpha_test_ref = ref * 255;
	GLImpl.state.dirty |= DIRTY_ALPHA;
}


static inline unsigned int getBlendFunc(GLenum func) {
	switch (func) {
	case GL_ZERO:
		return D3DBLEND_ZERO;
	case GL_ONE:
		return D3DBLEND_ONE;
	case GL_SRC_COLOR:
		return D3DBLEND_SRCCOLOR;
	case GL_ONE_MINUS_SRC_COLOR:
		return D3DBLEND_INVSRCCOLOR;
	case GL_DST_COLOR:
		return D3DBLEND_DESTCOLOR;
	case GL_ONE_MINUS_DST_COLOR:
		return D3DBLEND_INVDESTCOLOR;
	case GL_SRC_ALPHA:
		return D3DBLEND_SRCALPHA;
	case GL_ONE_MINUS_SRC_ALPHA:
		return D3DBLEND_INVSRCALPHA;
	case GL_DST_ALPHA:
		return D3DBLEND_DESTALPHA;
	case GL_ONE_MINUS_DST_ALPHA:
		return D3DBLEND_INVDESTALPHA;
		/*
	case GL_CONSTANT_COLOR:
		return D3DBLEND_BLENDFACTOR;
	case GL_ONE_MINUS_CONSTANT_COLOR:
		return D3DBLEND_INVBLENDFACTOR;
	case GL_CONSTANT_ALPHA:
		return D3DBLEND_CONSTANTALPHA;
	case GL_ONE_MINUS_CONSTANT_ALPHA:
		return D3DBLEND_INVCONSTANTALPHA;
		*/
	case GL_SRC_ALPHA_SATURATE:
		return D3DBLEND_SRCALPHASAT;
	}; 

	return 0;
}

/***********************************************************************
 * Blend
 ***********************************************************************/
void glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	GLImpl.state.blend_src = getBlendFunc(sfactor);
	GLImpl.state.blend_dst = getBlendFunc(dfactor);
	GLImpl.state.dirty |= DIRTY_BLEND;
}

/***********************************************************************
 * Blend
 ***********************************************************************/
void GlEnableDisable(GLenum cap, int enable)
{
	switch (cap)
	{
	case GL_SCISSOR_TEST:
		GLImpl.state.scissor_enabled = enable;
		GLImpl.state.dirty |= DIRTY_SCISSOR;
		break;
		
	case GL_BLEND:
		GLImpl.state.blend_enable = enable;
		GLImpl.state.dirty |= DIRTY_BLEND;
		break;
		
	case GL_ALPHA_TEST:
		GLImpl.state.alpha_test_enabled = enable;
		GLImpl.state.dirty |= DIRTY_ALPHA;
		break;

	case GL_TEXTURE_2D:
		GLImpl.tmus[GLImpl.current_tmu].enabled = enable;
		return;

	case GL_CULL_FACE:
		GLImpl.state.cull_enabled = enable;
		GLImpl.state.dirty |= DIRTY_CULL;
		break;
		
	case GL_STENCIL_TEST:
		GLImpl.state.stencil_enabled = enable;
		GLImpl.state.dirty |= DIRTY_STENCIL;
		break;
		
	case GL_DEPTH_TEST:
		GLImpl.state.z_enable = enable;
		GLImpl.state.dirty |= DIRTY_DEPTH;
		break;
		
	case GL_FOG:
		return;
		
	case GL_CLIP_PLANE0:
	case GL_CLIP_PLANE1:
	case GL_CLIP_PLANE2:
	case GL_CLIP_PLANE3:
	case GL_CLIP_PLANE4:
	case GL_CLIP_PLANE5:
		GLImpl.state.clipplane_enabled = enable;
		GLImpl.state.dirty |= DIRTY_CLIP;
		break;
		
	case GL_POLYGON_OFFSET_POINT:
	case GL_POLYGON_OFFSET_FILL:
	case GL_POLYGON_OFFSET_LINE:
		GLImpl.state.polygon_offset_enabled = enable;
		GLImpl.state.dirty |= DIRTY_ZBIAS;
		break;
		
	default:
		return;
	}
}

void glEnable(GLenum cap)
{
	GlEnableDisable(cap, 1);
}

void glDisable(GLenum cap)
{
	GlEnableDisable(cap, 0);
}

/***********************************************************************
 * Stencil
 ***********************************************************************/
static int stencil_back_and_front = 3;

static inline int Gl_Stencil_2_Xe(int value) {
	int ret = D3DSTENCILOP_KEEP;
#if 0 // ignore for now
	switch (value) {
		case GL_KEEP:
			ret = D3DSTENCILOP_KEEP;
			break;
		case GL_ZERO:
			ret = D3DSTENCILOP_ZERO;
			break;
		case GL_REPLACE:
			ret = D3DSTENCILOP_REPLACE;
			break;
		case GL_INCR:
			ret = D3DSTENCILOP_INCRSAT;
			break;		
		case GL_DECR:
			ret = D3DSTENCILOP_DECRSAT;
			break;		
		case GL_INVERT:
			ret = D3DSTENCILOP_INVERT;
			break;
		case GL_INCR_WRAP:
			ret = D3DSTENCILOP_INCR;
			break;
		case GL_DECR_WRAP:
			ret = D3DSTENCILOP_DECR;
			break;
		default:
			break;
	}
#endif
	return ret;
}
 
void glStencilFunc(GLenum func,	GLint ref,GLuint mask)
{
	GLImpl.state.dirty |= DIRTY_STENCIL;

	GLImpl.state.stencil_op_face = stencil_back_and_front;
	GLImpl.state.stencil_write_face = stencil_back_and_front;
	GLImpl.state.stencil_func_face = stencil_back_and_front;
	GLImpl.state.stencil_ref_face = stencil_back_and_front;

	GLImpl.state.stencil_func = ConvertComparison(func);
	GLImpl.state.stencil_mask = mask;
	GLImpl.state.stencil_ref = ref;
}

void glStencilMask(GLuint mask)
{
	GLImpl.state.dirty |= DIRTY_STENCIL;
	GLImpl.state.stencil_write_mask = mask;
}

void glStencilOp(GLenum sfail,	GLenum  dpfail,	GLenum  dppass)
{
	GLImpl.state.dirty |= DIRTY_STENCIL;
	GLImpl.state.stencil_op = Gl_Stencil_2_Xe(sfail);
	GLImpl.state.stencil_op_fail = Gl_Stencil_2_Xe(dpfail);
	GLImpl.state.stencil_op_zfail = Gl_Stencil_2_Xe(dppass);
}
/***********************************************************************
 * Misc
 ***********************************************************************/
void glPolygonMode (GLenum face, GLenum mode)
{
	/*
	int xmode = 0;
	
	if (mode == GL_LINE)
		xmode = D3DFILL_WIREFRAME;
	else if (mode == GL_POINT)
		xmode = D3DFILL_POINT;
	else 
		xmode = D3DFILL_SOLID;
	
	if (face == GL_FRONT)
		xe_state.fill_mode_front = xmode;
	else if (face == GL_BACK)
		xe_state.fill_mode_back = xmode;
	else
		xe_state.fill_mode_front = xe_state.fill_mode_back = xmode;
		
	xe_state.dirty = 1;
	*/ 
}

void glPolygonOffset (GLfloat factor, GLfloat units)
{
	GLImpl.state.zoffset = units * 0.000125f;
	GLImpl.state.woffset = -factor * 0.0025f;
	GLImpl.state.dirty |= DIRTY_ZBIAS;
}

void glClipPlane(GLenum plane, const GLdouble *equation) {
	int idx = GL_CLIP_PLANE0 - plane;

	GLImpl.state.clipplane[idx][0] = (GLfloat)equation[0];
	GLImpl.state.clipplane[idx][1] = (GLfloat)equation[1];
	GLImpl.state.clipplane[idx][2] = (GLfloat)equation[2];
	GLImpl.state.clipplane[idx][3] = (GLfloat)equation[3];

	GLImpl.state.dirty |= DIRTY_CLIP;
}

void glLineWidth(GLfloat width)
{
	
}

void glCallList(GLuint list)
{
	
}

void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	unsigned int mask = (red?D3DCOLORWRITEENABLE_RED:0) | (blue?D3DCOLORWRITEENABLE_BLUE:0) | (green?D3DCOLORWRITEENABLE_GREEN:0) | (alpha?D3DCOLORWRITEENABLE_ALPHA:0);
	GLImpl.state.color_mask = mask;
	// direct
	GLImpl.device->SetRenderState(D3DRS_COLORWRITEENABLE, mask);
}

void glReadPixels(GLint x,	GLint y, GLsizei width,	GLsizei height,	GLenum format, GLenum type,
 	GLvoid * data)
{
	
}

void glGetBooleanv(	GLenum pname, GLboolean * params)
{
	 glGetIntegerv(pname, (GLint *)params);
}

void glGetDoublev(GLenum pname,	GLdouble * params)
{
	glGetIntegerv(pname, (GLint *)params);
}

void glGetIntegerv(GLenum pname, GLint * params)
{
// here we only bother getting the values that glquake uses
	switch (pname)
	{
	case GL_MAX_TEXTURE_SIZE:
		params[0] = 2048;
		break;

	case GL_VIEWPORT:
		// todo
		break;

	case GL_STENCIL_BITS:
		params[0] = 8; // ?
		break;

	case GL_MAX_TEXTURE_UNITS_ARB:
		params[0] = 2;
		break;

	case GL_COLOR_WRITEMASK:
		params[0] = GLImpl.state.color_mask;

	default:		
		params[0] = 0;
		return;
	}
		
}

/***********************************************************************
 * Viewport
 ***********************************************************************/
void glDepthRange (GLclampd zNear, GLclampd zFar)
{
	if (zNear > zFar) {
		GLclampd temp = zNear;
		zNear = zFar;
		zFar = temp;
	}

	GLclampd mid = (zNear+zFar) / 2;

	GLImpl.state.viewport.MaxZ = (float)zFar;
	GLImpl.state.viewport.MinZ = (float)zNear;
	
	/*
	GLImpl.state.viewport.MaxZ = (float)mid;
	GLImpl.state.viewport.MinZ = (float)mid - zFar;
	*/
	GLImpl.state.dirty |= DIRTY_VIEWPORT;
}

void glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
	GLImpl.state.viewport.Width = width;
	GLImpl.state.viewport.Height = height;
	GLImpl.state.viewport.X = x;
	GLImpl.state.viewport.Y = GLImpl.state.render_height - (height + y);
	
	GLImpl.state.dirty |= DIRTY_VIEWPORT;
}

/***********************************************************************
 * States Management
 ***********************************************************************/
void CGLImpl::UpdateStates() {

	// Hack
	device->SetRenderState(D3DRS_HALFPIXELOFFSET, true);
	device->SetRenderState(D3DRS_VIEWPORTENABLE, true);

	if (state.dirty & DIRTY_BLEND) {
		// blending		
		if (GLImpl.state.blend_enable) {
			device->SetRenderState(D3DRS_SRCBLEND, state.blend_src);
			device->SetRenderState(D3DRS_DESTBLEND, state.blend_dst);
			device->SetRenderState(D3DRS_BLENDOP, state.blend_op);

			device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		}
		else {
			device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		}
	}
	if (state.dirty & DIRTY_ALPHA) {
		// Alpha test		
		device->SetRenderState(D3DRS_ALPHATESTENABLE, state.alpha_test_enabled);		
		device->SetRenderState(D3DRS_ALPHAFUNC, state.alpha_test_func);
		device->SetRenderState(D3DRS_ALPHAREF, state.alpha_test_ref & 0xFF);
	}
	if (state.dirty & DIRTY_DEPTH) {
		// Depth
		device->SetRenderState(D3DRS_ZENABLE, state.z_enable);
		device->SetRenderState(D3DRS_ZWRITEENABLE, state.z_mask);		
		device->SetRenderState(D3DRS_ZFUNC, state.z_func);
	}
	if (state.dirty & DIRTY_CULL) {
		// Culling
		// device->SetRenderState(D3DRS_CULLMODE, state.cull_mode);
		unsigned int culling;
#if 0
		if (state.cull_enabled) {
			if (state.cull_face == GL_BACK) {
				if (state.cull_order == GL_CCW) {
					culling = GPUCULL_BACK_FRONTFACE_CCW;
				} else {
					culling = GPUCULL_BACK_FRONTFACE_CW;
				}
			} else if (state.cull_face == GL_FRONT) {
				if (state.cull_order == GL_CCW) {
					culling = GPUCULL_FRONT_FRONTFACE_CCW;
				} else {
					culling = GPUCULL_FRONT_FRONTFACE_CW;
				}
			} else {
				if (state.cull_order == GL_CCW) {
					culling = GPUCULL_NONE_FRONTFACE_CCW;
				} else {
					culling = GPUCULL_NONE_FRONTFACE_CW;
				}
			}
		} else {
			// GL_BACK + GL_CCW
			culling = GPUCULL_NONE_FRONTFACE_CCW; // working
			//culling = GPUCULL_BACK_FRONTFACE_CCW;
		}
#else
		culling = D3DCULL_CCW;
		switch (state.cull_face)
		{
		  case GL_BACK:
			culling = (state.cull_order == GL_CCW ? D3DCULL_CW : D3DCULL_CCW);
			break;
		  case GL_FRONT:
			culling = (state.cull_order == GL_CCW ? D3DCULL_CCW : D3DCULL_CW);
			break;
		  case GL_FRONT_AND_BACK:
			culling = D3DCULL_NONE; // culling will be handled during draw
			break;
		}
#endif
		//culling = state.cull_enabled?D3DCULL_CCW:D3DCULL_NONE;
		device->SetRenderState(D3DRS_CULLMODE, culling);
		//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	if (state.dirty & DIRTY_SCISSOR) {
		// Scissor
		device->SetRenderState(D3DRS_SCISSORTESTENABLE, state.scissor_enabled);
		if (state.scissor_enabled) {
			// lower left
			RECT scissor;
			scissor.left = state.scissor_x;
			scissor.right = state.scissor_x + state.scissor_w;
			scissor.bottom = state.scissor_h + state.scissor_y;
			scissor.top = state.render_height - (state.scissor_h + state.scissor_y);
#ifndef _DEBUG
			//device->SetScissorRect(&scissor);
#endif
		}
	}

	if (state.dirty & DIRTY_ZBIAS) {
		if (state.polygon_offset_enabled) {
			device->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*)&state.zoffset) );
			device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&state.woffset) );
		} else {
			device->SetRenderState(D3DRS_DEPTHBIAS, 0 );
			device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0 );
		}
	}

	if (state.dirty & DIRTY_STENCIL) {
		if (state.stencil_enabled) {
			device->SetRenderState(D3DRS_STENCILENABLE, 1);
			device->SetRenderState(D3DRS_STENCILFUNC, state.stencil_func);
			device->SetRenderState(D3DRS_STENCILREF, state.stencil_ref);
			device->SetRenderState(D3DRS_STENCILMASK, state.stencil_mask_face);
			device->SetRenderState(D3DRS_STENCILWRITEMASK, state.stencil_write_mask);
			device->SetRenderState(D3DRS_STENCILFAIL, state.stencil_op_fail);
			device->SetRenderState(D3DRS_STENCILZFAIL, state.stencil_op_zfail);
			device->SetRenderState(D3DRS_STENCILPASS, state.stencil_op);
		} else {
			device->SetRenderState(D3DRS_STENCILENABLE, 0);
		}
	}
#if 0
		// Stencil
		Xe_SetStencilEnable(xe, xe_state.stencil_enabled);
		Xe_SetStencilFunc(xe, xe_state.stencil_func_face, xe_state.stencil_func);

		/* -1 to leave old value */
		Xe_SetStencilOp(xe, xe_state.stencil_op_face, xe_state.stencil_op_fail, xe_state.stencil_op_zfail, xe_state.stencil_op);

		Xe_SetStencilRef(xe, xe_state.stencil_ref_face, xe_state.stencil_ref);
		Xe_SetStencilMask(xe, xe_state.stencil_mask_face, xe_state.stencil_mask);
		Xe_SetStencilWriteMask(xe, xe_state.stencil_write_face, xe_state.stencil_write_mask);

		D3DDevice_SetRenderState(device, 
#endif

	if (state.dirty & DIRTY_CLIP) {
		//Xe_SetClipPlaneEnables(xe, xe_state.clipplane_enabled);
		BOOL enabled = state.clipplane_enabled;
		if (state.clipplane_enabled) {
			//Xe_SetClipPlane(xe, i, xe_state.clipplane[i]);
			//device->SetClipPlane(0, state.clipplane[0]);
			//device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
			
			//device->SetPixelShaderConstantB(0, &enabled, 1);
			//device->SetVertexShaderConstantF(PARAM_CLIPPLANE_0, (float*)state.clipplane[0], 1);
		} else {
			//device->SetRenderState(D3DRS_CLIPPLANEENABLE, false);
			//device->SetPixelShaderConstantB(0, &enabled, 1);
		}
	}
	if (state.dirty & DIRTY_VIEWPORT) {
		// viewport
		device->SetViewport(&state.viewport);
	}

	// other		
	//Xe_SetFillMode(xe, xe_state.fill_mode_front, xe_state.fill_mode_back);
	//Xe_SetFillMode(xe, XE_FILL_SOLID, XE_FILL_SOLID);

	/*
	if (state.cull_enabled == GL_FALSE)
	{
		// disable culling
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	} else if (state.cull_face == GL_CCW) {
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	} else {
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
	*/

	state.dirty = 0;
}

void CGLImpl::ResetStates()
{
	//memset(&state, 0, sizeof(xe_states_t));
	state.fill_mode_back = state.fill_mode_front = D3DFILL_SOLID;	

	/*
	state.cull_face = GL_BACK;
	state.cull_order = GL_CCW;

	state.viewport.Height = state.render_height;
	state.viewport.Width = state.render_width;
	state.viewport.X = 0;
	state.viewport.Y = 0;
	state.viewport.MinZ = 0;
	state.viewport.MaxZ = 1;
	*/
	/*
	state.blend_src = D3DBLEND_ONE;
	state.blend_op = D3DBLENDOP_ADD;
	state.blend_dst = D3DBLEND_ZERO;
	
	state.alpha_test_enabled = 0;
	state.alpha_test_func = 0;
	state.alpha_test_ref = 0;
	
	state.stencil_enabled = 0;
	state.stencil_func_face = 0;	
	state.stencil_func = 0;		
	state.stencil_op_face = 0;	
	state.stencil_op_fail = 0;	
	state.stencil_op_zfail = 0;	
	state.stencil_op = 0;		
	state.stencil_ref_face = 0;	
	state.stencil_ref = 0;		
	state.stencil_mask_face = 0;	
	state.stencil_mask = 0;		
	state.stencil_write_face = 0;
	state.stencil_write_mask = 0;
	
	state.z_enable = 0;
	state.z_mask = 0;
	state.z_func = 0;
	*/
	state.dirty = 0xFFFFFFFF;
}

void CGLImpl::InitStates() {
	memset(&state, 0, sizeof(xe_states_t));

	

	state.cull_face = GL_BACK;
	state.cull_order = GL_CCW;

	ResetStates();
}

static const char * GL_Description = "OpenGL Xenon By Ced2911";
static const char * GL_Driver = "OpenGL Xenon";
static char *GL_VersionString = "1.1";
static char *GL_ExtensionString = "";

const GLubyte *glGetString (GLenum name)
{
	switch (name)
	{
	case GL_VENDOR:
		return (GLubyte*)GL_Description;
	case GL_RENDERER:
		return (GLubyte*)GL_Driver;
	case GL_VERSION:
		return (GLubyte*)GL_VersionString;
	case GL_EXTENSIONS:
	default:
		return (GLubyte*)GL_ExtensionString;
	}
}