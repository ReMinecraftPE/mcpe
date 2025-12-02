#include "ItemSpriteRenderer.hpp"
#include "EntityRenderDispatcher.hpp"

ItemSpriteRenderer::ItemSpriteRenderer(int sprite)
{
	m_sprite = sprite;
}

void ItemSpriteRenderer::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
#ifdef ENH_GFX_MATRIX_STACK
	MatrixStack::Ref matrix = MatrixStack::World.push();
#else
	glPushMatrix();
#endif

#ifdef ENH_GFX_MATRIX_STACK
	matrix->translate(pos);
#else
	glTranslatef(pos.x, pos.y, pos.z);
#endif

	glEnable(GL_RESCALE_NORMAL);

#ifdef ENH_GFX_MATRIX_STACK
	matrix->scale(0.5f);
#else
	glScalef(0.5f, 0.5f, 0.5f);
#endif

	bindTexture(C_ITEMS_NAME);
	
	/*float texU_1 = float(16 * (m_sprite % 16))     / 256.0f;
	float texU_2 = float(16 * (m_sprite % 16 + 1)) / 256.0f;
	float texV_1 = float(16 * (m_sprite / 16))     / 256.0f;
	float texV_2 = float(16 * (m_sprite / 16 + 1)) / 256.0f;*/

#ifdef ENH_GFX_MATRIX_STACK
	matrix->rotate(180.0f - m_pDispatcher->m_rot.x, Vec3::UNIT_Y);
	matrix->rotate(-m_pDispatcher->m_rot.y, Vec3::UNIT_X);
#else
	glRotatef(180.0f - m_pDispatcher->m_rot.x, 0.0f, 1.0f, 0.0f);
	glRotatef(-m_pDispatcher->m_rot.y, 1.0f, 0.0f, 0.0f);
#endif

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(1.0f, 1.0f, 1.0f);
	t.normal(0.0f, 1.0f, 0.0f);
	t.vertexUV(-0.5f, -0.25f, 0.0f, float(16 * (m_sprite % 16))     / 256.0f, float(16 * (m_sprite / 16 + 1)) / 256.0f);
	t.vertexUV(+0.5f, -0.25f, 0.0f, float(16 * (m_sprite % 16 + 1)) / 256.0f, float(16 * (m_sprite / 16 + 1)) / 256.0f);
	t.vertexUV(+0.5f, +0.75f, 0.0f, float(16 * (m_sprite % 16 + 1)) / 256.0f, float(16 * (m_sprite / 16))     / 256.0f);
	t.vertexUV(-0.5f, +0.75f, 0.0f, float(16 * (m_sprite % 16))     / 256.0f, float(16 * (m_sprite / 16))     / 256.0f);
	t.draw();

	glDisable(GL_RESCALE_NORMAL);

#ifndef ENH_GFX_MATRIX_STACK
	glPopMatrix();
#endif
}
