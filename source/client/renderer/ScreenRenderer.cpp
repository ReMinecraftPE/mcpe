#include "ScreenRenderer.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "client/renderer/renderer/Tesselator.hpp"
#include "client/resources/Resource.hpp"

#ifdef _MSC_VER
#pragma warning (disable : 4244)
#endif

std::string ScreenRenderer::PANEL_SLICES[] = { "gui/console/Graphics/Panel_TL.png", "gui/console/Graphics/Panel_TM.png", "gui/console/Graphics/Panel_TR.png",
                                               "gui/console/Graphics/Panel_ML.png", "gui/console/Graphics/Panel_MM.png", "gui/console/Graphics/Panel_MR.png",
                                               "gui/console/Graphics/Panel_BL.png", "gui/console/Graphics/Panel_BM.png", "gui/console/Graphics/Panel_BR.png" };

std::string ScreenRenderer::SMALL_PANEL_SLICES[] = { "gui/console/Panel_Top_L.png", "gui/console/Panel_Top_M.png", "gui/console/Panel_Top_R.png",
                                                     "gui/console/Panel_Mid_L.png", "gui/console/Panel_Mid_M.png", "gui/console/Panel_Mid_R.png",
                                                     "gui/console/Panel_Bot_L.png", "gui/console/Panel_Bot_M.png", "gui/console/Panel_Bot_R.png" };

std::string ScreenRenderer::PANEL_RECESS_SLICES[] = { "gui/console/Graphics/Panel_Recess_Top_L.png", "gui/console/Graphics/Panel_Recess_Top_M.png", "gui/console/Graphics/Panel_Recess_Top_R.png",
                                                      "gui/console/Graphics/Panel_Recess_Mid_L.png", "gui/console/Graphics/Panel_Recess_Mid_M.png", "gui/console/Graphics/Panel_Recess_Mid_R.png",
                                                      "gui/console/Graphics/Panel_Recess_Bot_L.png", "gui/console/Graphics/Panel_Recess_Bot_M.png", "gui/console/Graphics/Panel_Recess_Bot_R.png" };

std::string ScreenRenderer::POINTER_TEXT_PANEL_SLICES[] = { "gui/console/Graphics/PointerTextPanel_TL.png", "gui/console/Graphics/PointerTextPanel_TM.png", "gui/console/Graphics/PointerTextPanel_TR.png",
                                                            "gui/console/Graphics/PointerTextPanel_ML.png", "gui/console/Graphics/PointerTextPanel_MM.png", "gui/console/Graphics/PointerTextPanel_MR.png",
                                                            "gui/console/Graphics/PointerTextPanel_BL.png", "gui/console/Graphics/PointerTextPanel_BM.png", "gui/console/Graphics/PointerTextPanel_BR.png" };

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

void ScreenRenderer::blitSprite(Textures& textures, const std::string& texture, int x, int y, int width, int height, mce::MaterialPtr* materialPtr, float u, float v, int uvWidth, int uvHeight)
{
    blitSprite(textures, textures.getGuiSprite(texture), x, y, width, height, materialPtr, u, v, uvWidth, uvHeight);
}

