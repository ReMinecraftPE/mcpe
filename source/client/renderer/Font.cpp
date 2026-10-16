#include <stdexcept>
#include "client/renderer/Font.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "client/resources/Resource.hpp"
#include "renderer/ShaderConstants.hpp"
#include "common/utility/hashing/HashCombine.hpp"
#include "common/Util.hpp"
#include "thirdparty/utf8proc/utf8proc.h"

static const Color COLOR_FROM_CODES[] = {
	Color::FromRGB(0, 0, 0),       // 0 - black
	Color::FromRGB(0, 0, 170),     // 1 - dark blue
	Color::FromRGB(0, 170, 0),     // 2 - dark green
	Color::FromRGB(0, 170, 170),   // 3 - dark aqua
	Color::FromRGB(170, 0, 0),     // 4 - dark red
	Color::FromRGB(170, 0, 170),   // 5 - dark purple
	Color::FromRGB(255, 170, 0),   // 6 - gold
	Color::FromRGB(170, 170, 170), // 7 - gray
	Color::FromRGB(85, 85, 85),    // 8 - dark gray
	Color::FromRGB(85, 85, 255),   // 9 - blue
	Color::FromRGB(85, 255, 85),   // a - green
	Color::FromRGB(85, 255, 255),  // b - aqua
	Color::FromRGB(255, 85, 85),   // c - red
	Color::FromRGB(255, 85, 255),  // d - light purple
	Color::FromRGB(255, 255, 85),  // e - yellow
	Color::FromRGB(255, 255, 255)  // f - white
};

#define C_FORMATTING_START_CHARACTER (uint8_t)'\xa7'

#define C_SPACE_WIDTH (uint8_t)2
#define C_SPACING_BETWEEN_CHARS (uint8_t)2
#define C_NEW_LINE_SPACING 2.0f // spacing on the Y-axis created by new lines
#define C_ITALIC_SHIFT 3.0f // shifting by 3 looks fine on both ascii and unicode
#define C_STRIKETHROUGH_Y_SHIFT (C_FONT_RENDER_GLYPH_SIZE / 2.0f) - 1.0f
#define C_UNDERLINE_Y_SHIFT C_FONT_RENDER_GLYPH_SIZE

// character to use for characters not in our valid ranges
#define C_UNK_CHAR 65533

size_t HashFunction<Color>::operator()(const Color& key) const
{
	return key.toUInt32();
}

size_t HashFunction<FontCacheKey>::operator()(const FontCacheKey& key) const
{
	size_t hash = HashFunction<std::string>()(key.str);
	hash_combine(hash, key.color);
	return hash;
}

Font::Materials::Materials()
{
	MATERIAL_PTR(common, ui_text);
}

Font::GlyphQuad::GlyphQuad(int c, float x, float y, const Color& color, bool italic, bool isAscii)
	: c(c)
	, x(x)
	, y(y)
	, color(color)
	, italic(italic)
	, isAscii(isAscii)
{
}

void Font::GlyphQuad::append(Tesselator& t)
{
	t.color(color);

	const int mapGlyphSize = isAscii ? ASCII_MAP_GLYPH_SIZE : UNICODE_MAP_GLYPH_SIZE;

	const int u = (c % COMMON_MAP_DIMENSION) * mapGlyphSize;
	const int v = (c / COMMON_MAP_DIMENSION) * mapGlyphSize;

	const int mapSize = isAscii ? ASCII_MAP_PIXEL_DIMENSION : UNICODE_MAP_PIXEL_DIMENSION;
	const float D = (1.0f / mapSize);

	// if this glyph is italic, shift the top corner by X to the right
	float shift = italic ? C_ITALIC_SHIFT : 0.0f;

	t.vertexUV(x,                                    y + C_FONT_RENDER_GLYPH_SIZE, 0.0f, u * D,                  (v + mapGlyphSize) * D);
	t.vertexUV(x + C_FONT_RENDER_GLYPH_SIZE,         y + C_FONT_RENDER_GLYPH_SIZE, 0.0f, (u + mapGlyphSize) * D, (v + mapGlyphSize) * D);
	t.vertexUV(x + C_FONT_RENDER_GLYPH_SIZE + shift, y,                     0.0f, (u + mapGlyphSize) * D, v * D);
	t.vertexUV(x + shift,                            y,                     0.0f, u * D,                  v * D);
}

Font::TextObject::TextObject()
{
}

Font::TextObject::~TextObject()
{
}

void Font::TextObject::addPage(mce::Mesh& mesh, TextureData* textureData)
{
	assert(textureData);
	pages.push_back(Page(mesh, textureData));
}

