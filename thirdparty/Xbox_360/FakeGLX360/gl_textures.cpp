#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "gl_main.h"
#include <xgraphics.h>

#define XE_MAX_TEXTURE 50000
#define TEXTURE_SLOT_EMPTY -1

glXeSurface_t * glXeSurfaces = NULL;

void CGLImpl::ApplyTextures()
{
	for(int i=0; i<XE_MAX_TMUS; i++) {    
		// set texture
		if (tmus[i].enabled && tmus[i].boundtexture) {
			//if (tmus[i].boundtexture->teximg->IsSet(device) == FALSE) {
				glXeSurface_t * tex = tmus[i].boundtexture;

				tex->teximg->setTexture(i);				
				// set sampler states
				device->SetSamplerAddressStates(i, tex->wrap_u, tex->wrap_v, D3DTADDRESS_WRAP);
				// device->SetSamplerFilterStates(i, tex->min_filter, tex->mag_filter, tex->mip_filter, tex->max_anisotropy);
				device->SetSamplerFilterStates(i, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, 16); // force anisotropic, no need for mipmap
			//}
		}
		else {
			device->SetTexture(i, NULL);
		}
	}
}

/***********************************************************************
 * Create/Gen/Delete images
 ***********************************************************************/
void CGLImpl::InitTextures()
{
	glXeSurfaces = (glXeSurface_t *)new glXeSurface_t[XE_MAX_TEXTURE]();
}

void CGLImpl::InitTexture(glXeSurface_t *tex)
{
	// slot is empty
	tex->glnum = TEXTURE_SLOT_EMPTY;

	// destroy texture
	if (tex->teximg) {
		delete tex->teximg;
		tex->teximg = NULL;
	}
}

glXeSurface_t *CGLImpl::AllocTexture(void)
{
	int i = 0;
	glXeSurface_t *tex;

	// find a free texture
	for (i = 0; i< XE_MAX_TEXTURE; i++)
	{
		tex = &glXeSurfaces[i];
		// no texture
		if (!tex->teximg)
		{
			InitTexture(tex);
			return tex;
		}
		// free slot
		if (tex->glnum == TEXTURE_SLOT_EMPTY)
		{
			InitTexture(tex);
			return tex;
		}
	}

	xe_gl_error("Xe_AllocTexture: out of textures!!!\n");
	return NULL;
}


void CGLImpl::ReleaseTextures (void)
{
	glXeSurface_t *tex;
	int i;

	// explicitly NULL all textures and force texparams to dirty
	for (i = 0; i< XE_MAX_TEXTURE; i++)
	{
		tex = &glXeSurfaces[i];
		InitTexture(tex);
	}
} 
 
void glDeleteTextures(GLsizei n, const GLuint *textures)
{
	int i;
	glXeSurface_t *tex;
	return;
	
	for (i = 0; i < n; i++) {
		for (i = 0; i< XE_MAX_TEXTURE; i++)
		{
			tex = &glXeSurfaces[i];
			if (tex->glnum == textures[i]) {
				GLImpl.InitTexture(tex);
				break;
			}
		}
	}
}

static GLuint texture_count = 1;

void glGenTextures(GLsizei n, GLuint *textures)
{
	int i;	
	
	for(i = 0; i < n; i++)
	{
		glXeSurface_t *tex = GLImpl.AllocTexture();
		tex->glnum = textures[i] = texture_count++;
	}
}

void glBindTexture(GLenum target, GLuint texture)
{
	int i;
	
	glXeSurface_t *tex;
	if (target != GL_TEXTURE_2D) 
		return;

	GLImpl.tmus[GLImpl.current_tmu].boundtexture = NULL;
	
	// find a texture
	for (i = 0; i< XE_MAX_TEXTURE; i++)
	{
		tex = &glXeSurfaces[i];
		
		if (tex && tex->glnum == texture)
		{
			GLImpl.tmus[GLImpl.current_tmu].boundtexture = tex;
			break;
		}
	}
	
	// did we find it?
	if (!GLImpl.tmus[GLImpl.current_tmu].boundtexture)
	{
		// nope, so fill in a new one (this will make it work with texture_extension_number)
		// (i don't know if the spec formally allows this but id seem to have gotten away with it...)
		GLImpl.tmus[GLImpl.current_tmu].boundtexture = GLImpl.AllocTexture();

		// reserve this slot
		GLImpl.tmus[GLImpl.current_tmu].boundtexture->glnum = texture;

		// ensure that it won't be reused
		if (texture > texture_count)
			texture_count = texture;
	}
	
	// this should never happen
	if (!GLImpl.tmus[GLImpl.current_tmu].boundtexture) 
		xe_gl_error("glBindTexture: out of textures!!!\n");
		
	// dirty the params
	GLImpl.tmus[GLImpl.current_tmu].texparamdirty = TRUE;
}

