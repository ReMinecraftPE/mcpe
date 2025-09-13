#include "ArrowRenderer.hpp"
#include <world/entity/Arrow.hpp>

ArrowRenderer::ArrowRenderer()
{
}

ArrowRenderer::~ArrowRenderer()
{
}

void ArrowRenderer::render(Entity* ent, const Vec3& pos, float rot, float a)
{
    Arrow* arrow = reinterpret_cast<Arrow*>(ent);
	
    glPushMatrix();
    bindTexture("item/arrows.png");
    
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(arrow->m_oRot.y + (arrow->m_rot.y - arrow->m_oRot.y) * a - 90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(arrow->m_oRot.x + (arrow->m_rot.x - arrow->m_oRot.x) * a, 0.0f, 0.0f, 1.0f);

    Tesselator& t = Tesselator::instance;
    int type = 0;
    float u0 = 0.0f;
    float u1 = 0.5f;
    float v0 = (0 + type * 10) / 32.0f;
    float v1 = (5 + type * 10) / 32.0f;
    float u02 = 0.0f;
    float u12 = 0.15625f;
    float v02 = (5 + type * 10) / 32.0f;
    float v12 = (10 + type * 10) / 32.0f;
    float ss = 0.05625f;
    glEnable(32826);
    float shake = arrow->m_shakeTime - a;
    if (shake > 0.0f) {
        float pow = -Mth::sin(shake * 3.0f) * shake;
        glRotatef(pow, 0.0f, 0.0f, 1.0f);
    }

    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
    glScalef(ss, ss, ss);
    glTranslatef(-4.0f, 0.0f, 0.0f);
    glNormal3f(ss, 0.0f, 0.0f);
    t.begin();
    t.vertexUV(-7.0, -2.0, -2.0, u02, v02);
    t.vertexUV(-7.0, -2.0, 2.0, u12, v02);
    t.vertexUV(-7.0, 2.0, 2.0, u12, v12);
    t.vertexUV(-7.0, 2.0, -2.0, u02, v12);
    t.draw();

    glNormal3f(-ss, 0.0f, 0.0f);
    t.begin();
    t.vertexUV(-7.0, 2.0, -2.0, u02, v02);
    t.vertexUV(-7.0, 2.0, 2.0, u12, v02);
    t.vertexUV(-7.0, -2.0, 2.0, u12, v12);
    t.vertexUV(-7.0, -2.0, -2.0, u02, v12);
    t.draw();

    for (int i = 0; i < 4; ++i) {
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, ss);
        t.begin();
        t.vertexUV(-8.0, -2.0, 0.0, u0, v0);
        t.vertexUV(8.0, -2.0, 0.0, u1, v0);
        t.vertexUV(8.0, 2.0, 0.0, u1, v1);
        t.vertexUV(-8.0, 2.0, 0.0, u0, v1);
        t.draw();
    }

    glDisable(32826);
    glPopMatrix();
}