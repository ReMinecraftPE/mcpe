#include "ScreenRenderer.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "client/renderer/renderer/Tesselator.hpp"

#ifdef _MSC_VER
#pragma warning (disable : 4244)
#endif

std::string ScreenRenderer::PANEL_SLICES[] = { "media/Graphics/Panel_TL.png", "media/Graphics/Panel_TM.png", "media/Graphics/Panel_TR.png", 
                                               "media/Graphics/Panel_ML.png", "media/Graphics/Panel_MM.png", "media/Graphics/Panel_MR.png", 
                                               "media/Graphics/Panel_BL.png", "media/Graphics/Panel_BM.png", "media/Graphics/Panel_BR.png" };

std::string ScreenRenderer::SMALL_PANEL_SLICES[] = { "media/Panel_Top_L.png", "media/Panel_Top_M.png", "media/Panel_Top_R.png",
                                                     "media/Panel_Mid_L.png", "media/Panel_Mid_M.png", "media/Panel_Mid_R.png",
                                                     "media/Panel_Bot_L.png", "media/Panel_Bot_M.png", "media/Panel_Bot_R.png" };

std::string ScreenRenderer::POINTER_TEXT_PANEL_SLICES[] = { "media/Graphics/PointerTextPanel_TL.png", "media/Graphics/PointerTextPanel_TM.png", "media/Graphics/PointerTextPanel_TR.png",
                                                            "media/Graphics/PointerTextPanel_ML.png", "media/Graphics/PointerTextPanel_MM.png", "media/Graphics/PointerTextPanel_MR.png",
                                                            "media/Graphics/PointerTextPanel_BL.png", "media/Graphics/PointerTextPanel_BM.png", "media/Graphics/PointerTextPanel_BR.png" };

ScreenRenderer::Materials::Materials()
{
    MATERIAL_PTR(common, ui_textured);
    MATERIAL_PTR(common, ui_fill_color);
    MATERIAL_PTR(common, ui_fill_gradient);
    MATERIAL_PTR(common, ui_texture_and_color);
    MATERIAL_PTR(common, ui_texture_and_color_nocull);
    MATERIAL_PTR(common, ui_textured_and_glcolor);
    MATERIAL_PTR(common, ui_fill_stencil);
}

ScreenRenderer* ScreenRenderer::singletonPtr = nullptr;

ScreenRenderer& ScreenRenderer::singleton()
{
    if (!ScreenRenderer::singletonPtr)
    {
        ScreenRenderer::singletonPtr = new ScreenRenderer();
    }

    return *singletonPtr;
}

ScreenRenderer::ScreenRenderer()
{
    m_blitOffset = 0.0f;
}

void ScreenRenderer::_blitBegin(const IntRectangle& rect)
{
    Tesselator& t = Tesselator::instance;
    t.begin(4);
    t.vertexUV(rect.x,          rect.h + rect.y, 0.0f, 0.0f, 1.0f);
    t.vertexUV(rect.w + rect.x, rect.h + rect.y, 0.0f, 1.0f, 1.0f);
    t.vertexUV(rect.w + rect.x, rect.y,          0.0f, 1.0f, 0.0f);
    t.vertexUV(rect.x,          rect.y,          0.0f, 0.0f, 0.0f);
}

void ScreenRenderer::blit(const IntRectangle& rect)
{
    _blitBegin(rect);
    Tesselator& t = Tesselator::instance;
    t.draw(m_materials.ui_textured);
}

void ScreenRenderer::blit(mce::Mesh& mesh, const IntRectangle& rect)
{
    _blitBegin(rect);
    Tesselator& t = Tesselator::instance;
    mesh = t.end();
}

void ScreenRenderer::blit(int dx, int dy, int sx, int sy, int tw, int th, int sw, int sh, mce::MaterialPtr* materialPtr)
{
    Tesselator& t = Tesselator::instance;

    if (!sh) sh = th;
    if (!sw) sw = tw;

    t.begin(4);
    t.vertexUV(dx + 0 , dy + th, m_blitOffset, float(sx + 0 ) / 256.0f, float(sy + sh) / 256.0f);
    t.vertexUV(dx + tw, dy + th, m_blitOffset, float(sx + sw) / 256.0f, float(sy + sh) / 256.0f);
    t.vertexUV(dx + tw, dy + 0 , m_blitOffset, float(sx + sw) / 256.0f, float(sy + 0 ) / 256.0f);
    t.vertexUV(dx + 0 , dy + 0 , m_blitOffset, float(sx + 0 ) / 256.0f, float(sy + 0 ) / 256.0f);
    t.draw(materialPtr ? *materialPtr : m_materials.ui_textured);
}