void Font::TextObject::render(const mce::MaterialPtr& material)
{
	for (std::vector<Page>::iterator it = pages.begin(); it != pages.end(); it++)
	{
		Page& page = *it;
		page.textureData->bind();
		page.mesh.render(material);
	}
}

Font::TextObject::Page::Page(mce::Mesh& mesh, TextureData* textureData)
	: mesh(mesh)
	, textureData(textureData)
{
}

Font::TextObjectGroup::TextObjectGroup()
	: base(nullptr)
	, shadow(nullptr)
	, hasUnicode(false)
	, requiresSeparateShadowTextObject(false)
{
}

Font::TextObjectGroup::~TextObjectGroup()
{
	delete base;
	delete shadow;
}

void Font::TextObjectGroup::_move(TextObjectGroup& other)
{
	base = other.base;
	shadow = other.shadow;
	hasUnicode = other.hasUnicode;
	requiresSeparateShadowTextObject = other.requiresSeparateShadowTextObject;
	
	other.base = nullptr;
	other.shadow = nullptr;
}

Font::TextObject& Font::TextObjectGroup::getOrCreate(Font& font, const std::string& str, const Color& color, bool isShadow)
{
	if (!requiresSeparateShadowTextObject)
	{
		if (!base)
			base = font._createTextObject(str, color, false);

		return *base;
	}
	else
	{
		if (isShadow)
		{
			if (!shadow)
				shadow = font._createTextObject(str, color, true);

			return *shadow;
		}
		else
		{
			if (!base)
				base = font._createTextObject(str, color, false);

			return *base;
		}
	}
}

void Font::TextObjectGroup::adjustRenderPosition(float& x, float& y, bool isShadow) const
{
	if (!requiresSeparateShadowTextObject)
	{
		// offset base mesh if we're rendering a shadow
		if (isShadow)
		{
			float offset = hasUnicode ? (C_FONT_RENDER_GLYPH_SIZE / UNICODE_MAP_GLYPH_SIZE) : 1.0f;
			x += offset;
			y += offset;
		}
	}
}

Font::Line::Line(float fromX, float toX, float y)
	: fromX(fromX)
	, toX(toX)
	, y(y)
{
}

Font::LineMap::LineMap()
	: lineCount(0)
{
}

void Font::Line::append(const Font& font, Tesselator& t, float height, float yShift)
{
	if (fromX == toX)
		return;

	assert(font.m_pixelX != -1 && font.m_pixelY != -1);

	const int u = font.m_pixelX;
	const int v = font.m_pixelY;

	const int mapSize = ASCII_MAP_PIXEL_DIMENSION; // always the ascii map for now
	const float D = (1.0f / mapSize);

	t.vertexUV(fromX, y + height + yShift, 0.0f, u * D,          (v + 1.0f) * D);
	t.vertexUV(toX,   y + height + yShift, 0.0f, (u + 1.0f) * D, (v + 1.0f) * D);
	t.vertexUV(toX,   y + yShift,          0.0f, (u + 1.0f) * D, v * D);
	t.vertexUV(fromX, y + yShift,          0.0f, u * D,          v * D);
}

Font::LineMap::LineList& Font::LineMap::getOrCreateList(const Color& color)
{
	// map automatically creates entry
	return lines[color];
}

Font::Line& Font::LineMap::createLine(const Color& color, float fromX, float y)
{
	LineList& list = getOrCreateList(color);
	list.push_back(Line(fromX, 0.0f, y));
	++lineCount;
	return list.back();
}

Font::Line& Font::LineMap::getLastLine(const Color& color)
{
	LineList& list = getOrCreateList(color);
	return list.back();
}

void Font::LineMap::clearLines()
{
	lines.clear();
	lineCount = 0;
}

void Font::LineMap::append(const Font& font, Tesselator& t, float height, float yShift)
{
	for (Map::iterator it = lines.begin(); it != lines.end(); it++)
	{
		const Color& color = it->first;
		LineList& list = it->second;

		assert(!list.empty());

		t.color(color);

		for (LineList::iterator it = list.begin(); it != list.end(); it++)
		{
			it->append(font, t, height, yShift);
		}
	}
}

Font::Font(Options* options, const std::string& fileName, Textures* textures)
	: m_asciiFileName(fileName)
	, m_options(options)
	, m_textures(textures)
	, m_cachingEnabled(true)
	, m_resetFormatOnBuild(true)
	, m_pixelX(-1)
	, m_pixelY(-1)
	, m_unicodeShadowOffset(C_FONT_RENDER_GLYPH_SIZE / UNICODE_MAP_GLYPH_SIZE)
{
	_init(options);
}

Font::~Font()
{
}

void Font::_init(Options* pOpts)
{
	memset(m_charWidth, 0, sizeof(m_charWidth));
	_computeAsciiSizes();
	_readUnicodeSizes("font/glyphs/glyph_sizes.bin");
}

