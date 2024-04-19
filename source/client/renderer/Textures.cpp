/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Textures.hpp"
#include "common/Utils.hpp"

bool Textures::MIPMAP = false;

int Textures::loadTexture(const std::string& name, bool bIsRequired)
{
	std::map<std::string, GLuint>::iterator i = m_textures.find(name);
	if (i != m_textures.end())
		return i->second;

	Texture t = m_pPlatform->loadTexture(name, bIsRequired);

	if (!t.m_pixels && bIsRequired) {
		t.field_C = 1;
		t.field_D = 0;
		t.m_width = 2;
		t.m_height = 2;
		t.m_pixels = new uint32_t[4];
		t.m_pixels[0] = 0xfff800f8;
		t.m_pixels[1] = 0xff000000;
		t.m_pixels[3] = 0xfff800f8;
		t.m_pixels[2] = 0xff000000;
	}

	if (t.m_pixels) {
		return assignTexture(name, t);
	} else {
		return -1;
	}
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

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.m_width, texture.m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.m_pixels);

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
				// texture is already bound so this is fine:
				glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					16 * (x + pDynaTex->m_textureIndex % 16),
					16 * (y + pDynaTex->m_textureIndex / 16),
					16, 16,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					pDynaTex->m_pixels
				);
			}
		}
	}
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