void ScreenRenderer::blitTexture(Textures& textures, const std::string& texture, int x, int y, float u, float v, int width, int height, int uvWidth, int uvHeight, int textureWidth, int textureHeight, mce::MaterialPtr* materialPtr)
{
    textures.loadAndBindTexture(texture);
    Tesselator& t = Tesselator::instance;
    t.begin(4);
    t.vertexUV(x, y + height, m_blitOffset, u / textureWidth, (v + uvHeight) / textureHeight);
    t.vertexUV(x + width, y + height, m_blitOffset, (u + uvWidth) / textureWidth, (v + uvHeight) / textureHeight);
    t.vertexUV(x + width, y, m_blitOffset, (u + uvWidth) / textureWidth, v / textureHeight);
    t.vertexUV(x, y, m_blitOffset, u / textureWidth, v / textureHeight);
    t.draw(materialPtr ? *materialPtr : m_materials.ui_textured);
}

void ScreenRenderer::blitTexture(Textures& textures, const std::string& texture, int x, int y, float u, float v, int width, int height, int textureWidth, int textureHeight, mce::MaterialPtr* materialPtr)
{
    blitTexture(textures, texture, x, y, u, v, width, height, width, height, textureWidth, textureHeight, materialPtr);
}

void ScreenRenderer::blitTexture(Textures& textures, const std::string& texture, int x, int y, float u, float v, int width, int height, mce::MaterialPtr* materialPtr)
{
    blitTexture(textures, texture, x, y, u, v, width, height, width, height, width, height, materialPtr);
}

void ScreenRenderer::blitRaw(float x1, float x2, float y1, float y2, float z, float u1, float u2, float v1, float v2)
{
    Tesselator& t = Tesselator::instance;

    t.begin(4);
    t.vertexUV(x1, y1, z, u2, v2);
    t.vertexUV(x2, y1, z, u1, v2);
    t.vertexUV(x2, y2, z, u1, v1);
    t.vertexUV(x1, y2, z, u2, v1);
    t.draw(m_materials.ui_textured_and_glcolor);
}

void ScreenRenderer::blitNineSlice(Textures& textures, int x, int y, int width, int height, int tileSize, const std::string* slices, mce::MaterialPtr* materialPtr)
{
    blitTexture(textures, slices[0], x, y, 0, 0, tileSize, tileSize, tileSize, tileSize, materialPtr);
    blitTexture(textures, slices[1], x + tileSize, y, 0, 0, width - 2 * tileSize, tileSize, tileSize, tileSize, materialPtr);
    blitTexture(textures, slices[2], x + width - tileSize, y, 0, 0, tileSize, tileSize, tileSize, tileSize, materialPtr);
    blitTexture(textures, slices[3], x, y + tileSize, 0, 0, tileSize, height - 2 * tileSize, tileSize, tileSize, materialPtr);
    blitTexture(textures, slices[4], x + tileSize, y + tileSize, 0, 0, width - 2 * tileSize, height - 2 * tileSize, tileSize, tileSize, materialPtr);
    blitTexture(textures, slices[5], x + width - tileSize, y + tileSize, 0, 0, tileSize, height - 2 * tileSize, tileSize, tileSize, materialPtr);
    blitTexture(textures, slices[6], x, y + height - tileSize, 0, 0, tileSize, tileSize, tileSize, tileSize, materialPtr);
    blitTexture(textures, slices[7], x + tileSize, y + height - tileSize, 0, 0, width - 2 * tileSize, tileSize, tileSize, tileSize, materialPtr);
    blitTexture(textures, slices[8], x + width - tileSize, y + height - tileSize, 0, 0, tileSize, tileSize, tileSize, tileSize, materialPtr);
}

