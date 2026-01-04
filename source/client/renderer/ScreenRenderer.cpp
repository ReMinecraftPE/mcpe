#include "ScreenRenderer.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "client/renderer/Tesselator.hpp"

#ifdef _MSVC_VER
#pragma warning (disable : 4244)
#endif

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

void ScreenRenderer::blit(const IntRectangle& rect)
{
    Tesselator& t = Tesselator::instance;
    t.begin(4);
    t.vertexUV(rect.x,          rect.h + rect.y, 0.0f, 0.0f, 1.0f);
    t.vertexUV(rect.w + rect.x, rect.h + rect.y, 0.0f, 1.0f, 1.0f);
    t.vertexUV(rect.w + rect.x, rect.y,          0.0f, 1.0f, 0.0f);
    t.vertexUV(rect.x,          rect.y,          0.0f, 0.0f, 0.0f);
    t.draw(m_materials.ui_textured);
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

void ScreenRenderer::drawCenteredString(Font& font, const std::string& str, int cx, int cy, const Color& color)
{
    int width = font.width(str);
    int height = font.height(str);
    font.drawShadow(str, cx - width / 2, cy - height / 2, color);
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
