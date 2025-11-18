#include "ScreenRenderer.hpp"
#include "renderer/RenderMaterialGroup.hpp"
#include "client/renderer/Tesselator.hpp"

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
}

void ScreenRenderer::blit(const IntRectangle& rect)
{
    Tesselator& t = Tesselator::instance;
    t.begin(4);
    t.vertexUV(rect.x, rect.h + rect.y, 0.0f, 0.0f, 1.0f);
    t.vertexUV(rect.w + rect.x, rect.h + rect.y, 0.0f, 1.0f, 1.0f);
    t.vertexUV(rect.w + rect.x, rect.y, 0.0f, 1.0f, 0.0f);
    t.vertexUV(rect.x, rect.y, 0.0f, 0.0f, 0.0f);
    t.draw(m_materials.ui_textured);
}