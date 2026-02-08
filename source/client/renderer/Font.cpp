/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Font.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "client/renderer/renderer/Tesselator.hpp"
#include "renderer/ShaderConstants.hpp"
#include "renderer/MatrixStack.hpp"
#include <sstream>

constexpr char COLOR_START_CHAR = '\xa7';

Font::Materials::Materials()
{
	MATERIAL_PTR(common, ui_text);
}

Font::Font(Options* pOpts, const std::string& fileName, Textures* pTexs) :
	m_fileName(fileName), m_pOptions(pOpts), m_pTextures(pTexs)
{
	field_0 = 0;

	init(pOpts);
}

void Font::init(Options* pOpts)
{
	TextureData* pTexture = m_pTextures->getTextureData(m_fileName, true);
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
				int pixelDataIndex = pTexture->m_imageData.m_width * 8 * y + 8 * x + j;

				for (int k = 0; k < 8; k++)
				{
					if ((uint8_t)pTexture->getData()[pixelDataIndex] != 0)
					{
						if (widthMax < j)
							widthMax = j;
					}

					pixelDataIndex += pTexture->m_imageData.m_width;
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

void Font::draw(const std::string& str, int x, int y, const Color& color)
{
	draw(str, x, y, color, false);
}

void Font::drawShadow(const std::string& str, int x, int y, const Color& color)
{
	draw(str, x + 1, y + 1, color, true);
	draw(str, x, y, color, false);
}

void Font::drawScalable(const std::string& str, int x, int y, const Color& color, float scale, bool shadow)
{
	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(Vec3(x, y, 0));
	matrix->scale(scale);
	draw(str, 0, 0, color, shadow);
}

void Font::drawScalableShadow(const std::string& str, int x, int y, const Color& color, float scale)
{
	drawScalable(str, x + 1, y + 1, color, scale, true);
	drawScalable(str, x, y, color, scale);
}

void Font::drawString(const std::string& str, int x, int y, const Color& color, bool hasShadow, bool isConsole)
{
	if (hasShadow)
	{
		if (isConsole)
			drawScalableShadow(str, x, y, color);
		else
			drawShadow(str, x, y, color);
	}
	else
	{
		if (isConsole)
			drawScalable(str, x, y, color);
		else
			draw(str, x, y, color);
	}
}

void Font::drawOutlinedString(const std::string& str, int x, int y, const Color& color, const Color& outlineColor, float scale, int thickness)
{
	int translations[] = {0, thickness, -thickness};
	for (int xi = 0; xi < 3; ++xi)
	{
		int t = translations[xi];
		for (int yi = 0; yi < 3; ++yi)
		{
			int t1 = translations[yi];
			if (t != 0 || t1 != 0) {
				MatrixStack::Ref matrix = MatrixStack::World.push();
				matrix->translate(Vec3(t, t1, 0));
				drawScalable(str, x, y, outlineColor, scale, false);
			}
		}
	}

	drawScalable(str, x, y, color, scale, false);
}

void Font::drawWordWrap(const std::string& str, int x, int y, int color, int width, int lineHeight, bool shadow, bool isConsole)
{
	drawWordWrap(split(str, width), x, y, color, lineHeight, shadow, isConsole);
}

void Font::drawWordWrap(const std::vector<std::string>& lines, int x, int y, int color, int lineHeight, bool shadow, bool isConsole)
{
	for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
		drawString(*it, x, y, color, shadow, isConsole);
		y += lineHeight;
	}
}

void Font::draw(const std::string& str, int x, int y, const Color& color, bool bShadow)
{
	drawSlow(str, x, y, color, bShadow);
}

void Font::drawSlow(const std::string& str, int x, int y, const Color& color, bool bShadow)
{
	if (str.empty()) return;

	if (bShadow)
	{
		currentShaderDarkColor = Color(0.25f, 0.25f, 0.25f);
	}
	else
	{
		currentShaderDarkColor = Color::WHITE;
	}

	m_pTextures->loadAndBindTexture(m_fileName);

	Color finalColor = color;
	// For hex colors which don't specify an alpha
	if (finalColor.a == 0.0f)
		finalColor.a = 1.0f;

#ifndef FEATURE_GFX_SHADERS
	finalColor *= currentShaderDarkColor;
#endif

	MatrixStack::Ref mtx = MatrixStack::World.push();
	mtx->translate(Vec3(x, y, 0.0f));

	Tesselator& t = Tesselator::instance;
	t.begin(4 * str.size());

	t.color(finalColor);

	float cXPos = 0.0f, cYPos = 0.0f;

	for (size_t i = 0; i < str.size(); i++)
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

	t.draw(m_materials.ui_text);
}

void Font::onGraphicsReset()
{
	init(m_pOptions);
}

int Font::height(const std::string& str, int maxWidth)
{
	return split(str, maxWidth).size() * 8;
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

std::vector<std::string> Font::split(const std::string& text, int maxWidth)
{
	std::vector<std::string> lines;

	std::vector<std::string> paragraphs;
	size_t start = 0;
	size_t newlinePos = text.find('\n');
	while (newlinePos != std::string::npos)
	{
		paragraphs.push_back(text.substr(start, newlinePos - start));
		start = newlinePos + 1;
		newlinePos = text.find('\n', start);
	}
	paragraphs.push_back(text.substr(start));

	for (std::vector<std::string>::iterator it = paragraphs.begin(); it != paragraphs.end(); ++it)
	{
		std::string& paragraph = *it;

		if (paragraph.empty())
		{
			lines.push_back("");
			continue;
		}

		std::string currentLine;
		std::istringstream iss(paragraph);
		std::string word;

		while (iss >> word) {
			std::string testLine = currentLine.empty() ? word : currentLine + " " + word;

			if (width(testLine) <= maxWidth)
				currentLine = testLine;
			else
			{
				if (!currentLine.empty())
				{
					lines.push_back(currentLine);
					currentLine.clear();
				}

				while (!word.empty() && width(word) > maxWidth)
				{
					size_t breakPos = 0;
					for (size_t j = 1; j <= word.length(); ++j)
					{
						if (width(word.substr(0, j)) <= maxWidth)
							breakPos = j;
						else
							break;
					}

					if (breakPos == 0) breakPos = 1;

					std::string chunk = word.substr(0, breakPos);
					lines.push_back(chunk);
					word = word.substr(breakPos);
				}

				currentLine = word;
			}
		}

		if (!currentLine.empty())
			lines.push_back(currentLine);
	}

	while (!lines.empty() && lines.back().empty())
		lines.pop_back();

	if (lines.empty())
		lines.push_back("");

	return lines;
}