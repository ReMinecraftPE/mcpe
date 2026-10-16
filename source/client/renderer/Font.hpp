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
#include "client/renderer/renderer/Tesselator.hpp"
#include "client/renderer/texture/TextureData.hpp"
#include "common/utility/HashMap.hpp"
#include "common/utility/FixedCircularBuffer.hpp"
#include "compat/LegacyCPP_Compat.hpp"
#include <vector>
#include <set>

struct FontCacheKey
{
public:
	std::string str;
	Color color;

public:
	FontCacheKey()
	{
	}

	FontCacheKey(const std::string& str, const Color& color)
		: str(str)
		, color(color)
	{
	}

	bool operator==(const FontCacheKey& other) const
	{
		return str == other.str && color == other.color;
	}

	bool operator!=(const FontCacheKey& other) const
	{
		return str != other.str || color != other.color;
	}
};

template<>
struct HashFunction<Color>
{
	size_t operator()(const Color& key) const;
};

template<>
struct HashFunction<FontCacheKey>
{
	size_t operator()(const FontCacheKey& key) const;
};

#define C_FONT_RENDER_GLYPH_SIZE 8.0f
#define C_FONT_MAX_CACHE_SIZE 500

class Font
{
public:
	static constexpr int NUM_ASCII_CHARS = 256; // Whole ASCII set
	static constexpr int NUM_GLYPHS = 0x1FFFF + 1; // Plane 0 to 1

	// COMMON //
	static constexpr int COMMON_MAP_DIMENSION = 16; // number of glyphs on one row/column
	static constexpr int COMMON_MAP_TOTAL = COMMON_MAP_DIMENSION * COMMON_MAP_DIMENSION; // total number of glyphs in one map

	// ASCII MAP //
	static constexpr int ASCII_MAP_GLYPH_SIZE = 8; // size of each glyph on the map in pixels
	static constexpr int ASCII_MAP_PIXEL_DIMENSION = COMMON_MAP_DIMENSION * ASCII_MAP_GLYPH_SIZE; // size of one row/column in pixels

	// UNICODE MAPS //
	static constexpr int UNICODE_MAP_GLYPH_SIZE = 16; // size of each glyph on the map in pixels
	static constexpr int UNICODE_MAP_PIXEL_DIMENSION = COMMON_MAP_DIMENSION * UNICODE_MAP_GLYPH_SIZE; // size of one row/column in pixels

private:
	class Materials
	{
	public:
		mce::MaterialPtr ui_text;

		Materials();
	};

	class Format
	{
	public:
		Color color;
		bool italic;
		bool bold;
		bool strikeThrough;
		bool underline;

		Format()
			: color()
			, italic(false)
			, bold(false)
			, strikeThrough(false)
			, underline(false)
		{
		}
	};

	class GlyphQuad
	{
	public:
		int c;
		float x;
		float y;
		Color color;
		bool italic;
		bool isAscii;

	public:
		GlyphQuad(int c, float x, float y, const Color& color, bool italic, bool isAscii);

	public:
		void append(Tesselator& t);
	};

	class TextObject
	{
	private:
		class Page
		{
		public:
			mce::Mesh mesh;
			TextureData* textureData;

		public:
			Page(mce::Mesh& mesh, TextureData* textureData);
		};

	private:
		std::vector<Page> pages;

	public:
		TextObject();
		~TextObject();

	public:
		void addPage(mce::Mesh& mesh, TextureData* textureData);
		void render(const mce::MaterialPtr& material);
	};

	class TextObjectGroup
	{
	public:
		TextObject* base;
		TextObject* shadow;
		bool hasUnicode;
		bool requiresSeparateShadowTextObject;

	public:
		TextObjectGroup();
		~TextObjectGroup();

		MC_CTOR_MOVE(TextObjectGroup);
		MC_FUNC_MOVE(TextObjectGroup);

	private:
		void _move(TextObjectGroup& other);

	public:
		TextObject& getOrCreate(Font& font, const std::string& str, const Color& color, bool isShadow);
		void adjustRenderPosition(float& x, float& y, bool isShadow) const;
	};

	class Line
	{
	public:
		float fromX;
		float toX;
		float y;

	public:
		Line(float fromX, float toX, float y);

	public:
		void append(const Font& font, Tesselator& t, float height, float yShift);
	};

	class LineMap
	{
	public:
		typedef std::vector<Line> LineList;
		typedef HashMap<Color, std::vector<Line> > Map;

	public:
		Map lines;
		int lineCount;

	public:
		LineMap();

	public:
		LineList& getOrCreateList(const Color& color);
		Line& createLine(const Color& color, float fromX, float y);
		Line& getLastLine(const Color& color);
		void clearLines();
		void append(const Font& font, Tesselator& t, float height, float yShift);
	};

