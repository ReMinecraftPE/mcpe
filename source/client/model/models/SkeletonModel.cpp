#include "SkeletonModel.hpp"

SkeletonModel::SkeletonModel() : ZombieModel()
{
    m_pMaterial = &Model::m_materials.entity_alphatest;

    m_armRight.clear();
    m_armLeft.clear();
    m_legRight.clear();
    m_legRight.clear();

    m_armRight.addBox(-1.0f, -2.0f, -1.0f, 2, 12, 2, 0.0f);
    m_armRight.setPos(-5.0f, 2.0f, 0.0f);

    m_armLeft.m_bMirror = true;
    m_armLeft.addBox(-1.0f, -2.0f, -1.0f, 2, 12, 2, 0.0f);
    m_armLeft.setPos(5.0f, 2.0f, 0.0f);

    m_legRight.addBox(-1.0f, 0.0f, -1.0f, 2, 12, 2, 0.0f);
    m_legRight.setPos(-2.0f, 12.0f, 0.0f);

    m_legLeft.m_bMirror = true;
    m_legLeft.addBox(-1.0f, 0.0f, -1.0f, 2, 12, 2, 0.0f);
    m_legLeft.setPos(2.0f, 12.0f, 0.0f);
}

SkeletonModel::~SkeletonModel()
{
}