void Font::_computeAsciiSizes()
{
	m_pixelX = -1;
	m_pixelY = -1;

	TextureData* defaultTexture = m_textures->getTextureData(m_asciiFileName, false);
	if (!defaultTexture)
		throw std::runtime_error("Missing ASCII font image");

	if (defaultTexture->m_imageData.m_width != ASCII_MAP_PIXEL_DIMENSION && defaultTexture->m_imageData.m_height != ASCII_MAP_PIXEL_DIMENSION)
		throw std::runtime_error("Bad ASCII font image: wrong dimensions");

	for (int i = 0; i < NUM_ASCII_CHARS; ++i)
	{
		uint8_t c = static_cast<uint8_t>(i);
		uint8_t widthMax = 0;

		if (c == ' ')
		{
			widthMax = C_SPACE_WIDTH;
		}
		else
		{
			int x = c % COMMON_MAP_DIMENSION;
			int y = c / COMMON_MAP_DIMENSION;

			int pixelDataIndexBase = (ASCII_MAP_GLYPH_SIZE * x) + (ASCII_MAP_PIXEL_DIMENSION * ASCII_MAP_GLYPH_SIZE * y);

			for (int xOffset = ASCII_MAP_GLYPH_SIZE - 1; xOffset >= 0; --xOffset)
			{
				for (int yOffset = 0; yOffset < ASCII_MAP_GLYPH_SIZE; ++yOffset)
				{
					uint32_t pixelData = defaultTexture->getData()[pixelDataIndexBase + xOffset + (ASCII_MAP_PIXEL_DIMENSION * yOffset)];
					if (static_cast<uint8_t>(pixelData) != 0) // check for channel data
					{
						widthMax = xOffset;

						// mark a position where there is a opaque pixel in the ascii map
						if (m_pixelX == -1 || m_pixelY == -1)
						{
							m_pixelX = ASCII_MAP_GLYPH_SIZE * x + xOffset;
							m_pixelY = ASCII_MAP_GLYPH_SIZE * y + yOffset;
						}

						goto done;
					}
				}
			}

		done:
			;
		}

		m_charWidth[c] = widthMax + C_SPACING_BETWEEN_CHARS;
	}
}

void Font::_readUnicodeSizes(const std::string& filePath)
{
	std::string fileData;
	Resource::load(filePath, fileData);

	if (fileData.size() != NUM_GLYPHS)
	{
		LOG_E("Glyph file size is invalid. Got: %d, expected: %d", fileData.size(), NUM_GLYPHS);
		throw std::runtime_error("Bad glyph sizes file");
	}

	// skip all ascii characters as their widths are computed in _computeAsciiSizes
	for (int i = NUM_ASCII_CHARS; i < NUM_GLYPHS; ++i)
	{
		// these widths are for font size 16
		// we render at font size 8
		m_charWidth[i] = static_cast<uint8_t>(fileData[i] / (COMMON_MAP_DIMENSION / C_FONT_RENDER_GLYPH_SIZE)) + C_SPACING_BETWEEN_CHARS;
	}
}

TextureData* Font::_getAsciiTextureData()
{
	return m_textures->getTextureData(m_asciiFileName, false);
}

TextureData* Font::_getUnicodeTextureData(int id)
{
	std::string fileName = "font/glyphs/glyph_" + Util::toString(id) + ".png";
	return m_textures->getTextureData(fileName, false);
}

TextureData* Font::_getTextureData(int id)
{
	// id == 0 uses the ascii/default texture map
	return id == 0 ? _getAsciiTextureData() : _getUnicodeTextureData(id);
}

void Font::resetFormat(const Color& baseColor)
{
	m_format.color = baseColor;
	m_format.italic = false;
	m_format.bold = false;
	m_format.strikeThrough = false;
	m_format.underline = false;
}

void Font::_computeRequireSeperateTextObjectForShadow(TextObjectGroup* group, const std::string& str)
{
#ifndef FEATURE_GFX_SHADERS
	// we will always need to use a seperate text object for shadows
	// if shaders are not enabled
	// as finalColor is multiplied by currentShaderDarkColor
	group->requiresSeparateShadowTextObject = true;
#else
	if (group->hasUnicode)
	{
		// we need a seperate text object for shadows if ascii and unicode characters are present
		// since ascii and unicode characters need different shadow text position offsets
		if (ContainsAsciiCharacters(str))
			group->requiresSeparateShadowTextObject = true;
	}
#endif
}