	friend class TextObjectGroup;
	friend class Line;

private:
	typedef HashMap<FontCacheKey, TextObjectGroup> TextObjectCacheMap;

private:
	void _init(Options* pOpts);
	void _computeAsciiSizes();
	void _readUnicodeSizes(const std::string& filePath);

	TextureData* _getAsciiTextureData();
	TextureData* _getUnicodeTextureData(int id);
	TextureData* _getTextureData(int id);

	void _computeRequireSeperateTextObjectForShadow(TextObjectGroup* group, const std::string& str);
	float _buildChar(int c, float x, float y, const Format& format, bool isShadow);
	void _buildLines(Tesselator& t);
	TextObject* _createTextObject(const std::string& str, const Color& color, bool isShadow);

	void _buildCharSimple(uint8_t c, float x, float y);

public:
	Font(Options* pOpts, const std::string& fileName, Textures* pTexs);
	virtual ~Font();

	void resetFormat(const Color& baseColor);

	void drawCached(const std::string& str, int x, int y, const Color& color, bool isShadow);
	void drawSimple(const std::string& str, int x, int y, const Color& color, bool bShadow);

	void draw(const std::string& str, int x, int y, const Color& color);
	void draw(const std::string& str, int x, int y, const Color& color, bool bShadow);
	void drawShadow(const std::string& str, int x, int y, const Color& color);
	void drawScalable(const std::string& str, int x, int y, const Color& color, float scale = 2.0f, bool shadow = false);
	virtual void drawScalableShadow(const std::string& str, int x, int y, const Color& color, float scale = 2.0f);
	virtual void drawString(const std::string& str, int x, int y, const Color& color, bool hasShadow);
	void drawOutlinedString(const std::string& str, int x, int y, const Color& color, const Color& outlineColor, float scale = 4.0f, int thickness = 2);
	void drawWordWrap(const std::string& str, int x, int y, const Color& color, int width, int lineHeight = 8, bool shadow = false);
	void drawWordWrap(const std::vector<std::string>& lines, int x, int y, const Color& color, int lineHeight = 8, bool shadow = false);

	void drawSimple(const std::string&, int x, int y, const Color& color);
	void drawSimpleShadow(const std::string&, int x, int y, const Color& color);
	void drawSimpleScalable(const std::string&, int x, int y, const Color& color, float scale = 2.0f, bool shadow = false);
	void drawSimpleScalableShadow(const std::string&, int x, int y, const Color& color, float scale = 2.0f);

	// Controls if cache will be used before creating a new text object
	bool getCachingEnabled() const
	{
		return m_cachingEnabled;
	}
	void setCachingEnabled(bool enabled)
	{
		m_cachingEnabled = enabled;
	}

	// Controls whether or not format should be preserved through text object builds
	bool getResetFormatOnBuild() const
	{
		return m_resetFormatOnBuild;
	}
	void setResetFormatOnBuild(bool enabled)
	{
		m_resetFormatOnBuild = enabled;
	}

	void onGraphicsReset();
	void clearTextObjectCache();

	std::vector<std::string> split(const std::string& str, int width);

	int height(const std::string& str) const;
	int width(const std::string& str) const;

	int heightSimple(const std::string& str) const;
	int widthSimple(const std::string& str) const;

public:
	static bool ContainsAsciiCharacters(const std::string& str);
	static bool ContainsUnicodeCharacters(const std::string& str);

private:
	static bool _IsAsciiCharacter(int c);
	static int _GetGlyphMapId(int c);
	static bool _IsColorFormatCode(uint8_t c);
	static const Color& _GetColorFromColorFormatCode(uint8_t c);
	static int _CountNewLines(const std::string& str);

private:
	std::string m_asciiFileName;
	Options* m_options;
	Textures* m_textures;
	Materials m_materials;

	uint8_t m_charWidth[NUM_GLYPHS];
	std::vector<GlyphQuad> m_glyphMapQuads[NUM_GLYPHS / COMMON_MAP_TOTAL];
	std::set<int> m_usedGlyphMapQuads;

	TextObjectCacheMap m_textObjectCache;
	FixedCircularBuffer<FontCacheKey, C_FONT_MAX_CACHE_SIZE> m_recentTextObjectCaches;
	bool m_cachingEnabled;
	bool m_resetFormatOnBuild;

	Format m_format;

	LineMap m_strikeThroughLines;
	LineMap m_underlineLines;

	// TODO: switch to integer Vec2
	int m_pixelX;
	int m_pixelY;

protected:
	float m_unicodeShadowOffset;
};
