#include "ArrowRenderer.hpp"
#include "client/renderer/Tesselator.hpp"
#include "renderer/MatrixStack.hpp"
#include "world/entity/Arrow.hpp"

#define ARROW_SCALE 0.05625f

ArrowRenderer::ArrowRenderer()
{
}

ArrowRenderer::~ArrowRenderer()
{
}

void ArrowRenderer::_buildMesh()
{
    Tesselator& t = Tesselator::instance;

    constexpr int type = 0;
    constexpr float u0 = 0.0f;
    constexpr float u1 = 0.5f;
    constexpr float v0 = (0 + type * 10) / 32.0f;
    constexpr float v1 = (5 + type * 10) / 32.0f;
    constexpr float u02 = 0.0f;
    constexpr float u12 = 0.15625f;
    constexpr float v02 = (5 + type * 10) / 32.0f;
    constexpr float v12 = (10 + type * 10) / 32.0f;

    t.begin(12);

    t.normal(Vec3::UNIT_X * ARROW_SCALE);
    t.vertexUV(-7.0f, -2.0f, -2.0f, u02, v02);
    t.vertexUV(-7.0f, -2.0f,  2.0f, u12, v02);
    t.vertexUV(-7.0f,  2.0f,  2.0f, u12, v12);
    t.vertexUV(-7.0f,  2.0f, -2.0f, u02, v12);

    /*t.normal(Vec3::NEG_UNIT_X * ARROW_SCALE);
    t.vertexUV(-7.0f,  2.0f, -2.0f, u02, v02);
    t.vertexUV(-7.0f,  2.0f,  2.0f, u12, v02);
    t.vertexUV(-7.0f, -2.0f,  2.0f, u12, v12);
    t.vertexUV(-7.0f, -2.0f, -2.0f, u02, v12);*/

    t.normal(Vec3::UNIT_Z * ARROW_SCALE);
    t.vertexUV(-8.0f, -2.0f, 0.0f, u0, v0);
    t.vertexUV( 8.0f, -2.0f, 0.0f, u1, v0);
    t.vertexUV( 8.0f,  2.0f, 0.0f, u1, v1);
    t.vertexUV(-8.0f,  2.0f, 0.0f, u0, v1);

    t.normal(Vec3::UNIT_Y * ARROW_SCALE);
    t.vertexUV(-8.0f, 0.0f, -2.0f, u0, v0);
    t.vertexUV( 8.0f, 0.0f, -2.0f, u1, v0);
    t.vertexUV( 8.0f, 0.0f,  2.0f, u1, v1);
    t.vertexUV(-8.0f, 0.0f,  2.0f, u0, v1);

    m_mesh = t.end();
}

void ArrowRenderer::onAppSuspended()
{
    m_mesh.reset();
}

void ArrowRenderer::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
    const Arrow& arrow = reinterpret_cast<const Arrow&>(entity);

    if (!m_mesh.isValid())
        _buildMesh();

    bindTexture("item/arrows.png");

    MatrixStack::Ref matrix = MatrixStack::World.push();
    matrix->translate(pos);
    matrix->rotate(arrow.m_oRot.y + (arrow.m_rot.y - arrow.m_oRot.y) * a - 90.0f, Vec3::UNIT_Y);
    matrix->rotate(arrow.m_oRot.x + (arrow.m_rot.x - arrow.m_oRot.x) * a,         Vec3::UNIT_Z);

    float shake = arrow.m_shakeTime - a;
    if (shake > 0.0f)
    {
        float pow = -Mth::sin(shake * 3.0f) * shake;
        matrix->rotate(pow, Vec3::UNIT_Z);
    }

    matrix->rotate(45.0f, Vec3::UNIT_X);
    matrix->scale(ARROW_SCALE);
    matrix->translate(Vec3(-4.0f, 0.0f, 0.0f));

    //_setupShaderParameters(ent, Color::NIL, a);

    m_mesh.render(m_materials.entity_alphatest);
}
