/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include <map>

#include "compat/GL.hpp"
#include "Options.hpp"
#include "AppPlatform.hpp"
#include "DynamicTexture.hpp"

class DynamicTexture; // in case we are being included from DynamicTexture. We don't store complete references to that

struct TextureData
{
	int glID = 0;
	Texture textureData;

	TextureData() {}
	TextureData(int i, Texture& x)
	{
		glID = i;
		textureData = x;
	}
};

class Textures
{
public:
	int loadTexture(const std::string& name, bool b);
	int loadAndBindTexture(const std::string& name);
	void clear();
	void tick();
	void addDynamicTexture(DynamicTexture* pTexture);
	Texture* getTemporaryTextureData(GLuint id);

	Textures(Options*, AppPlatform*);
	~Textures();

private:
	static bool MIPMAP;

	int assignTexture(const std::string& name, Texture& t);

protected:
	std::map<std::string, GLuint> m_textures;
	Options* m_pOptions;
	AppPlatform* m_pPlatform;
	int m_currBoundTex = 0;
	bool field_38 = false;
	bool field_39 = false;
	std::map<GLuint, TextureData> m_textureData;
	std::vector<DynamicTexture*> m_dynamicTextures;

	friend class StartMenuScreen;
};

