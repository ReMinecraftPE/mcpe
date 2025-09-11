#include "SheepModel.hpp"

SheepModel::SheepModel(bool hasFur) :
	QuadrupedModel(12, 0.0f)
{
    if (hasFur)
    {
        // For the fully-clothed sheep that inhabit the TGA textures
        /*m_body.clear();
        m_body.texOffs(28, 40);
        m_body.addBox(-4.0f, -10.0f, -7.0f, 8, 16, 6, 1.75f);
        m_body.setPos(0.0f, 5.0f, 2.0f);
        m_head.clear();
        m_head.texOffs(0, 0);
        m_head.addBox(-3.0f, -4.0f, -6.0f, 6, 6, 8);
        m_head.setPos(0.0f, 6.0f, -8.0f);
        m_head.texOffs(0, 32);
        m_head.addBox(-3.0f, -4.0f, -4.0f, 6, 6, 6, 0.6f);
        m_leg1.texOffs(0, 48);
        m_leg1.addBox(-2.0f, 0.0f, -2.0f, 4, 6, 4, 0.5f);
        m_leg2.texOffs(0, 48);
        m_leg2.addBox(-2.0f, 0.0f, -2.0f, 4, 6, 4, 0.5f);
        m_leg3.texOffs(0, 48);
        m_leg3.addBox(-2.0f, 0.0f, -2.0f, 4, 6, 4, 0.5f);
        m_leg4.texOffs(0, 48);
        m_leg4.addBox(-2.0f, 0.0f, -2.0f, 4, 6, 4, 0.5f);*/

        m_body.clear();
        m_body.texOffs(28, 8);
        m_body.addBox(-4.0f, -10.0f, -7.0f, 8, 16, 6, 1.75f);
        m_body.setPos(0.0f, 5.0f, 2.0f);
        m_head.clear();
        m_head.texOffs(0, 0);
        m_head.addBox(-3.0f, -4.0f, -4.0f, 6, 6, 6, 0.6f);
        m_head.setPos(0.0f, 6.0f, -8.0f);

        float g = 0.5f;
        m_leg1.clear();
        m_leg1.texOffs(0, 16);
        m_leg1.addBox(-2.0f, 0.0f, -2.0f, 4, 6, 4, g);
        m_leg1.setPos(-3.0f, 12.0f, 7.0f);
        m_leg2.clear();
        m_leg2.texOffs(0, 16);
        m_leg2.addBox(-2.0f, 0.0f, -2.0f, 4, 6, 4, g);
        m_leg2.setPos(3.0f, 12.0f, 7.0f);
        m_leg3.clear();
        m_leg3.texOffs(0, 16);
        m_leg3.addBox(-2.0f, 0.0f, -2.0f, 4, 6, 4, g);
        m_leg3.setPos(-3.0f, 12.0f, -5.0f);
        m_leg4.clear();
        m_leg4.texOffs(0, 16);
        m_leg4.addBox(-2.0f, 0.0f, -2.0f, 4, 6, 4, g);
        m_leg4.setPos(3.0f, 12.0f, -5.0f);

    }
    else
    {
        m_body.clear();
        m_body.texOffs(28, 8);
        m_body.addBox(-4.0f, -10.0f, -7.0f, 8, 16, 6);
        m_body.setPos(0.0f, 5.0f, 2.0f);
        m_head.clear();
        m_head.texOffs(0, 0);
        m_head.addBox(-3.0f, -4.0f, -6.0f, 6, 6, 8);
        m_head.setPos(0.0f, 6.0f, -8.0f);
    }
}

SheepModel::~SheepModel()
{
}
