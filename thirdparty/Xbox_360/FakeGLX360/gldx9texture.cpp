#include <cstdio>
#include <cstdlib>
#include "gl_main.h"

GLTexture::GLTexture(int width, int height, D3DFORMAT format) {	
	_info.uncompressed_format = _info.format = format;
	_info.compressed = 0;
	_info.tiled = 0;
	_info.base_height = height;
	_info.base_width = width;
	/*
	int miplevel = 0;
	int scaled_width = width;
	int scaled_height = height;
	while (scaled_width > 1 || scaled_height > 1)
	{
		scaled_width >>= 1;
		scaled_height >>= 1;
		if (scaled_width < 1)
			scaled_width = 1;
		if (scaled_height < 1)
			scaled_height = 1;
		miplevel++;
	}
	*/
#if 0
	D3DXCreateTexture(GLImpl.device, width, height, 0, 0, format, 0, &tex);
#else
	D3DXCreateTexture(GLImpl.device, width, height, 1, 0, format, 0, &tex); // no mip map
#endif
	//printf("tex lvl count: %d\n", tex->GetLevelCount());
}
GLTexture::~GLTexture()
{
	if(tex) {
		tex->Release();
	}
}
void GLTexture::GetLevelDesc(int level, D3DSURFACE_DESC *pDesc) {
	tex->GetLevelDesc(level, pDesc);
}

void GLTexture::setTexture(int sampler)
{
	GLImpl.device->SetTexture(sampler, tex);
}

void GLTexture::lockTexture(int lvl) {
	// get texture information
	tex->GetLevelDesc(lvl, &_info.desc);		
	tex->LockRect(lvl, &_info.rect, NULL, NULL);

	_info.XgFlags = 0;

	if (XGIsBorderTexture((D3DBaseTexture*)this)) {
		_info.XgFlags |= XGTILE_BORDER;
	}
	if (!XGIsPackedTexture((D3DBaseTexture*)this)) {
		_info.XgFlags |= XGTILE_NONPACKED;
	}

	// untile the surface
	if (_info.tiled) {
		XGUntileTextureLevel(_info.base_width, _info.base_height, lvl, XGGetGpuFormat(_info.desc.Format), _info.XgFlags, _info.rect.pBits, _info.rect.Pitch, NULL, _info.rect.pBits, NULL);
	}
}

void GLTexture::unlockTexture(int lvl)
{
	// until
	_info.tiled = 1;
	XGTileTextureLevel(_info.base_width, _info.base_height, lvl, XGGetGpuFormat(_info.desc.Format), _info.XgFlags, _info.rect.pBits, NULL, _info.rect.pBits, _info.rect.Pitch, NULL);
	
	// unlock
	tex->UnlockRect(lvl);
}

BYTE * GLTexture::getData()
{
	//return (BYTE*)_info.data;
	return (BYTE*)_info.rect.pBits;
}

int GLTexture::getPitch()
{
	return _info.rect.Pitch;
}