void ScreenRenderer::blitSprite(Textures& textures, const TextureAtlasSprite* sprite, int x, int y, int width, int height, mce::MaterialPtr* materialPtr, float u, float v, int uvWidth, int uvHeight)
{
    if (!sprite || !sprite->m_pAtlas) return;

    TextureAtlas& atlas = *sprite->m_pAtlas;


    float u0 = u ? sprite->minU + u / atlas.getWidth() : sprite->minU;
    float u1 = u || uvWidth ? sprite->minU + (u + uvWidth) / atlas.getWidth() : sprite->maxU;
    float v0 = v ? sprite->minV + v / atlas.getHeight() : sprite->minV;
    float v1 = v || uvHeight ? sprite->minV + (v + uvHeight) / atlas.getHeight() : sprite->maxV;

    textures.loadAndBindTexture(sprite->m_pAtlas->m_name);
    Tesselator& t = Tesselator::instance;
    t.begin(4);
    t.vertexUV(x, y + height, m_blitOffset, u0, v1);
    t.vertexUV(x + width, y + height, m_blitOffset, u1, v1);
    t.vertexUV(x + width, y, m_blitOffset, u1, v0);
    t.vertexUV(x, y, m_blitOffset, u0, v0);
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

void ScreenRenderer::blitNineSlice(Textures& textures, const std::string* slices, int x, int y, int width, int height, int border, mce::MaterialPtr* materialPtr)
{
    blitSprite(textures, slices[0], x, y, border, border, materialPtr, 0, 0, border, border);
    blitSprite(textures, slices[1], x + border, y, width - 2 * border, border, materialPtr, 0, 0, border, border);
    blitSprite(textures, slices[2], x + width - border, y, border, border, materialPtr, 0, 0, border, border);
    blitSprite(textures, slices[3], x, y + border, border, height - 2 * border, materialPtr, 0, 0, border, border);
    blitSprite(textures, slices[4], x + border, y + border, width - 2 * border, height - 2 * border, materialPtr, 0, 0, border, border);
    blitSprite(textures, slices[5], x + width - border, y + border, border, height - 2 * border, materialPtr, 0, 0, border, border);
    blitSprite(textures, slices[6], x, y + height - border, border, border, materialPtr, 0, 0, border, border);
    blitSprite(textures, slices[7], x + border, y + height - border, width - 2 * border, border, materialPtr, 0, 0, border, border);
    blitSprite(textures, slices[8], x + width - border, y + height - border, border, border, materialPtr, 0, 0, border, border);
}

void ScreenRenderer::blitNineSlice(Textures& textures, const std::string& spriteName, int x, int y, int width, int height, int border, mce::MaterialPtr* materialPtr)
{
    const TextureAtlasSprite* sprite = textures.getGuiSprite(spriteName);

    if (!sprite || !sprite->m_pAtlas) return;

    int iw = sprite->width;
    int ih = sprite->height;
    if (iw == width && ih == height)
    {
        blitSprite(textures, spriteName, x, y, width, height, materialPtr);
        return;
    }

    if (iw == width)
    {
        blitSprite(textures, sprite, x, y, width, border, materialPtr);
        blitSprite(textures, sprite, x, y + border, width, height - border * 2, materialPtr, 0, border, width, border);
        blitSprite(textures, sprite, x, y + height - border, width, border, materialPtr, 0, ih - border);
        return;
    }

    if (iw == height)
    {
        blitSprite(textures, sprite, x, y, border, height, materialPtr);
        blitSprite(textures, sprite, x + border, y, width - 2 * border, height, materialPtr, border, 0, border, height);
        blitSprite(textures, sprite, x + width - border, y, border, height, materialPtr, iw - border);
        return;
    }

    blitSprite(textures, sprite, x, y, border, border, materialPtr, 0, 0, border, border);
    blitSprite(textures, sprite, x + border, y, width - 2 * border, border, materialPtr, border, 0, border, border);
    blitSprite(textures, sprite, x + width - border, y, border, border, materialPtr, iw - border, 0, border, border);
    blitSprite(textures, sprite, x, y + border, border, height - 2 * border, materialPtr, 0, border, border, border);
    blitSprite(textures, sprite, x + border, y + border, width - 2 * border, height - 2 * border, materialPtr, border, border, border, border);
    blitSprite(textures, sprite, x + width - border, y + border, border, height - 2 * border, materialPtr, iw - border, border, border, border);
    blitSprite(textures, sprite, x, y + height - border, border, border, materialPtr, 0, ih - border, border, border);
    blitSprite(textures, sprite, x + border, y + height - border, width - 2 * border, border, materialPtr, border, ih - border, border, border);
    blitSprite(textures, sprite, x + width - border, y + height - border, border, border, materialPtr, iw - border, ih - border, border, border);
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