float Font::_buildChar(int c, float x, float y, const Format& format, bool isShadow)
{
	assert(c < NUM_GLYPHS);

	// ignore space characters (they are always empty so they don't need to be rendered)
	if (c == ' ')
		return static_cast<float>(C_SPACE_WIDTH);

	bool isAscii = _IsAsciiCharacter(c);
	float width = static_cast<float>(m_charWidth[c]);

	int glyphMapId = _GetGlyphMapId(c);
	std::vector<GlyphQuad>& quads = m_glyphMapQuads[glyphMapId];

	if (isShadow)
	{
		float offset = isAscii ? 1.0f : m_unicodeShadowOffset;
		x += offset;
		y += offset;
	}

	if (format.bold)
	{
		float widthAddition = isAscii ? 1.0f : (C_FONT_RENDER_GLYPH_SIZE / UNICODE_MAP_GLYPH_SIZE);
		width += widthAddition;

		for (int i = 0; i < 2; i++)
			quads.push_back(GlyphQuad(c, x + (widthAddition * i), y, format.color, format.italic, isAscii));
	}
	else
	{
		quads.push_back(GlyphQuad(c, x, y, format.color, format.italic, isAscii));
	}

	m_usedGlyphMapQuads.insert(glyphMapId);

	return width;
}

void Font::_buildLines(Tesselator& t)
{
	m_strikeThroughLines.append(*this, t, 1.0f, C_STRIKETHROUGH_Y_SHIFT);
	m_underlineLines.append(*this, t, 1.0f, C_UNDERLINE_Y_SHIFT);
}

Font::TextObject* Font::_createTextObject(const std::string& str, const Color& color, bool isShadow)
{
	TextObject* textObject = new TextObject();

	Line* currentStrikeThroughLine = nullptr;
	Line* currentUnderlineLine = nullptr;

	if (m_resetFormatOnBuild)
	{
		resetFormat(color);
	}
	else
	{
		if (m_format.strikeThrough)
			currentStrikeThroughLine = &m_strikeThroughLines.createLine(m_format.color, 0.0f, 0.0f);
		if (m_format.underline)
			currentUnderlineLine = &m_underlineLines.createLine(m_format.color, 0.0f, 0.0f);
	}

	const uint8_t* data = reinterpret_cast<const uint8_t*>(str.c_str());
	utf8proc_ssize_t len = str.size();

	float x = 0.0f;
	float y = 0.0f;

	utf8proc_ssize_t charLen;
	int c;
	while ((charLen = utf8proc_iterate(data, len, &c)) > 0)
	{
		assert(c >= 0);

		data += charLen;
		len -= charLen;

		if (c >= NUM_GLYPHS)
			c = C_UNK_CHAR;

		if (c == C_FORMATTING_START_CHARACTER)
		{
			if (len > 0)
			{
				// format code should always be ascii
				uint8_t formatCode = *data;

				if (_IsColorFormatCode(formatCode))
				{
					m_format.color = _GetColorFromColorFormatCode(formatCode);
#ifndef FEATURE_GFX_SHADERS
					m_format.color *= currentShaderDarkColor;
#endif

					float offset = isShadow ? 1.0f : 0.0f;
					if (m_format.strikeThrough)
					{
						assert(currentStrikeThroughLine);
						currentStrikeThroughLine->toX = x + offset;
						currentStrikeThroughLine = &m_strikeThroughLines.createLine(m_format.color, x + offset, y + offset);
					}

					if (m_format.underline)
					{
						assert(currentUnderlineLine);
						currentUnderlineLine->toX = x + offset;
						currentUnderlineLine = &m_underlineLines.createLine(m_format.color, x + offset, y + offset);
					}
				}
				else
				{
					float offset;

					// TODO: obfuscation
					switch (formatCode)
					{
					case 'l':
						m_format.bold = true;
						break;

					case 'o':
						m_format.italic = true;
						break;

					case 'm':
						if (!m_format.strikeThrough)
						{
							m_format.strikeThrough = true;
							offset = isShadow ? 1.0f : 0.0f;
							currentStrikeThroughLine = &m_strikeThroughLines.createLine(m_format.color, x + offset, y + offset);
						}
						break;

					case 'n':
						if (!m_format.underline)
						{
							m_format.underline = true;
							offset = isShadow ? 1.0f : 0.0f;
							currentUnderlineLine = &m_underlineLines.createLine(m_format.color, x + offset, y + offset);
						}
						break;

					case 'r':
						resetFormat(color);

						offset = isShadow ? 1.0f : 0.0f;
						if (currentStrikeThroughLine)
							currentStrikeThroughLine->toX = x + offset;
						if (currentUnderlineLine)
							currentUnderlineLine->toX = x + offset;
						break;
					}
				}
				
				data++;
				len--;
			}
		}
		else if (c == '\n')
		{
			float offset = isShadow ? 1.0f : 0.0f;

			if (m_format.strikeThrough)
			{
				assert(currentStrikeThroughLine);
				currentStrikeThroughLine->toX = x + offset;
			}

			if (m_format.underline)
			{
				assert(currentUnderlineLine);
				currentUnderlineLine->toX = x + offset;
			}

			x = 0.0f;
			y += C_FONT_RENDER_GLYPH_SIZE + C_NEW_LINE_SPACING;

			if (m_format.strikeThrough)
				currentStrikeThroughLine = &m_strikeThroughLines.createLine(m_format.color, offset, y + offset);

			if (m_format.underline)
				currentUnderlineLine = &m_strikeThroughLines.createLine(m_format.color, offset, y + offset);
		}
		else
		{
			x += _buildChar(c, x, y, m_format, isShadow);
		}
	}

	// assert no utf8 errors
	assert(charLen >= 0);

	// end strikethrough and underline lines
	float lineOffset = isShadow ? 1.0f : 0.0f;
	if (m_format.strikeThrough)
	{
		assert(currentStrikeThroughLine);
		currentStrikeThroughLine->toX = x + lineOffset;
	}

	if (m_format.underline)
	{
		assert(currentUnderlineLine);
		currentUnderlineLine->toX = x + lineOffset;
	}
		
	// build meshes
	Tesselator& t = Tesselator::instance;

	for (std::set<int>::const_iterator it = m_usedGlyphMapQuads.begin(); it != m_usedGlyphMapQuads.end(); it++)
	{
		int id = *it;
		std::vector<GlyphQuad>& quads = m_glyphMapQuads[id];

		TextureData* textureData = _getTextureData(id);
		if (textureData) // there is a glyph map available for this
		{
			t.begin(quads.size() * 4);

			for (std::vector<GlyphQuad>::iterator it = quads.begin(); it != quads.end(); it++)
				(*it).append(t);

			mce::Mesh mesh = t.end();
			textObject->addPage(mesh, textureData);
		}

		// cleanup
		m_glyphMapQuads[id].clear();
	}

	// build line meshes
	bool needsToBuildLines = m_strikeThroughLines.lineCount != 0 || m_underlineLines.lineCount != 0;

	if (needsToBuildLines)
	{
		// we use the ascii map to build lines
		TextureData* textureData = _getAsciiTextureData();
		if (textureData)
		{
			int maxVertices = m_strikeThroughLines.lineCount * 4 + m_underlineLines.lineCount * 4;

			t.begin(maxVertices);

			_buildLines(t);

			mce::Mesh mesh = t.end();
			textObject->addPage(mesh, textureData);
		}

		// cleanup
		m_strikeThroughLines.clearLines();
		m_underlineLines.clearLines();
	}

	// cleanup
	m_usedGlyphMapQuads.clear();

	return textObject;
}