/***********************************************************************
 * Images manipulation
 ***********************************************************************/

void check_format(int srcbytes, int dstbytes) {
	int d = 0;
	
	if (srcbytes == 4) {
		if (dstbytes == 4) {
			d++;
		} else if(dstbytes == 1) {
			d++;
		}
	} else if (srcbytes == 3) {
		if (dstbytes == 4) {
			d++;
		} else if (dstbytes == 1) {
			d++;
		}
	} else if (srcbytes == 1) {
		if (dstbytes == 1) {
			d++;
		} else if (dstbytes == 4) {
			d++;
		}
	}
				
	if (d == 0) {
		printf("Error check that src = %d dst = %d\n", srcbytes, dstbytes);
	}
}

static inline int src_format_to_bypp(GLenum format)
{
	int ret = 0;
	if (format == 1 || format == GL_LUMINANCE)
		ret = 1;
	else if (format == 3 || format == GL_RGB || format == GL_RGB8)
		ret = 3;
	else if (format == 4 || format == GL_RGBA)
		ret = 4;
	else 
		xe_gl_error ("D3D_FillTextureLevel: illegal format");
	if (ret != 4)
		printf("src_format_to_bypp %d\n", ret);
	return ret;
}

static inline int dst_format_to_bypp(GLenum format)
{
	int ret = 0;
	if (format == 1 || format == GL_LUMINANCE)
		ret = 1;
	else if (format == 3 || format == GL_RGB || format == GL_RGB8)
		ret = 4;
	else if (format == 4 || format == GL_RGBA)
		ret = 4;
	else
		xe_gl_error ("D3D_FillTextureLevel: illegal format");
		
	if (ret != 4)
		printf("dst_format_to_bypp %d\n", ret);
	return ret;
}

static void copyImage(int xoffset, int yoffset, int width, int height, BYTE * srcdata, int srcbytes, BYTE * surfbuf, int dstbytes) 
{
	BYTE * dstdata = NULL;
	int pitch = (width * dstbytes);
	int offset = 0;

	for (int y = yoffset; y < (yoffset + height); y++) {
		offset = (y * pitch)+(xoffset * dstbytes);
		dstdata = surfbuf + offset;
			
		for (int x = xoffset; x < (xoffset + width); x++) {
			if (srcbytes == 4) {
				if (dstbytes == 4) {
					dstdata[0] = srcdata[3];
					dstdata[3] = srcdata[2];
					dstdata[2] = srcdata[1];
					dstdata[1] = srcdata[0];
				} else if(dstbytes == 1) {
					dstdata[0] = ((int) srcdata[0] + (int) srcdata[1] + (int) srcdata[2] + (int) srcdata[3]) / 3;
				}
						
				srcdata += srcbytes;
				dstdata += dstbytes;
					
			} else if (srcbytes == 3) {
				if (dstbytes == 4) {					
					dstdata[0] = 0xff;
					dstdata[3] = srcdata[2];
					dstdata[2] = srcdata[1];
					dstdata[1] = srcdata[0];
				} else if (dstbytes == 1) {
					dstdata[0] = ((int) srcdata[0] + (int) srcdata[1] + (int) srcdata[2]) / 3;
				}
						
				srcdata += srcbytes;
				dstdata += dstbytes;
			} else if (srcbytes == 1) {
				if (dstbytes == 1) {
					dstdata[0] = srcdata[0];
				} else if (dstbytes == 4) {
					dstdata[0] = srcdata[0];
					dstdata[1] = srcdata[0];
					dstdata[2] = srcdata[0];
					dstdata[3] = srcdata[0];
				}
				srcdata += srcbytes;
				dstdata += dstbytes;
			}
		}
	}
}

void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
	bool is_set = false;
	
#ifdef _DEBUG
	return;
#endif

#if 1 // no mipmap
	if (level > 0)
		return;
