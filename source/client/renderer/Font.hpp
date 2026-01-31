/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Textures.hpp"
#include "client/options/Options.hpp"
#include "renderer/MaterialPtr.hpp"

class Font
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr ui_text;

		Materials();
	};

public:
	Font(Options* pOpts, const std::string& fileName, Textures* pTexs);

	void init(Options* pOpts);
	void buildChar(unsigned char chr, float x, float y);
	void draw(const std::string&, int x, int y, const Color& color);
	void draw(const std::string&, int x, int y, const Color& color, bool bShadow);
	void drawSlow(const std::string&, int x, int y, const Color& color, bool bShadow);
	void drawShadow(const std::string&, int x, int y, const Color& color);
	void drawLegacy(const std::string&, int x, int y, const Color& color, float scale = 2.0f, bool shadow = false);
	void drawLegacyShadow(const std::string&, int x, int y, const Color& color, float scale = 2.0f);
	void drawString(const std::string&, int x, int y, const Color& color, bool hasShadow, bool isLegacy = false);
	void drawOutlinedString(const std::string&, int x, int y, const Color& color, const Color& outlineColor, float scale = 4.0f, int thickness = 2);
	void drawWordWrap(const std::string&, int x, int y, int color, int width, int lineHeight = 8, bool shadow = false, bool isLegacy = false);
	void drawWordWrap(const std::vector<std::string>&, int x, int y, int color, int lineHeight = 8, bool shadow = false, bool isLegacy = false);

	void onGraphicsReset();

	int width(const std::string& str);
	std::vector<std::string> split(const std::string& str, int width);
	int height(const std::string& str, int maxWidth);

private:
	int field_0; 
	int m_charWidthInt[256];
	float m_charWidthFloat[256];
	// huge gap, don't know why it's there...
	std::string m_fileName;
	Options* m_pOptions;
	Textures* m_pTextures;
	Materials m_materials;
};