void Font::drawCached(const std::string& str, int x, int y, const Color& color, bool isShadow)
{
	if (str.empty())
		return;

	const mce::MaterialPtr& material = m_materials.ui_text;

	if (isShadow)
		currentShaderDarkColor = Color(0.25f, 0.25f, 0.25f);
	else
		currentShaderDarkColor = Color::WHITE;

	Color finalColor = color;
	// For hex colors which don't specify an alpha
	if (finalColor.a == 0.0f)
		finalColor.a = 1.0f;

#ifndef FEATURE_GFX_SHADERS
	finalColor *= currentShaderDarkColor;
#endif

	if (m_cachingEnabled)
	{
		FontCacheKey key(str, color);

		TextObjectGroup* group = nullptr;
		bool isInCache = false;

		{
			TextObjectCacheMap::iterator it = m_textObjectCache.find(key);
			if (it != m_textObjectCache.end())
			{
				isInCache = true;
				group = &it->second;
			}
		}

		if (!group)
		{
			(void)isInCache;
			assert(!isInCache);

			if (m_recentTextObjectCaches.full())
			{
				const FontCacheKey& oldestKey = m_recentTextObjectCaches.front();
				m_textObjectCache.erase(oldestKey);
				m_recentTextObjectCaches.pop_front();
			}

			group = &m_textObjectCache[key];
			group->hasUnicode = ContainsUnicodeCharacters(str);
			_computeRequireSeperateTextObjectForShadow(group, str);
			m_recentTextObjectCaches.push_back(key);
		}

		TextObject& textObject = group->getOrCreate(*this, str, finalColor, isShadow);

		float fX = x;
		float fY = y;

		group->adjustRenderPosition(fX, fY, isShadow);
		MatrixStack::Ref mtx = MatrixStack::World.push();
		mtx->translate(Vec3(fX, fY, 0.0f));

		textObject.render(material);
	}
	else
	{
		MatrixStack::Ref mtx = MatrixStack::World.push();
		mtx->translate(Vec3(x, y, 0));

		TextObject* textObject = _createTextObject(str, finalColor, isShadow);
		textObject->render(material);
		delete textObject;
	}
}

