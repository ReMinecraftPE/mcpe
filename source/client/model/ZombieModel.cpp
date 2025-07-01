#include "ZombieModel.hpp"
#include "common/Mth.hpp"

ZombieModel::ZombieModel() :
	HumanoidModel(0.0f, 0.0f)
{
}

ZombieModel::~ZombieModel()
{
}

void ZombieModel::setupAnim(float time, float r, float bob, float y_rot, float x_rot, float scale)
{
	float attackTime = 1.f;
	HumanoidModel::setupAnim(time, r, bob, y_rot, x_rot, scale);
	float attack2 = Mth::sin(attackTime * M_PI);
	float attack = Mth::sin((1.0f - (1.0f - attackTime) * (1.0f - attackTime)) * M_PI);

	m_arm1.m_rot.z = .0f;
	m_arm2.m_rot.z = .0f;
	m_arm1.m_rot.y = -(0.1f - attack2 * 0.6f);
	m_arm2.m_rot.y = 0.1f - attack2 * 0.6f;
	m_arm1.m_rot.x = -1.5707964f;
	m_arm2.m_rot.x = -1.5707964f;

	m_arm1.m_rot.x -= attack2 * 1.2f - attack * 0.4f;
	m_arm2.m_rot.x -= attack2 * 1.2f - attack * 0.4f;

	m_arm1.m_rot.z += Mth::cos(bob * 0.09f) * 0.05f + 0.05f;
	m_arm2.m_rot.z -= Mth::cos(bob * 0.09f) * 0.05f + 0.05f;

	m_arm1.m_rot.x += Mth::sin(bob * 0.067f) * 0.05f;
	m_arm2.m_rot.x -= Mth::sin(bob * 0.067f) * 0.05f;
}