#endif

	if (!GLImpl.tmus[GLImpl.current_tmu].boundtexture) {
		printf("Not texture binded\n");
		return;
	}
		
	GLImpl.device->SetTexture(0, NULL);
	GLTexture * surf = NULL;
	
	if (GLImpl.tmus[GLImpl.current_tmu].boundtexture && GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg ) {
		surf = GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg;
	}
	
	if (surf) {
		int srcbytes = src_format_to_bypp(format);
		int dstbytes = dst_format_to_bypp(GLImpl.tmus[GLImpl.current_tmu].boundtexture->internalformat);
		BYTE * surfbuf;
		BYTE * srcdata = (BYTE*) pixels;
		BYTE * dstdata;

		surf->lockTexture(level);

		srcdata = (BYTE*) pixels;
		surfbuf = (BYTE*)surf->getData();		
		dstdata = (BYTE*)surf->getData();
		
		check_format(srcbytes, dstbytes);

		copyImage(xoffset, yoffset, width, height, srcdata, srcbytes, surfbuf, dstbytes);

		surf->unlockTexture(level);

		GLImpl.tmus[GLImpl.current_tmu].boundtexture->dirty = 1;
	}
}

void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum pixelformat, GLenum type, const GLvoid *pixels)
{
	D3DFORMAT format;
	int need_texture = 1;
	GLTexture * surf= NULL;
	
	if (type != GL_UNSIGNED_BYTE) {		
		xe_gl_error("glTexImage2D: Unrecognised pixel format\n");
		
		return;
	}
	if (target != GL_TEXTURE_2D)
		return;
		
	if (!GLImpl.tmus[GLImpl.current_tmu].boundtexture) {
		printf("No texture bound\n");
		return;
	}
#if 1 // no mipmap
	if (level > 0)
		return;
#endif
	GLImpl.device->SetTexture(0, NULL);

	int srcbytes = src_format_to_bypp(pixelformat);
	int dstbytes = 4;
	
	// validate format
	switch (internalformat)
	{
	case 1:
	case GL_LUMINANCE:
		dstbytes = 1;
		//format = D3DFMT_LIN_L8;
		format = D3DFMT_L8;
		break;
	case 3:
	case GL_RGB8:
	case GL_RGB:
		dstbytes = 4;
		//format = D3DFMT_LIN_A8R8G8B8;
		format = D3DFMT_A8R8G8B8;
		break;
	case 4:
	case GL_RGBA:
		dstbytes = 4;
		//format = D3DFMT_LIN_A8R8G8B8;
		format = D3DFMT_A8R8G8B8;
		break;
	default:
		printf("%X internalformat\n" , internalformat);
		xe_gl_error ("invalid texture internal format\n");
		return;
	}
	
	

	if (level > 0) {
		need_texture = 0;
	}

	if (GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg && level == 0) {
		D3DSURFACE_DESC desc;
		surf = GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg;
		surf->GetLevelDesc(level, &desc);
		if ((desc.Width != width) || (desc.Height != height) || (desc.Format != format)) {
			need_texture = 1;
			// destroy texture
			delete GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg;
		}
		else {
			need_texture = 0;
		}
	}
	
	if (need_texture) {	
		surf = new GLTexture(width, height, format);
		GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg = surf;
	} else {
		surf = GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg;
	}

	// lock texture
	surf->lockTexture(level);
	
	memset(surf->getData(), 0x00, surf->getPitch() * height);
	GLImpl.tmus[GLImpl.current_tmu].boundtexture->internalformat = internalformat;
		
	BYTE * surfbuf = (BYTE*) surf->getData();
	BYTE * srcdata = (BYTE*) pixels;
	BYTE * dstdata = surfbuf;
	
	check_format(srcbytes, dstbytes);

	copyImage(0, 0, width, height, srcdata, srcbytes, surfbuf, dstbytes);

	surf->unlockTexture(level);

	GLImpl.tmus[GLImpl.current_tmu].boundtexture->dirty = 1;
}

/** Not used in QII */
void glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{
	
}
/***********************************************************************
 * 
 * Multi texturing
 * 
 ***********************************************************************/

int xeGetTmu(GLenum texture)
{
	switch(texture) {
		case 0:
		case GL_TEXTURE0_SGIS:
		case GL_TEXTURE0:
			return 0;
		case 1:
		case GL_TEXTURE1_SGIS:
		case GL_TEXTURE1:
			return 1;
			/*			
		case GL_TEXTURE2:
			return 2;
		case GL_TEXTURE3:
			return 3;
			*/
		default:
			printf("xeGetTmu %x\n", texture);
			return 0;
	}
}

void glActiveTexture(GLenum texture)
{
	GLImpl.current_tmu = xeGetTmu(texture);
}