void Font::_buildCharSimple(uint8_t c, float x, float y)
{
	Tesselator& t = Tesselator::instance;

	float u = float((c % COMMON_MAP_DIMENSION) * ASCII_MAP_GLYPH_SIZE);
	float v = float((c / COMMON_MAP_DIMENSION) * ASCII_MAP_GLYPH_SIZE);
	
	constexpr float D = (1.0f / ASCII_MAP_PIXEL_DIMENSION);

#define CO (ASCII_MAP_GLYPH_SIZE - 0.01f)

	t.vertexUV(x,      y + CO, 0.0f,  u       * D, (v + CO) * D);
	t.vertexUV(x + CO, y + CO, 0.0f, (u + CO) * D, (v + CO) * D);
	t.vertexUV(x + CO, y,      0.0f, (u + CO) * D,  v       * D);
	t.vertexUV(x,      y,      0.0f,  u       * D,  v       * D);

#undef CO
}

void Font::drawSimple(const std::string& str, int x, int y, const Color& color, bool bShadow)
{
	if (str.empty())
		return;

	if (bShadow)
		currentShaderDarkColor = Color(0.25f, 0.25f, 0.25f);
	else
		currentShaderDarkColor = Color::WHITE;

	Color finalColor = color;
	// For hex colors which don't specify an alpha
	if (finalColor.a == 0.0f)
		finalColor.a = 1.0f;

#ifndef FEATURE_GFX_SHADERS
	finalColor *= currentShaderDarkColor;
#endif

	TextureData* textureData = _getAsciiTextureData();
	if (!textureData)
	{
		assert(false);
		return;
	}

	textureData->bind();

	MatrixStack::Ref mtx = MatrixStack::World.push();
	mtx->translate(Vec3(x, y, 0));

	Tesselator& t = Tesselator::instance;
	t.begin(4 * str.size());

	t.color(finalColor);

	float cXPos = 0.0f, cYPos = 0.0f;

	for (size_t i = 0; i < str.size(); i++)
	{
		uint8_t c = static_cast<uint8_t>(str[i]);

		if (c == C_FORMATTING_START_CHARACTER)
		{
			// skip format code too
			i++;
		}
		else if (c == '\n')
		{
			cYPos += C_FONT_RENDER_GLYPH_SIZE + C_NEW_LINE_SPACING;
			cXPos = 0;
		}
		else
		{
			_buildCharSimple(c, cXPos, cYPos);
			cXPos += static_cast<float>(m_charWidth[c]);
		}
	}

	t.draw(m_materials.ui_text);
}

void Font::draw(const std::string& str, int x, int y, const Color& color, bool bShadow)
{
	drawCached(str, x, y, color, bShadow);
}

void Font::draw(const std::string& str, int x, int y, const Color& color)
{
	draw(str, x, y, color, false);
}

void Font::drawShadow(const std::string& str, int x, int y, const Color& color)
{
	//draw(str, x + 1, y + 1, color, true);
	draw(str, x, y, color, true);
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
	//drawScalable(str, x + 1, y + 1, color, scale, true);
	drawScalable(str, x, y, color, scale, true);
	drawScalable(str, x, y, color, scale, false);
}

void Font::drawString(const std::string& str, int x, int y, const Color& color, bool hasShadow)
{
	if (hasShadow)
	{
		drawShadow(str, x, y, color);
	}
	else
	{
		draw(str, x, y, color);
	}
}

void Font::drawOutlinedString(const std::string& str, int x, int y, const Color& color, const Color& outlineColor, float scale, int thickness)
{
	int translations[] = {0, thickness, -thickness};
	for (size_t translationX = 0; translationX < ARRAY_SIZE_OF(translations); ++translationX)
	{
		int xOffset = translations[translationX];
		for (size_t translationY = 0; translationY < ARRAY_SIZE_OF(translations); ++translationY)
		{
			int yOffset = translations[translationY];
			if (xOffset != 0 || yOffset != 0)
			{
				MatrixStack::Ref matrix = MatrixStack::World.push();
				matrix->translate(Vec3(xOffset, yOffset, 0));
				drawScalable(str, x, y, outlineColor, scale, false);
			}
		}
	}

	drawScalable(str, x, y, color, scale, false);
}

void Font::drawWordWrap(const std::string& str, int x, int y, const Color& color, int width, int lineHeight, bool shadow)
{
	drawWordWrap(split(str, width), x, y, color, lineHeight, shadow);
}

void Font::drawWordWrap(const std::vector<std::string>& lines, int x, int y, const Color& color, int lineHeight, bool shadow)
{
	for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
		drawString(*it, x, y, color, shadow);
		y += lineHeight;
	}
}

