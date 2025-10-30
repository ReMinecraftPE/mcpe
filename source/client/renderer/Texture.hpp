/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>
#include "compat/LegacyCPP.hpp"

struct Texture
{
	int m_width;
	int m_height;
	uint32_t* m_pixels;
	bool	  m_hasAlpha;
	uint8_t   field_D;

	Texture()
	{
		m_width = 0;
		m_height = 0;
		m_pixels = nullptr;
		m_hasAlpha = false;
		field_D = false;
	}
	Texture(int width, int height, void* pixels, bool hasAlpha, uint8_t a2) : m_width(width), m_height(height), m_pixels((uint32_t*)pixels), m_hasAlpha(hasAlpha), field_D(a2) {}
};
