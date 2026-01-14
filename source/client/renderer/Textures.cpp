/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Textures.hpp"
#include "common/Util.hpp"
#include "common/Utils.hpp"
#include "renderer/RenderContextImmediate.hpp"

#define MIP_TAG "_mip"
#define MIP_TAG_SIZE 4

bool Textures::MIPMAP = false;

TextureData* Textures::loadTexture(const std::string& name, bool bIsRequired)
{
	assert(m_textures.find(name) == m_textures.end());

	TextureData t = m_pPlatform->loadTexture(name, bIsRequired);

	if (t.isEmpty())
	{
		if (bIsRequired)
		{
			t.m_imageData.m_colorSpace = COLOR_SPACE_RGBA;
			t.m_imageData.m_width = 2;
			t.m_imageData.m_height = 2;
			uint32_t* placeholder = new uint32_t[4];
			placeholder[0] = 0xfff800f8;
			placeholder[1] = 0xff000000;
			placeholder[3] = 0xfff800f8;
			placeholder[2] = 0xff000000;
			t.m_imageData.m_data = (uint8_t*)placeholder;
		}
		else
		{
			// Record the fact that the texture file couldn't be loaded
			// This means we can stop checking the filesystem every frame to see if the texture can be found
			m_textures[name] = nullptr;
			return nullptr;
		}
	}

	t.m_bEnableFiltering = m_bBlur;
	t.m_bWrap = !m_bClamp;

	return uploadTexture(name, t);
}

size_t _mipTagStart(const std::string& path)
{
	// "_mip" + "0."
	constexpr size_t mipSuffixLength = MIP_TAG_SIZE + 2;

	std::string extension = Util::getExtension(path);

	return path.length() - mipSuffixLength - extension.length();
}

bool _isMipmap(const std::string& path)
{
	std::string mipTag = path.substr(_mipTagStart(path), MIP_TAG_SIZE);
	return mipTag == MIP_TAG;
}

TextureData* Textures::uploadTexture(const std::string& name, TextureData& t)
{
	TextureData* result = nullptr;

	bool isMipmap = _isMipmap(name);
	if (isMipmap && name.find(MIP_TAG))
	{
		if (mce::Texture::supportsMipMaps())
		{
			// Find the starting position of the mipmap tag (e.g., "_mip") in the filename.
			size_t mipTagPos = _mipTagStart(name);

			// Reconstruct the base texture name from the mipmapped filename.
			// e.g., "images/terrain-atlas_mip0.tga" -> "images/terrain-atlas.tga"
			std::string basePathNoExtension = name.substr(0, mipTagPos);
			std::string extension = Util::getExtension(name); // "tga"
			std::string basePath = basePathNoExtension + "." + extension;

			TextureMap::iterator it = m_textures.find(basePath);
			if (it != m_textures.end())
			{
				char mipLevelChar = name[mipTagPos + MIP_TAG_SIZE];
				t.m_imageData.m_mipCount = mipLevelChar - '0';

				TextureData* pBaseTexture = it->second;

				if (pBaseTexture)
				{
					pBaseTexture->loadMipmap(t.m_imageData);
				}

				result = pBaseTexture;
			}
		}
		return result;
	}

	t.load();
	result = new TextureData(t);
	m_textures[name] = result;

	return result;
}

void Textures::unloadAll()
{
	for (TextureMap::iterator it = m_textures.begin(); it != m_textures.end(); it++)
	{
		TextureData* pTexture = it->second;
		if (pTexture)
			pTexture->unload();
	}

	TextureData::unbindAll();
}

void Textures::clear()
{
	unloadAll();
	// note: Textures::clear() does not touch the dynamic textures vector

	for (TextureMap::iterator it = m_textures.begin(); it != m_textures.end(); it++)
		delete it->second;

	m_textures.clear();
	m_currBoundTex = -1;
}

Textures::Textures(Options* pOptions, AppPlatform* pAppPlatform)
{
	m_bClamp = false;
	m_bBlur = false;

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
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	// tick dynamic textures here
	for (std::vector<DynamicTexture*>::iterator it = m_dynamicTextures.begin(); it < m_dynamicTextures.end(); it++)
	{
		DynamicTexture* pDynaTex = *it;

		TextureData* pData = pDynaTex->bindTexture(this);
		if (!pData) continue;
		pDynaTex->tick();

		mce::Texture& texture = pData->m_texture;

		texture.enableWriteMode(renderContext);

		for (int x = 0; x < pDynaTex->m_textureSize; x++)
		{
			for (int y = 0; y < pDynaTex->m_textureSize; y++)
			{
				texture.subBuffer(renderContext,
					pDynaTex->m_pixels,
					16 * (x + pDynaTex->m_textureIndex % 16),
					16 * (y + pDynaTex->m_textureIndex / 16),
					16, 16, 0);
			}
		}

		texture.disableWriteMode(renderContext);
	}
}

TextureData* Textures::_loadAndBindTexture(const std::string& name, bool isRequired, unsigned int textureUnit)
{
	TextureData* pTexture = getTextureData(name, isRequired);

	if (!pTexture)
		return nullptr;

	// bound twice on initial load in _loadTexData
	// if it was just loaded, this is our third call to glBindTexture
	pTexture->bind(textureUnit);

	return pTexture;
}

TextureData* Textures::loadAndBindTexture(const std::string& name, bool isRequired, unsigned int textureUnit)
{
	std::vector<std::string> resourcepacks = m_pOptions->m_resourcepacks;
	std::string fullpath, slashname = "/" + name;
	TextureData* ret = nullptr;

	for (size_t i = 0; i < resourcepacks.size(); ++i)
	{
		fullpath = "/resource_packs/" + resourcepacks[i] + slashname;
		ret = _loadAndBindTexture(fullpath, false, textureUnit);
		if (ret)
			break;
	}
	if (!ret)
		ret = _loadAndBindTexture(name, isRequired, textureUnit);

	return ret;
}

TextureData* Textures::getTextureData(const std::string& name, bool isRequired)
{
	TextureMap::iterator it = m_textures.find(name);
	TextureData* pTexture;
	if (it != m_textures.end())
		pTexture = it->second;
	else
		pTexture = loadTexture(name, isRequired);
	return pTexture;
}

void Textures::addDynamicTexture(DynamicTexture* pTexture)
{
	m_dynamicTextures.push_back(pTexture);
	pTexture->tick();
}