void Font::drawSimple(const std::string& str, int x, int y, const Color& color)
{
	drawSimple(str, x, y, color, false);
}

void Font::drawSimpleShadow(const std::string& str, int x, int y, const Color& color)
{
	drawSimple(str, x + 1, y + 1, color, true);
	drawSimple(str, x, y, color, false);
}

void Font::drawSimpleScalable(const std::string& str, int x, int y, const Color& color, float scale, bool shadow)
{
	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(Vec3(x, y, 0));
	matrix->scale(scale);
	drawSimple(str, 0, 0, color, shadow);
}

void Font::drawSimpleScalableShadow(const std::string& str, int x, int y, const Color& color, float scale)
{
	drawSimpleScalable(str, x + 1, y + 1, color, scale, true);
	drawSimpleScalable(str, x, y, color, scale);
}

void Font::onGraphicsReset()
{
	_init(m_options);
}

void Font::clearTextObjectCache()
{
	m_textObjectCache.clear();
	m_recentTextObjectCaches.clear();
}

bool Font::ContainsUnicodeCharacters(const std::string& str)
{
	const uint8_t* data = reinterpret_cast<const uint8_t*>(str.c_str());
	utf8proc_ssize_t len = str.size();

	utf8proc_ssize_t charLen;
	int c;
	while ((charLen = utf8proc_iterate(data, len, &c)) > 0)
	{
		if (!_IsAsciiCharacter(c))
			return true;

		data += charLen;
		len -= charLen;
	}

	return false;
}

int Font::height(const std::string& str) const
{
	int height = 0;

	int newLines = static_cast<int>(_CountNewLines(str));
	height += (newLines + 1) * static_cast<int>(C_FONT_RENDER_GLYPH_SIZE);
	height += newLines * static_cast<int>(C_NEW_LINE_SPACING);

	return height;
}

int Font::width(const std::string& str) const
{
	int largestWidth = 0;
	int currWidth = 0;

	const uint8_t* data = reinterpret_cast<const uint8_t*>(str.c_str());
	utf8proc_ssize_t len = str.size();

	utf8proc_ssize_t charLen;
	int c;
	while ((charLen = utf8proc_iterate(data, len, &c)) > 0)
	{
		assert(c >= 0);

		data += charLen;
		len -= charLen;

		if (c >= NUM_GLYPHS)
			c = C_UNK_CHAR;

		if (c == C_FORMATTING_START_CHARACTER)
		{
			if (len > 0)
			{
				// skip the format code
				data++;
				len--;
			}
		}
		else if (c == '\n')
		{
			if (currWidth > largestWidth)
				largestWidth = currWidth;

			currWidth = 0;
		}
		else
		{
			currWidth += m_charWidth[c];
		}
	}

	// assert no utf8 errors
	assert(charLen >= 0);

	if (currWidth > largestWidth)
		largestWidth = currWidth;

	return largestWidth;
}

int Font::heightSimple(const std::string& str) const
{
	int height = 0;

	int newLines = static_cast<int>(std::count(str.begin(), str.end(), '\n'));
	height += (newLines + 1) * static_cast<int>(C_FONT_RENDER_GLYPH_SIZE);
	height += newLines * static_cast<int>(C_NEW_LINE_SPACING);

	return height;
}

int Font::widthSimple(const std::string& str) const
{
	int maxLineWidth = 0;
	int currentLineWidth = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		uint8_t c = static_cast<uint8_t>(str[i]);

		if (c == C_FORMATTING_START_CHARACTER)
		{
			// skip format code
			i++;
		}
		else if (c == '\n')
		{
			if (maxLineWidth < currentLineWidth)
				maxLineWidth = currentLineWidth;

			currentLineWidth = 0;
		}
		else
		{
			currentLineWidth += m_charWidth[c];
		}
	}

	if (maxLineWidth < currentLineWidth)
		maxLineWidth = currentLineWidth;

	return maxLineWidth;
}