void glMultiTexCoord1f(GLenum target, GLfloat s)
{
	int tmu = xeGetTmu(target);
	GLImpl.prim.uv[tmu].u = s;
	GLImpl.prim.uv[tmu].v = 0;
}

void glMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t)
{
	int tmu = xeGetTmu(target);
	GLImpl.prim.uv[tmu].u = s;
	GLImpl.prim.uv[tmu].v = t;
}
/***********************************************************************
 * 
 * Images parameters
 * 
 ***********************************************************************/
void glTexParameterfv(	GLenum  	target,
 	GLenum  	pname,
 	const GLfloat *  	params){

}

static unsigned int glWrap(int param) {
	switch (param) {
		case GL_CLAMP_TO_EDGE:
			return D3DTADDRESS_CLAMP;
		case GL_CLAMP_TO_BORDER:
			return D3DTADDRESS_BORDER;
		case GL_MIRRORED_REPEAT:
			return D3DTADDRESS_MIRROR;
		case GL_REPEAT:
			return D3DTADDRESS_WRAP;
		default:
			return D3DTADDRESS_WRAP;
	}
}

static unsigned int getFilteringParam(int param) {
	unsigned int ret = D3DTEXF_POINT;

	switch(param) {
	case GL_LINEAR:
	case GL_LINEAR_MIPMAP_NEAREST:
	case GL_LINEAR_MIPMAP_LINEAR:
		ret = D3DTEXF_LINEAR;
		break;
	case GL_NEAREST:
	case GL_NEAREST_MIPMAP_NEAREST:
	case GL_NEAREST_MIPMAP_LINEAR:
	default:
		ret = D3DTEXF_POINT;
		break;
	}
	return ret;
}

void glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
	if (!GLImpl.tmus[GLImpl.current_tmu].boundtexture) return;
	if (!GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg ) return;
	if (target != GL_TEXTURE_2D) return;

	GLImpl.tmus[GLImpl.current_tmu].texparamdirty = TRUE;

	switch (pname)
	{
#if 0
	case GL_TEXTURE_MIN_FILTER:
	case GL_TEXTURE_MAG_FILTER:
		if(pname ==  GL_TEXTURE_MIN_FILTER) {
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->min_filter = getFilteringParam((int) param);
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->min_filter = getFilteringParam((int) param);
		} else {
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->mag_filter = getFilteringParam((int) param);
		}
		break;
#else
		if ((int) param == GL_NEAREST_MIPMAP_NEAREST)
		{
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->min_filter = D3DTEXF_POINT;
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->mip_filter = D3DTEXF_POINT;
		}
		else if ((int) param == GL_LINEAR_MIPMAP_NEAREST)
		{
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->min_filter = D3DTEXF_LINEAR;
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->mip_filter = D3DTEXF_POINT;
		}
		else if ((int) param == GL_NEAREST_MIPMAP_LINEAR)
		{
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->min_filter = D3DTEXF_POINT;
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->mip_filter = D3DTEXF_LINEAR;
		}
		else if ((int) param == GL_LINEAR_MIPMAP_LINEAR)
		{
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->min_filter = D3DTEXF_LINEAR;
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->mip_filter = D3DTEXF_LINEAR;
		}
		else if ((int) param == GL_LINEAR)
		{
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->min_filter = D3DTEXF_LINEAR;
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->mip_filter = D3DTEXF_NONE;
		}
		else
		{
			// GL_NEAREST
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->min_filter = D3DTEXF_POINT;
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->mip_filter = D3DTEXF_NONE;
		}
		break;

	case GL_TEXTURE_MAG_FILTER:
		if ((int) param == GL_LINEAR)
			GLImpl.tmus[GLImpl.current_tmu].boundtexture->mag_filter = D3DTEXF_LINEAR;
		else GLImpl.tmus[GLImpl.current_tmu].boundtexture->mag_filter = D3DTEXF_POINT;
		break;
#endif
	case GL_TEXTURE_WRAP_S:
		GLImpl.tmus[GLImpl.current_tmu].boundtexture->wrap_u = glWrap((int) param);
		break;

	case GL_TEXTURE_WRAP_T:
		GLImpl.tmus[GLImpl.current_tmu].boundtexture->wrap_v = glWrap((int) param);
		break;

	default:
		break;
	}
}

 
void glTexParameteri (GLenum target, GLenum pname, GLint param)
{
	glTexParameterf (target, pname, (GLfloat)param);
}

void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{

}