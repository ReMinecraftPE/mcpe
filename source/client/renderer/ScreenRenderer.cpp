#include "ScreenRenderer.hpp"
#include "renderer/RenderMaterialGroup.hpp"
#include "client/renderer/Tesselator.hpp"

using namespace mce;

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
#ifdef FEATURE_SHADERS
    m_matBlit          = MaterialPtr(RenderMaterialGroup::common, "ui_textured");
    m_matFill          = MaterialPtr(RenderMaterialGroup::common, "ui_fill_color");
    m_matFillGradient  = MaterialPtr(RenderMaterialGroup::common, "ui_fill_gradient");
    m_matColorBlit     = MaterialPtr(RenderMaterialGroup::common, "ui_texture_and_color");
    m_matColorBlitBlur = MaterialPtr(RenderMaterialGroup::common, "ui_textured_and_glcolor");
    m_matStencilFill   = MaterialPtr(RenderMaterialGroup::common, "ui_fill_stencil");
#endif
}

void ScreenRenderer::blit(const IntRectangle& rect)
{
    Tesselator& t = Tesselator::instance;
    t.begin(4);
    t.vertexUV(rect.x, rect.h + rect.y, 0.0f, 0.0f, 1.0f);
    t.vertexUV(rect.w + rect.x, rect.h + rect.y, 0.0f, 1.0f, 1.0f);
    t.vertexUV(rect.w + rect.x, rect.y, 0.0f, 1.0f, 0.0f);
    t.vertexUV(rect.x, rect.y, 0.0f, 0.0f, 0.0f);
    t.draw(m_matBlit);
}