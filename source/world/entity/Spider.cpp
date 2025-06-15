#include "Spider.hpp"

Spider::Spider(Level* pLevel) : Monster(pLevel)
{
	m_pDescriptor = &EntityTypeDescriptor::spider;
	field_C8 = RENDER_SPIDER;

	m_texture = "mob/spider.png";
	setSize(1.4f, 0.9f);
	m_runSpeed = 0.8f;
}