void ScreenRenderer::blitNineSlice(Textures& textures, int x, int y, int width, int height, int border, const std::string& texture, mce::MaterialPtr* materialPtr)
{
    ImageData& data = textures.getTextureData(texture, true)->m_imageData;
    int iw = data.m_width;
    int ih = data.m_height;
    if (iw == width && ih == height)
    {
        blitTexture(textures, texture, x, y, 0, 0, width, height, materialPtr);
        return;
    }

    if (iw == width)
    {
        blitTexture(textures, texture, x, y, 0, 0, width, border, iw, ih, materialPtr);
        blitTexture(textures, texture, x, y + border, 0, border, width, height - border * 2, width, border, iw, ih, materialPtr);
        blitTexture(textures, texture, x, y + height - border, 0, ih - border, width, border, iw, ih, materialPtr);
        return;
    }

    if (iw == height)
    {
        blitTexture(textures, texture, x, y, 0, 0, border, height, iw, ih, materialPtr);
        blitTexture(textures, texture, x + border, y, border, 0, width - 2 * border, height, border, height, iw, ih, materialPtr);
        blitTexture(textures, texture, x + width - border, y, iw - border, 0, border, height, iw, ih, materialPtr);
        return;
    }

    blitTexture(textures, texture, x, y, 0, 0, border, border, iw, ih, materialPtr);
    blitTexture(textures, texture, x + border, y, border, 0, width - 2 * border, border, border, border, iw, ih, materialPtr);
    blitTexture(textures, texture, x + width - border, y, iw - border, 0, border, border, iw, ih, materialPtr);
    blitTexture(textures, texture, x, y + border, 0, border, border, height - 2 * border, border, border, iw, ih, materialPtr);
    blitTexture(textures, texture, x + border, y + border, border, border, width - 2 * border, height - 2 * border, border, border, iw, ih, materialPtr);
    blitTexture(textures, texture, x + width - border, y + border, iw - border, border, border, height - 2 * border, border, border, iw, ih, materialPtr);
    blitTexture(textures, texture, x, y + height - border, 0, ih - border, border, border, iw, ih, materialPtr);
    blitTexture(textures, texture, x + border, y + height - border, border, ih - border, width - 2 * border, border, border, border, iw, ih, materialPtr);
    blitTexture(textures, texture, x + width - border, y + height - border, iw - border, ih - border, border, border, iw, ih, materialPtr);
}

void ScreenRenderer::drawCenteredString(Font& font, const std::string& str, int cx, int cy, const Color& color)
{
    int width = font.width(str);
    font.drawShadow(str, cx - width / 2, cy, color);
}

void ScreenRenderer::drawString(Font& font, const std::string& str, int cx, int cy, const Color& color)
{
    font.drawShadow(str, cx, cy, color);
}

void ScreenRenderer::fill(float left, float top, float right, float bottom, const Color& color)
{
    currentShaderColor = color;

    Tesselator& t = Tesselator::instance;
    t.begin(4);

    t.vertex(left,  bottom, 0.0f);
    t.vertex(right, bottom, 0.0f);
    t.vertex(right, top,    0.0f);
    t.vertex(left,  top,    0.0f);

    t.draw(m_materials.ui_fill_color);
}

void ScreenRenderer::fill(int left, int top, int right, int bottom, const Color& color)
{
    fill(float(left), float(top), float(right), float(bottom), color);
}

void ScreenRenderer::fillGradient(float left, float top, float right, float bottom, const Color& colorUp, const Color& colorDown)
{
    mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
    renderContext.setShadeMode(mce::SHADE_MODE_SMOOTH);

    Tesselator& t = Tesselator::instance;
    t.begin(4);

    t.color(colorUp);
    t.vertex(left, bottom, 0.0f);
    t.vertex(right, bottom, 0.0f);
    t.color(colorDown);
    t.vertex(right, top, 0.0f);
    t.vertex(left, top, 0.0f);

    t.draw(m_materials.ui_fill_gradient);

    renderContext.setShadeMode(mce::SHADE_MODE_FLAT);
}

void ScreenRenderer::fillGradient(int left, int top, int right, int bottom, const Color& colorUp, const Color& colorDown)
{
    fillGradient(float(left), float(top), float(right), float(bottom), colorUp, colorDown);
}
