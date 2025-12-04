#include "ItemSpriteRenderer.hpp"
#include "EntityRenderDispatcher.hpp"

ItemSpriteRenderer::ItemSpriteRenderer(int sprite)
{
	m_sprite = sprite;
}

void ItemSpriteRenderer::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
	MatrixStack::Ref matrix = MatrixStack::World.push();

	matrix->translate(pos);

	//glEnable(GL_RESCALE_NORMAL);

	matrix->scale(0.5f);

	bindTexture(C_ITEMS_NAME);
	
	/*float texU_1 = float(16 * (m_sprite % 16))     / 256.0f;
	float texU_2 = float(16 * (m_sprite % 16 + 1)) / 256.0f;
	float texV_1 = float(16 * (m_sprite / 16))     / 256.0f;
	float texV_2 = float(16 * (m_sprite / 16 + 1)) / 256.0f;*/

	matrix->rotate(180.0f - m_pDispatcher->m_rot.x, Vec3::UNIT_Y);
	matrix->rotate(-m_pDispatcher->m_rot.y, Vec3::UNIT_X);

	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.color(Color::WHITE);
	t.normal(Vec3::UNIT_Y);
	t.vertexUV(-0.5f, -0.25f, 0.0f, float(16 * (m_sprite % 16))     / 256.0f, float(16 * (m_sprite / 16 + 1)) / 256.0f);
	t.vertexUV(+0.5f, -0.25f, 0.0f, float(16 * (m_sprite % 16 + 1)) / 256.0f, float(16 * (m_sprite / 16 + 1)) / 256.0f);
	t.vertexUV(+0.5f, +0.75f, 0.0f, float(16 * (m_sprite % 16 + 1)) / 256.0f, float(16 * (m_sprite / 16))     / 256.0f);
	t.vertexUV(-0.5f, +0.75f, 0.0f, float(16 * (m_sprite % 16))     / 256.0f, float(16 * (m_sprite / 16))     / 256.0f);
	t.draw(m_shaderMaterials.entity_alphatest);

	//glDisable(GL_RESCALE_NORMAL);
}
