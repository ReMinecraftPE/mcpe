#include "ScreenRenderer.hpp"
#include "renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "client/renderer/Tesselator.hpp"

#ifdef _WIN32
#pragma warning (disable : 4244)
#endif

ScreenRenderer::Materials::Materials()
{
    MATERIAL_PTR(common, ui_textured);
    MATERIAL_PTR(common, ui_fill_color);
    MATERIAL_PTR(common, ui_fill_gradient);
    MATERIAL_PTR(common, ui_texture_and_color);
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
#ifndef FEATURE_GFX_SHADERS
    //glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default
#endif

    currentShaderColor = color;

    Tesselator& t = Tesselator::instance;
    t.begin(4);

    t.vertex(left,  bottom, 0.0f);
    t.vertex(right, bottom, 0.0f);
    t.vertex(right, top,    0.0f);
    t.vertex(left,  top,    0.0f);

    t.draw(m_materials.ui_fill_color);

#ifndef FEATURE_GFX_SHADERS
    glEnable(GL_TEXTURE_2D);
    //glDisable(GL_BLEND);
#endif
}

void ScreenRenderer::fill(int left, int top, int right, int bottom, const Color& color)
{
    fill(float(left), float(top), float(right), float(bottom), color);
}

void ScreenRenderer::fillGradient(float left, float top, float right, float bottom, const Color& colorUp, const Color& colorDown)
{
#ifndef FEATURE_GFX_SHADERS
    glDisable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default
    glShadeModel(GL_SMOOTH);
#endif

    Tesselator& t = Tesselator::instance;
    t.begin(4);

    t.color(colorUp);
    t.vertex(left, bottom, 0.0f);
    t.vertex(right, bottom, 0.0f);
    t.color(colorDown);
    t.vertex(right, top, 0.0f);
    t.vertex(left, top, 0.0f);

    t.draw(m_materials.ui_fill_gradient);

#ifndef FEATURE_GFX_SHADERS
    glShadeModel(GL_FLAT);
    //glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
#endif
}

void ScreenRenderer::fillGradient(int left, int top, int right, int bottom, const Color& colorUp, const Color& colorDown)
{
    fillGradient(float(left), float(top), float(right), float(bottom), colorUp, colorDown);
}