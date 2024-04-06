/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Textures.hpp"
#include "common/Utils.hpp"

int Textures::terrainWidth = 256;
int Textures::terrainHeight = 256;
int Textures::terrainTileSize = 16;
bool Textures::MIPMAP = false;

int Textures::loadTexture(const std::string& name, bool bIsRequired)
{
	std::map<std::string, GLuint>::iterator i = m_textures.find(name);
	if (i != m_textures.end())
		return i->second;

	Texture t = m_pPlatform->loadTexture(name, bIsRequired);

	int result = -1;
	if (t.m_pixels)
		result = assignTexture(name, t);

	return result;
}

int Textures::assignTexture(const std::string& name, Texture& texture)
{
	GLuint textureID = 0;

	glGenTextures(1, &textureID);
	if (textureID != m_currBoundTex)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		m_currBoundTex = textureID;
	}

	if (MIPMAP)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (field_39)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (field_38)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	GLuint internalFormat = GL_RGB;

	if (texture.field_C)
		internalFormat = GL_RGBA;

	// If the texture is called "terrain.png", perform some transformations to turn it into a strip.
	uint32_t* pixelData = texture.m_pixels;
	int width = texture.m_width, height = texture.m_height;
	bool deletePixelData = false;

	if (name == "terrain.png" || name == "gui/items.png")
	{
		// 1024x1024 texture.

		// How it works is that we repeat the texture 16 times per column, as that's the max
		// that the game will greedy mesh up to.  So we have 4 columns, each with 64 frames.

		const int tileSize = texture.m_width / 16;
		width  = tileSize * 16 * 4;
		height = tileSize * 64;
		pixelData = new uint32_t[width * height];
		deletePixelData = true;

		for (int i = 0; i < 256; i++)
		{
			int dstX = (i / 64) * tileSize * 16;
			int dstY = (i % 64) * tileSize;
			int srcX = (i & 0xF) * tileSize;
			int srcY = (i >>  4) * tileSize;

			for (int x = 0; x < tileSize; x++)
			{
				for (int y = 0; y < tileSize; y++)
				{
					uint32_t pixel = texture.m_pixels[(srcY + y) * texture.m_width + (srcX + x)];

					for (int z = 0; z < 16; z++)
						pixelData[(dstY + y) * width + (dstX + x) + tileSize * z] = pixel;
				}
			}
		}

		terrainWidth  = width;
		terrainHeight = height;
		terrainTileSize = tileSize;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

	if (deletePixelData)
		delete[] pixelData;

	m_textures[name] = textureID;
	m_textureData[textureID] = TextureData(textureID, texture);
	return textureID;
}

void Textures::clear()
{
	// note: Textures::clear() does not touch the dynamic textures vector

	for (std::map<std::string, GLuint>::iterator it = m_textures.begin(); it != m_textures.end(); it++)
		glDeleteTextures(1, &it->second);

	for (std::map<GLuint, TextureData>::iterator it = m_textureData.begin(); it != m_textureData.end(); it++)
		delete[] it->second.textureData.m_pixels;

	m_textures.clear();
	m_textureData.clear();
	m_currBoundTex = -1;
}

Textures::Textures(Options* pOptions, AppPlatform* pAppPlatform)
{
	field_38 = false;
	field_39 = false;

	m_pPlatform = pAppPlatform;
	m_pOptions = pOptions;
	m_currBoundTex = -1;
}

Textures::~Textures()
{
	clear();

	for (std::vector<DynamicTexture*>::iterator it = m_dynamicTextures.begin(); it != m_dynamicTextures.end(); it++)
	{
		DynamicTexture* pDynaTex = *it;
		SAFE_DELETE(pDynaTex);
	}

	m_dynamicTextures.clear();
}

void Textures::tick()
{
	// tick dynamic textures here
	for (std::vector<DynamicTexture*>::iterator it = m_dynamicTextures.begin(); it < m_dynamicTextures.end(); it++)
	{
		DynamicTexture* pDynaTex = *it;

		pDynaTex->bindTexture(this);
		pDynaTex->tick();

		for (int x = 0; x < pDynaTex->m_textureSize; x++)
		{
			for (int y = 0; y < pDynaTex->m_textureSize; y++)
			{
				int texIndex = y * 16 + x + pDynaTex->m_textureIndex;

				// texture is already bound so this is fine:
				glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					0,
					terrainTileSize * texIndex,
					terrainTileSize, terrainTileSize,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					pDynaTex->m_pixels
				);
			}
		}
	}

	/*if (rand() % 70 == 0) {
		LOG_I("Output!");
		FILE* f = fopen("C:\\growalone_maps\\test.bin", "wb");
		char* buf = new char[16 * 4096 * sizeof(uint32_t)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
		fwrite(buf, 1, 16 * 4096 * sizeof(uint32_t), f);
		fclose(f);
		delete[] buf;
	}*/
}

int Textures::loadAndBindTexture(const std::string& name)
{
	int id = loadTexture(name, true);

	if (m_currBoundTex != id)
	{
		m_currBoundTex = id;
		glBindTexture(GL_TEXTURE_2D, id);
	}

	return id;
}

void Textures::addDynamicTexture(DynamicTexture* pTexture)
{
	m_dynamicTextures.push_back(pTexture);
	pTexture->tick();
}

Texture* Textures::getTemporaryTextureData(GLuint id)
{
	std::map<GLuint, TextureData>::iterator i = m_textureData.find(id);
	if (i == m_textureData.end())
		return nullptr;

	return &i->second.textureData;
}
