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

    t.begin();
    t.vertexUV(dx + 0 , dy + th, m_blitOffset, float(sx + 0 ) / 256.0f, float(sy + sh) / 256.0f);
    t.vertexUV(dx + tw, dy + th, m_blitOffset, float(sx + sw) / 256.0f, float(sy + sh) / 256.0f);
    t.vertexUV(dx + tw, dy + 0 , m_blitOffset, float(sx + sw) / 256.0f, float(sy + 0 ) / 256.0f);
    t.vertexUV(dx + 0 , dy + 0 , m_blitOffset, float(sx + 0 ) / 256.0f, float(sy + 0 ) / 256.0f);
    t.draw(materialPtr ? *materialPtr : m_materials.ui_textured);
}

void ScreenRenderer::fill(int left, int top, int right, int bottom, const Color& color)
{
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    currentShaderColor = color;

    Tesselator& t = Tesselator::instance;
    t.begin();

    t.vertex(left,  bottom, 0.0f);
    t.vertex(right, bottom, 0.0f);
    t.vertex(right, top,    0.0f);
    t.vertex(left,  top,    0.0f);

    t.draw(); // m_materials.ui_fill_color

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}