#include "ItemSpriteRenderer.hpp"
#include "EntityRenderDispatcher.hpp"

ItemSpriteRenderer::ItemSpriteRenderer(int sprite)
{
	m_sprite = sprite;
}

void ItemSpriteRenderer::render(Entity* pEntity, float x, float y, float z, float a, float b)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glEnable(GL_RESCALE_NORMAL);
	glScalef(0.5f, 0.5f, 0.5f);
	bindTexture(C_ITEMS_NAME);
	
	float texU_1 = float(16 * (m_sprite % 16))     / 256.0f;
	float texU_2 = float(16 * (m_sprite % 16 + 1)) / 256.0f;
	float texV_1 = float(16 * (m_sprite / 16))     / 256.0f;
	float texV_2 = float(16 * (m_sprite / 16 + 1)) / 256.0f;

	glRotatef(180.0f - m_pDispatcher->m_yaw, 0.0f, 1.0f, 0.0f);
	glRotatef(-m_pDispatcher->m_pitch, 1.0f, 0.0f, 0.0f);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(1.0f, 1.0f, 1.0f);
	t.vertexUV(-0.5f, -0.25f, 0.0f, float(16 * (m_sprite % 16))     / 256.0f, float(16 * (m_sprite / 16 + 1)) / 256.0f);
	t.vertexUV(+0.5f, -0.25f, 0.0f, float(16 * (m_sprite % 16 + 1)) / 256.0f, float(16 * (m_sprite / 16 + 1)) / 256.0f);
	t.vertexUV(+0.5f, +0.75f, 0.0f, float(16 * (m_sprite % 16 + 1)) / 256.0f, float(16 * (m_sprite / 16))     / 256.0f);
	t.vertexUV(-0.5f, +0.75f, 0.0f, float(16 * (m_sprite % 16))     / 256.0f, float(16 * (m_sprite / 16))     / 256.0f);
	t.draw();

	glDisable(GL_RESCALE_NORMAL);
	glPopMatrix();
}