std::vector<std::string> Font::split(const std::string& text, int maxWidth)
{
	assert(maxWidth >= 8 + C_SPACING_BETWEEN_CHARS); // max character width is 8, +2 for spacing

	std::vector<std::string> result;

	const uint8_t* from = reinterpret_cast<const uint8_t*>(text.data());
	utf8proc_ssize_t lineCopyCount = 0;
	int lineWidth = 0;

	utf8proc_ssize_t nextWordCopyCount = 0;
	int nextWordWidth = 0;

	int numOfConsecutiveSpaces = 0;

	const uint8_t* data = reinterpret_cast<const uint8_t*>(text.c_str());
	utf8proc_ssize_t len = text.size();

	utf8proc_ssize_t charLen;
	int c;
	while ((charLen = utf8proc_iterate(data, len, &c)) > 0)
	{
		assert(c >= 0);

		int cWidth;

		bool popLine = false;
		bool skipCharacter = false;
		bool addToWordCount = false;

		if (c == '\n')
		{
			popLine = true;
			skipCharacter = true;

			// copy the next word we have with the line
			lineCopyCount += nextWordCopyCount;

			nextWordCopyCount = 0;
			nextWordWidth = 0;
			numOfConsecutiveSpaces = 0;
		}
		else if (c == ' ' && (lineCopyCount == 0 && nextWordCopyCount == 0))
		{
			// trim from start
			skipCharacter = true;
			from += 1;
		}
		else
		{
			cWidth = m_charWidth[c];
			if (c == ' ')
			{
				// add next word to line
				lineCopyCount += nextWordCopyCount;
				lineWidth += nextWordWidth;

				nextWordCopyCount = 0;
				nextWordWidth = 0;
				numOfConsecutiveSpaces = 0;
			}
			else
			{
				addToWordCount = true;
			}

			if (lineWidth + nextWordWidth + cWidth > maxWidth)
			{
				// character extends the current line's width past max width
				popLine = true;

				if (lineCopyCount == 0)
				{
					assert(nextWordCopyCount != 0);

					// we dont have any space for the next word on a completely blank line
					// so we need to split it into multiple chunks
					lineCopyCount = nextWordCopyCount;
					lineWidth = nextWordWidth;

					nextWordCopyCount = 0;
					nextWordWidth = 0;
					numOfConsecutiveSpaces = 0;
				}

				// skip spaces so they aren't copied after the pop
				if (c == ' ')
					skipCharacter = true;
			}
		}

		if (skipCharacter)
		{
			// ignore this character right now
			// so we dont start from this character after the pop
			data += charLen;
			len -= charLen;
		}

		if (popLine)
		{
			lineCopyCount -= numOfConsecutiveSpaces;
			if (lineCopyCount > 0)
				result.push_back(std::string(reinterpret_cast<const char*>(from), lineCopyCount));

			lineCopyCount = 0;
			lineWidth = 0;
			numOfConsecutiveSpaces = 0;
			from = data - nextWordCopyCount;
		}

		if (!skipCharacter)
		{
			if (addToWordCount)
			{
				nextWordCopyCount += charLen;
				nextWordWidth += cWidth;
			}
			else
			{
				lineCopyCount += charLen;
				lineWidth += cWidth;

				if (c == ' ')
					numOfConsecutiveSpaces++;
				else
					numOfConsecutiveSpaces = 0;
			}

			data += charLen;
			len -= charLen;
		}
	}

	lineCopyCount += nextWordCopyCount;

	// only trim if we don't have a next word
	// since if we do, now we won't be ending with a space
	if (nextWordCopyCount == 0)
		lineCopyCount -= numOfConsecutiveSpaces;

	if (lineCopyCount > 0)
		result.push_back(std::string(reinterpret_cast<const char*>(from), lineCopyCount));

	return result;
}

bool Font::ContainsAsciiCharacters(const std::string& str)
{
	const uint8_t* data = reinterpret_cast<const uint8_t*>(str.c_str());
	utf8proc_ssize_t len = str.size();

	utf8proc_ssize_t charLen;
	int c;
	while ((charLen = utf8proc_iterate(data, len, &c)) > 0)
	{
		if (_IsAsciiCharacter(c))
			return true;

		data += charLen;
		len -= charLen;
	}

	return false;
}

bool Font::_IsAsciiCharacter(int c)
{
	assert(c >= 0);
	return c < NUM_ASCII_CHARS;
}

int Font::_GetGlyphMapId(int c)
{
	assert(c < NUM_GLYPHS);
	return c / COMMON_MAP_TOTAL;
}

bool Font::_IsColorFormatCode(uint8_t c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

const Color& Font::_GetColorFromColorFormatCode(uint8_t c)
{
	assert(_IsColorFormatCode(c));

	size_t index = c >= 'a' ? c - 'a' + 10 : c - '0';
	assert(index >= 0 && index < ARRAY_SIZE_OF(COLOR_FROM_CODES));

	return COLOR_FROM_CODES[index];
}

int Font::_CountNewLines(const std::string& str)
{
	int count = 0;

	const uint8_t* data = reinterpret_cast<const uint8_t*>(str.c_str());
	utf8proc_ssize_t len = str.size();

	utf8proc_ssize_t charLen;
	int c;
	while ((charLen = utf8proc_iterate(data, len, &c)) > 0)
	{
		assert(c >= 0);

		data += charLen;
		len -= charLen;

		if (c == '\n')
			count++;
	}

	return count;
}
