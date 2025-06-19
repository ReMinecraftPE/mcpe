#include "SkeletonModel.hpp"

SkeletonModel::SkeletonModel() : ZombieModel()
{
    m_arm1.clear();
    m_arm2.clear();
    m_leg1.clear();
    m_leg2.clear();

    m_arm1.addBox(-1.0f, -2.0f, -1.0f, 2, 12, 2, 0.0f);
    m_arm1.setPos(-5.0f, 2.0f, 0.0f);

    m_arm2.m_bMirror = true;
    m_arm2.addBox(-1.0f, -2.0f, -1.0f, 2, 12, 2, 0.0f);
    m_arm2.setPos(5.0f, 2.0f, 0.0f);

    m_leg1.addBox(-1.0f, 0.0f, -1.0f, 2, 12, 2, 0.0f);
    m_leg1.setPos(-2.0f, 12.0f, 0.0f);

    m_leg2.m_bMirror = true;
    m_leg2.addBox(-1.0f, 0.0f, -1.0f, 2, 12, 2, 0.0f);
    m_leg2.setPos(2.0f, 12.0f, 0.0f);
}

SkeletonModel::~SkeletonModel()
{
}