/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Font.hpp"
#include "Tesselator.hpp"

constexpr char COLOR_START_CHAR = '\xa7';

Font::Font(Options* pOpts, const std::string& fileName, Textures* pTexs)
{
	m_fileName = fileName;
	m_pOptions = pOpts;
	m_pTextures = pTexs;

	init(pOpts);
}

void Font::init(Options* pOpts)
{
	GLuint texID = m_pTextures->loadTexture(m_fileName, true);
	Texture* pTexture = m_pTextures->getTemporaryTextureData(texID);
	if (!pTexture) return;

	for (int i = 0; i < 256; i++) // character number
	{
		// note: the 'widthMax' behavior is assumed. It might not be like that exactly
		int widthMax = 0;

		if (i == 32) // space
		{
			widthMax = 2;
		}
		else
		{
			for (int j = 7; j >= 0; j--) // x position
			{
				int x = (i % 16), y = (i / 16);
				int pixelDataIndex = pTexture->m_width * 8 * y + 8 * x + j;

				for (int k = 0; k < 8; k++)
				{
					if ((uint8_t)pTexture->m_pixels[pixelDataIndex] != 0)
					{
						if (widthMax < j)
							widthMax = j;
					}

					pixelDataIndex += pTexture->m_width;
				}
			}
		}

		m_charWidthInt[i] = widthMax + 2;
		m_charWidthFloat[i] = float (widthMax) + 2;
	}
}

void Font::buildChar(unsigned char chr, float x, float y)
{
	Tesselator& t = Tesselator::instance;

	float u = float((chr % 16) * 8);
	float v = float((chr / 16) * 8);

	constexpr float D128 = (1.0f / 128.0f);

#define CO (7.99f)

	t.vertexUV(x,      y + CO, 0.0f,  u       * D128, (v + CO) * D128);
	t.vertexUV(x + CO, y + CO, 0.0f, (u + CO) * D128, (v + CO) * D128);
	t.vertexUV(x + CO, y,      0.0f, (u + CO) * D128,  v       * D128);
	t.vertexUV(x,      y,      0.0f,  u       * D128,  v       * D128);

#undef CO
}

void Font::draw(const std::string& str, int x, int y, int color)
{
	draw(str, x, y, color, false);
}

void Font::drawShadow(const std::string& str, int x, int y, int color)
{
	draw(str, x + 1, y + 1, color, true);
	draw(str, x, y, color, false);
}

void Font::draw(const std::string& str, int x, int y, int color, bool bShadow)
{
	drawSlow(str, x, y, color, bShadow);
}

void Font::drawSlow(const std::string& str, int x, int y, int colorI, bool bShadow)
{
	if (str.empty()) return;

	uint32_t color = colorI;

	if (bShadow)
		color = (color & 0xFF000000U) + ((color & 0xFCFCFCu) >> 2);

	m_pTextures->loadAndBindTexture(m_fileName);

	uint32_t red = (color >> 16) & 0xFF;
	uint32_t grn = (color >>  8) & 0xFF;
	uint32_t blu = (color >>  0) & 0xFF;
	uint32_t alp = (color >> 24) & 0xFF;

	float alpf = float(alp) / 255.0f;
	if (alpf == 0.0f)
		alpf = 1.0f;

	glColor4f(float(red) / 255.0f, float(grn) / 255.0f, float(blu) / 255.0f, alpf);
	glPushMatrix();

	Tesselator& t = Tesselator::instance;
	t.begin();

	glTranslatef(float(x), float(y), 0.0f);

	float cXPos = 0.0f, cYPos = 0.0f;

	for (int i = 0; i < int(str.size()); i++)
	{
		if (str[i] == '\n')
		{
			cYPos += 12.0f;
			cXPos = 0;
			continue;
		}

		uint8_t x = uint8_t(str[i]);

		buildChar(x, cXPos, cYPos);

		cXPos += m_charWidthFloat[x];
	}

	t.draw();

	glPopMatrix();
}

void Font::onGraphicsReset()
{
	init(m_pOptions);
}

int Font::height(const std::string& str)
{
	if (str.empty()) return 0;

	int res = 0; // note: starting at 0 looks wrong
	
	for (int i = 0; i < int(str.size()); i++)
		if (str[i] == '\n')
			res += 12;

	return res;
}

int Font::width(const std::string& str)
{
	int maxLineWidth = 0, currentLineWidth = 0;

	for (int i = 0; i < int(str.size()); i++)
	{
		char chr = str[i];

		if (chr == COLOR_START_CHAR)
		{
			// skip the color code as well
			i++;
			continue;
		}
		if (chr == '\n')
		{
			if (maxLineWidth < currentLineWidth)
				maxLineWidth = currentLineWidth;
			currentLineWidth = 0;
		}

		currentLineWidth += m_charWidthInt[uint8_t(str[i])];
	}

	if (maxLineWidth < currentLineWidth)
		maxLineWidth = currentLineWidth;

	return maxLineWidth;
}
