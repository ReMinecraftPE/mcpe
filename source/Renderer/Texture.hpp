/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	Texture.hpp

	The following code is licensed under the following license:
	< no license yet :( >
 ********************************************************************/

#pragma once

#include <cstdint>

struct Texture
{
	int m_width = 0;
	int m_height = 0;
	uint32_t* m_pixels = nullptr;
	uint8_t   field_C = false;
	uint8_t   field_D = false;

	Texture() {}
	Texture(int width, int height, void* pixels, uint8_t a1, uint8_t a2) : m_width(width), m_height(height), m_pixels((uint32_t*)pixels), field_C(a1), field_D(a2) {}
};

struct GLTexture
{
	unsigned m_textureID; // GL texture ID
	Texture m_textureData;
};
