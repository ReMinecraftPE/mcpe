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

class DynamicTexture; // in case we are being included from DynamicTexture. We don't store complete references to that

struct TextureData
{
	int glID;
	Texture textureData;

	TextureData()
	{
		glID = 0;
	}
	TextureData(int i, Texture& x)
	{
		glID = i;
		textureData = x;
	}
};

class Textures
{
public:
	int loadTexture(const std::string& name, bool bRequired);
	int loadAndBindTexture(const std::string& name);
	void clear();
	void tick();
	void addDynamicTexture(DynamicTexture* pTexture);
	Texture* getTemporaryTextureData(GLuint id);

	// set smoothing for next texture to be loaded
	void setSmoothing(bool b)
	{
		field_39 = b;
	}

	// set smoothing for next texture to be loaded
	void setClampToEdge(bool b)
	{
		field_38 = b;
	}

	Textures(Options*, AppPlatform*);
	~Textures();

protected:
	static bool MIPMAP;
	static int terrainWidth;
	static int terrainHeight;
	static int terrainTileSize;

	int assignTexture(const std::string& name, Texture& t);

	std::map<std::string, GLuint> m_textures;
	Options* m_pOptions;
	AppPlatform* m_pPlatform;
	int m_currBoundTex;
	bool field_38;
	bool field_39;
	std::map<GLuint, TextureData> m_textureData;
	std::vector<DynamicTexture*> m_dynamicTextures;

	friend class StartMenuScreen;
	friend class DynamicTexture;
	friend class PatchManager;
};

