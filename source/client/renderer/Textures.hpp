/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include <map>

#include "thirdparty/GL/GL.hpp"
#include "client/options/Options.hpp"
#include "client/app/AppPlatform.hpp"
#include "DynamicTexture.hpp"

#define C_TERRAIN_NAME "terrain.png"
#define C_ITEMS_NAME   "gui/items.png"
#define C_BLOCKS_NAME  "gui/gui_blocks.png"

class DynamicTexture; // in case we are being included from DynamicTexture. We don't store complete references to that

struct TextureDataGL
{
	int glID;
	Texture textureData;

	TextureDataGL()
	{
		glID = 0;
	}
	TextureDataGL(int i, Texture& x)
	{
		glID = i;
		textureData = x;
	}
};

class Textures
{
protected:
	typedef std::map<std::string, TextureData*> TextureMap;

public:
	TextureData* loadTexture(const std::string& name, bool bRequired);
	TextureData* loadAndBindTexture(const std::string& name, bool isRequired = true, unsigned int textureUnit = 0);
	TextureData* getTextureData(const std::string& name, bool isRequired);
	void unloadAll();
	void clear();
	void tick();
	void addDynamicTexture(DynamicTexture* pTexture);

	// set smoothing for next texture to be loaded
	void setSmoothing(bool b)
	{
		m_bBlur = b;
	}

	// set smoothing for next texture to be loaded
	void setClampToEdge(bool b)
	{
		m_bClamp = b;
	}

	Textures(Options*, AppPlatform*);
	~Textures();

private:
	static bool MIPMAP;

	TextureData* uploadTexture(const std::string& name, TextureData& t);

protected:
	TextureMap m_textures;
	Options* m_pOptions;
	AppPlatform* m_pPlatform;
	int m_currBoundTex;
	bool m_bClamp;
	bool m_bBlur;
	std::vector<DynamicTexture*> m_dynamicTextures;

	friend class StartMenuScreen;
};

