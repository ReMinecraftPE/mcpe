/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TripodCameraRenderer.hpp"
#include "common/Mth.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/ShaderConstants.hpp"

TripodCameraRenderer::TripodCameraRenderer()
	: m_modelPart(0, 0)
{
	m_modelPart.addBox(-4.0f, -4.0f, -6.0f, 8, 8, 10);
	m_modelPart.m_pos.y = 11.0f;
	m_modelPart.m_pMaterial = &m_shaderMaterials.entity;
	m_shadowRadius = 0.5f;
}

float TripodCameraRenderer::getFlashTime(const TripodCamera& camera, float f)
{
	if (camera.field_B90 > 7 || camera.field_B90 < 0)
		return -1.0f;

	return 0.125f * (float(camera.field_B90) - f);
}

void TripodCameraRenderer::render(const Entity& entity, const Vec3& pos, float rot, float a)
{
	static constexpr float C_RATIO_X = 1.0f / 64.0f, C_RATIO_Y = 1.0f / 32.0f;

	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(pos);

	m_modelPart.m_rot.x  = MTH_DEG_TO_RAD * (180.0f + 0.5f * entity.m_rot.y);
	m_modelPart.m_rot.y = -MTH_DEG_TO_RAD * entity.m_rot.x;

	Tesselator& t = Tesselator::instance;
	//t.color(1.0f, 1.0f, 1.0f);

	float brightness = entity.getBrightness(1.0f);

	bindTexture("item/camera.png");

	// @TODO: Make this use PolygonQuad and stick it in TripodCameraModel

	float texX = 16 * 3;
	float texY = 16 * 1;

	// calculate U and V coordinates
	float texU_l = texX * C_RATIO_X, texU_r = (texX + 15.99f) * C_RATIO_X;
	float texV_u = texY * C_RATIO_Y, texV_d = (texY + 15.99f) * C_RATIO_Y;

	float cenX = 0.0f, cenZ = 0.0f;
	float newY = -0.5f;

	float x1 = cenX - 0.45f, x2 = cenX + 0.45f;
	float z1 = cenZ - 0.45f, z2 = cenZ + 0.45f;

	t.begin(8);

	// Needed for D3D, since the shader expects this
	t.normal(Vec3::ZERO);
	t.vertexUV(x1, newY + 1, z1, texU_l, texV_u);
	t.vertexUV(x1, newY + 0, z1, texU_l, texV_d);
	t.vertexUV(x2, newY + 0, z2, texU_r, texV_d);
	t.vertexUV(x2, newY + 1, z2, texU_r, texV_u);

	t.vertexUV(x1, newY + 1, z2, texU_l, texV_u);
	t.vertexUV(x1, newY + 0, z2, texU_l, texV_d);
	t.vertexUV(x2, newY + 0, z1, texU_r, texV_d);
	t.vertexUV(x2, newY + 1, z1, texU_r, texV_u);

	t.draw(m_shaderMaterials.entity_alphatest);

	m_modelPart.setBrightness(brightness);
	m_modelPart.render(0.0625f);

	Entity* pHREntity = m_pDispatcher->m_pMinecraft->m_hitResult.m_pEnt;

	const TripodCamera& camera = (const TripodCamera&)entity;

	float time = getFlashTime(camera, a);
	if (time >= 0.0f)
	{
		// pulse effect
		currentShaderColor = Color::WHITE;
		currentShaderDarkColor = Color(1.0f, 1.0f, 1.0f, sinf(float(M_PI) * 1.0f * time));
		
		// restore camera flash texture
		MatrixStack::Ref flashMatrix = MatrixStack::World.push();
		
		const float radToDeg = 1.0f / MTH_DEG_TO_RAD;
		
		float yaw = m_modelPart.m_rot.y;
		float pitch = m_modelPart.m_rot.x;
		float forwardX = -sinf(yaw) * cosf(pitch);
		float forwardY = sinf(pitch);
		float forwardZ = -cosf(yaw) * cosf(pitch);
		
		flashMatrix->translate(Vec3(forwardX * 0.4f, forwardY * 0.4f + 0.7f, forwardZ * 0.4f));
		flashMatrix->rotate(yaw * radToDeg, Vec3::UNIT_Y);
		flashMatrix->rotate(pitch * radToDeg, Vec3::UNIT_X);
		flashMatrix->rotate(90.0f, Vec3::UNIT_X);
		
		t.begin(8);
		t.normal(Vec3::UNIT_Y);
		
		static constexpr float U_RATIO = 1.0f / 64.0f;
		static constexpr float V_RATIO = 1.0f / 32.0f;

		// calculate U and V coordinates
		float texU_l = 48.0f * U_RATIO;
		float texU_r = (48.0f + 15.99f) * U_RATIO;
		float texV_u = 0.0f * V_RATIO;
		float texV_d = (0.0f + 15.99f) * V_RATIO;
		float x1 = -0.5f, x2 = 0.5f;
		float z1 = -0.5f, z2 = 0.5f;
		float y  = 0.0f;
		
		t.vertexUV(x1, y, z2, texU_l, texV_u);
		t.vertexUV(x1, y, z1, texU_r, texV_u);
		t.vertexUV(x2, y, z1, texU_r, texV_d);
		t.vertexUV(x2, y, z2, texU_l, texV_d);
		t.draw(m_shaderMaterials.entity_alphatest);
	}

	if (&entity == pHREntity)
	{
		// @TODO: Use EntityShaderManager to set overlay color, except this will only work for shaders
		// _setupShaderParameters(entity, Color::NIL, a);
		currentShaderColor = Color(0.5f, 0.5f, 0.5f, 0.5f);
		m_modelPart.render(0.0625f, &m_shaderMaterials.entity_color_overlay);
	}
}
