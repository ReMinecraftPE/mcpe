#include "SpiderModel.hpp"

SpiderModel::SpiderModel() : 
	Model(64, 64),
	m_head(32, 4),
	m_body0(0, 0),
	m_body1(0, 12),
	m_leg0(18, 0),
	m_leg1(18, 0),
	m_leg2(18, 0),
	m_leg3(18, 0),
	m_leg4(18, 0),
	m_leg5(18, 0),
	m_leg6(18, 0),
	m_leg7(18, 0)
{
	static float g = 0.0f;
	static int yo = 15;

	m_head.addBox(-4.0f, -4.0f, -8.0f, 8, 8, 8, g);
    m_head.addBox(-4.0f, -4.0f, -8.0f, 8, 8, 8, g);
    m_head.setPos(0.0f, (float)(0 + yo), -3.0f);

    m_body0.addBox(-3.0f, -3.0f, -3.0f, 6, 6, 6, g);
    m_body0.setPos(0.0f, (float)yo, 0.0f);

    m_body1.addBox(-5.0f, -4.0f, -6.0f, 10, 8, 12, g);
    m_body1.setPos(0.0f, (float)(0 + yo), 9.0f);

    m_leg0.addBox(-15.0f, -1.0f, -1.0f, 16, 2, 2, g);
    m_leg0.setPos(-4.0f, (float)(0 + yo), 2.0f);

    m_leg1.addBox(-1.0f, -1.0f, -1.0f, 16, 2, 2, g);
    m_leg1.setPos(4.0f, (float)(0 + yo), 2.0f);

    m_leg2.addBox(-15.0f, -1.0f, -1.0f, 16, 2, 2, g);
    m_leg2.setPos(-4.0f, (float)(0 + yo), 1.0f);

    m_leg3.addBox(-1.0f, -1.0f, -1.0f, 16, 2, 2, g);
    m_leg3.setPos(4.0f, (float)(0 + yo), 1.0f);

    m_leg4.addBox(-15.0f, -1.0f, -1.0f, 16, 2, 2, g);
    m_leg4.setPos(-4.0f, (float)(0 + yo), 0.0f);

    m_leg5.addBox(-1.0f, -1.0f, -1.0f, 16, 2, 2, g);
    m_leg5.setPos(4.0f, (float)(0 + yo), 0.0f);

    m_leg6.addBox(-15.0f, -1.0f, -1.0f, 16, 2, 2, g);
    m_leg6.setPos(-4.0f, (float)(0 + yo), -1.0f);

    m_leg7.addBox(-1.0f, -1.0f, -1.0f, 16, 2, 2, g);
    m_leg7.setPos(4.0f, (float)(0 + yo), -1.0f);
}

SpiderModel::~SpiderModel()
{
}

void SpiderModel::render(float time, float r, float bob, float yRot, float xRot, float scale) {
    setupAnim(time, r, bob, yRot, xRot, scale);
    m_head.render(scale);

    m_body0.render(scale);
    m_body1.render(scale);

    m_leg0.render(scale);
    m_leg1.render(scale);
    m_leg2.render(scale);
    m_leg3.render(scale);
    m_leg4.render(scale);
    m_leg5.render(scale);
    m_leg6.render(scale);
    m_leg7.render(scale);
}

void SpiderModel::setupAnim(float time, float r, float bob, float yRot, float xRot, float scale) {
    m_head.m_rot.y = yRot / 57.295776f;
    m_head.m_rot.x = xRot / 57.295776f;

    static float sr = 0.7853982f;
    m_leg0.m_rot.z = -sr;
    m_leg1.m_rot.z = sr;
    m_leg2.m_rot.z = -sr * 0.74f;
    m_leg3.m_rot.z = sr * 0.74f;
    m_leg4.m_rot.z = -sr * 0.74f;
    m_leg5.m_rot.z = sr * 0.74f;
    m_leg6.m_rot.z = -sr;
    m_leg7.m_rot.z = sr;

    static float ro = -0.0f;
    static float ur = 0.3926991f;
    m_leg0.m_rot.y = ur * 2.0f + ro;
    m_leg1.m_rot.y = -ur * 2.0f - ro;
    m_leg2.m_rot.y = ur * 1.0f + ro;
    m_leg3.m_rot.y = -ur * 1.0f - ro;
    m_leg4.m_rot.y = -ur * 1.0f + ro;
    m_leg5.m_rot.y = ur * 1.0f - ro;
    m_leg6.m_rot.y = -ur * 2.0f + ro;
    m_leg7.m_rot.y = ur * 2.0f - ro;

    float c0 = -(Mth::cos(time * 0.6662f * 2.0f + 0.0f) * 0.4f) * r;
    float c1 = -(Mth::cos(time * 0.6662f * 2.0f + float(M_PI)) * 0.4f) * r;
    float c2 = -(Mth::cos(time * 0.6662f * 2.0f + 1.5707964f) * 0.4f) * r;
    float c3 = -(Mth::cos(time * 0.6662f * 2.0f + 4.712389f) * 0.4f) * r;
    float s0 = Mth::abs(Mth::sin(time * 0.6662f + 0.0f) * 0.4f) * r;
    float s1 = Mth::abs(Mth::sin(time * 0.6662f + float(M_PI)) * 0.4f) * r;
    float s2 = Mth::abs(Mth::sin(time * 0.6662f + 1.5707964f) * 0.4f) * r;
    float s3 = Mth::abs(Mth::sin(time * 0.6662f + 4.712389f) * 0.4f) * r;
    
    m_leg0.m_rot.y += c0;
    m_leg1.m_rot.y += -c0;
    m_leg2.m_rot.y += c1;
    m_leg3.m_rot.y += -c1;
    m_leg4.m_rot.y += c2;
    m_leg5.m_rot.y += -c2;
    m_leg6.m_rot.y += c3;
    m_leg7.m_rot.y += -c3;
    
    m_leg0.m_rot.z += s0;
    m_leg1.m_rot.z += -s0;
    m_leg2.m_rot.z += s1;
    m_leg3.m_rot.z += -s1;
    m_leg4.m_rot.z += s2;
    m_leg5.m_rot.z += -s2;
    m_leg6.m_rot.z += s3;
    m_leg7.m_rot.z += -s3;
}
