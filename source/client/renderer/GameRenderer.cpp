/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "compat/GL.hpp"
#include "GameRenderer.hpp"
#include "Minecraft.hpp"
#include "Frustum.hpp"

int t_keepPic;

GameRenderer::GameRenderer(Minecraft* pMinecraft)
{
	m_pMinecraft = pMinecraft;

	saveMatrices();

	m_pItemInHandRenderer = new ItemInHandRenderer(pMinecraft);
	EntityRenderDispatcher::getInstance()->m_pItemInHandRenderer = m_pItemInHandRenderer;
}

GameRenderer::~GameRenderer()
{
	delete m_pItemInHandRenderer;
}

void GameRenderer::zoomRegion(float a, float b, float c)
{
	field_44 = a;
	field_48 = b;
	field_4C = c;
}

void GameRenderer::unZoomRegion()
{
	field_44 = 1.0f;
}

void GameRenderer::setupCamera(float f, int i)
{
	field_8 = float(256 >> m_pMinecraft->m_options.field_10);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (m_pMinecraft->m_options.m_bAnaglyphs)
	{
		glTranslatef(float(1 - 2 * i) * 0.07f, 0.0f, 0.0f);
	}

	if (field_44 != 1.0)
	{
		glTranslatef(field_48, -field_4C, 0.0);
		glScalef(field_44, field_44, 1.0);
	}

	float fov = getFov(f);
	gluPerspective(fov, float(Minecraft::width) / float(Minecraft::height), 0.05f, field_8);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (m_pMinecraft->m_options.m_bAnaglyphs)
	{
		glTranslatef(float(2 * i - 1) * 0.1f, 0.0f, 0.0f);
	}

	bobHurt(f);
	if (m_pMinecraft->m_options.field_14)
		bobView(f);

	moveCameraToPlayer(f);
}

float x = 0, y = 0;

void GameRenderer::moveCameraToPlayer(float f)
{
	Mob* pMob = m_pMinecraft->m_pMobPersp;

	float headHeightDiff = pMob->field_84 - 1.62f;

	float posX = Lerp(pMob->field_3C.x, pMob->m_pos.x, f);
	float posY = Lerp(pMob->field_3C.y, pMob->m_pos.y, f);
	float posZ = Lerp(pMob->field_3C.z, pMob->m_pos.z, f);

	glRotatef(field_5C + f * (field_58 - field_5C), 0.0f, 0.0f, 1.0f);

	if (m_pMinecraft->m_options.field_23D)
	{
		float v11 = field_30 + (field_2C - field_30) * f;
		if (m_pMinecraft->m_options.field_241)
		{
			glTranslatef(0.0f, 0.0f, -v11);
			glRotatef(field_38 + (field_34 - field_38) * f, 1.0f, 0.0f, 0.0f);
			glRotatef(field_40 + (field_3C - field_40) * f, 0.0f, 1.0f, 0.0f);
		}
		else
		{
			float mob_yaw = pMob->m_yaw;
			float mob_pitch = pMob->m_pitch;

			float pitchRad = mob_pitch / 180.0f * float(M_PI);

			float aX = posX - (-(Mth::sin(mob_yaw / 180.0f * float(M_PI)) * Mth::cos(pitchRad)) * v11);
			float aY = posY + (Mth::sin(pitchRad) * v11);
			float aZ = posZ - ((Mth::cos(mob_yaw / 180.0f * float(M_PI)) * Mth::cos(pitchRad)) * v11);

			for (int i = 0; i < 8; i++)
			{
				float offsX = (2 * (i & 1) - 1) * 0.1f;
				float offsY = ((i & 2) - 1) * 0.1f;
				float offsZ = (2 * ((i >> 2) & 1) - 1) * 0.1f;

				HitResult hr = m_pMinecraft->m_pLevel->clip(
					Vec3(posX + offsX, posY + offsY, posZ + offsZ),
					Vec3(aX + offsX + offsZ, aY + offsY, aZ + offsZ) // @NOTE: Not sure why it adds offsZ to offsX.
				);

				if (hr.m_hitType != HitResult::NONE)
				{
					float dX = posX - hr.m_hitPos.x;
					float dY = posY - hr.m_hitPos.y;
					float dZ = posZ - hr.m_hitPos.z;
					float dist = sqrtf(dX * dX + dY * dY + dZ * dZ);
					if (v11 > dist)
						v11 = dist;
				}
			}

			// @HUH: Why the hell is it rotating by 0
			glRotatef(pMob->m_pitch - mob_pitch, 1.0f, 0.0f, 0.0f);
			glRotatef(pMob->m_yaw - mob_yaw, 0.0f, 1.0f, 0.0f);
			glTranslatef(0.0, 0.0, -v11);
			glRotatef(mob_yaw - pMob->m_yaw, 0.0f, 1.0f, 0.0f);
			glRotatef(mob_pitch - pMob->m_pitch, 1.0f, 0.0f, 0.0f);
		}
	}
	else
	{
		glTranslatef(0.0f, 0.0f, -0.1f);
	}

	if (!m_pMinecraft->m_options.field_241)
	{
		glRotatef(pMob->field_60 + f * (pMob->m_pitch - pMob->field_60), 1.0f, 0.0f, 0.0f);
		glRotatef(pMob->field_5C + f * (pMob->m_yaw   - pMob->field_5C) + 180.0f, 0.0f, 1.0f, 0.0f);
	}

	glTranslatef(0.0f, headHeightDiff, 0.0f);
}

void GameRenderer::saveMatrices()
{
	glGetFloatv(GL_PROJECTION_MATRIX, m_matrix_projection);
	glGetFloatv(GL_MODELVIEW_MATRIX,  m_matrix_model_view);
}

void GameRenderer::setupGuiScreen()
{
	float x = Gui::InvGuiScale * Minecraft::width;
	float y = Gui::InvGuiScale * Minecraft::height;
	glClear(GL_ACCUM);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	xglOrthof(0, float(int(x)), float(int(y)), 0, 2000.0f, 3000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2000.0f);
}

void GameRenderer::bobHurt(float f)
{
	Mob* pMob = m_pMinecraft->m_pMobPersp;

	if (pMob->m_health <= 0)
		glRotatef(-8000.0f / (float(pMob->field_110) + f + 200.0f) + 40.0f, 0.0f, 0.0f, 1.0f);

	if (pMob->field_104 > 0)
	{
		float val = (pMob->field_104 - f) / pMob->field_108;

		glRotatef(-pMob->field_10C, 0.0f, 1.0f, 0.0f);
		glRotatef(Mth::sin(val * val * val * val * 3.1416f) * -14.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(pMob->field_10C, 0.0f, 1.0f, 0.0f);
	}
}

void GameRenderer::bobView(float f)
{
	if (!m_pMinecraft->m_pMobPersp->isPlayer())
		return;

	Player* player = (Player*)m_pMinecraft->m_pMobPersp;
	float f1 = Lerp(player->field_B9C, player->field_BA0, f);
	float f2 = Lerp(player->field_118, player->field_11C, f);
	// @NOTE: Multiplying by M_PI inside of the paren makes it stuttery for some reason? Anyways it works now :)
	float f3 = -(player->field_94 + (player->field_94 - player->field_90) * f) * float(M_PI);
	float f4 = Mth::sin(f3);
	float f5 = Mth::cos(f3);
	glTranslatef((f4 * f1) * 0.5f, -fabsf(f5 * f1), 0.0f);
	float f6 = Mth::sin(f3);
	glRotatef((f6 * f1) * 3.0f, 0.0f, 0.0f, 1.0f);
	float f7 = Mth::cos(f3 - 0.2f);
	glRotatef(fabsf(f7 * f1) * 5.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(f2, 1.0f, 0.0f, 0.0f);
}

void GameRenderer::setupClearColor(float f)
{
	Minecraft* pMC = m_pMinecraft;
	Level* pLevel = pMC->m_pLevel;
	Mob* pMob = pMC->m_pMobPersp;

	float x1 = 1.0f - powf(1.0f / float(4 - pMC->m_options.field_10), 0.25f);

	Vec3 skyColor = pLevel->getSkyColor(pMob, f), fogColor = pLevel->getFogColor(f);

	//@BUG: double set to these?
	field_60 = fogColor.x;
	field_64 = fogColor.y;

	field_60 = fogColor.x + (skyColor.x - fogColor.x) * x1;
	field_64 = fogColor.y + (skyColor.y - fogColor.y) * x1;
	field_68 = fogColor.z + (skyColor.z - fogColor.z) * x1;

	if (pMob->isUnderLiquid(Material::water))
	{
		field_60 = 0.02f;
		field_64 = 0.02f;
		field_68 = 0.2f;
	}
	else if (pMob->isUnderLiquid(Material::lava))
	{
		field_60 = 0.6f;
		field_64 = 0.1f;
		field_68 = 0.0f;
	}

	float x2 = field_6C + (field_70 - field_6C) * f;

	field_60 *= x2;
	field_64 *= x2;
	field_68 *= x2;

	if (pMC->m_options.m_bAnaglyphs)
	{
		float r = (field_60 * 30.0f + field_64 * 59.0f + field_68 * 11.0f) / 100.0f;
		float g = (field_60 * 30.0f + field_64 * 70.0f) / 100.0f;
		float b = (field_60 * 30.0f + field_68 * 70.0f) / 100.0f;

		field_60 = r;
		field_64 = g;
		field_68 = b;
	}

	glClearColor(field_60, field_64, field_68, 1.0f);
}

#ifndef ORIGINAL_CODE
void GameRenderer::renderNoCamera()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
#endif

void GameRenderer::setupFog(int i)
{
	float fog_color[4];
	fog_color[0] = field_60;
	fog_color[1] = field_64;
	fog_color[2] = field_68;
	fog_color[3] = 1.0f;

	glFogfv(GL_FOG_COLOR, fog_color);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (m_pMinecraft->m_pMobPersp->isUnderLiquid(Material::water))
	{
	#ifdef ORIGINAL_CODE
		glFogx(GL_FOG_MODE, 0x0800);
	#else
		glFogi(GL_FOG_MODE, GL_EXP);
	#endif

		glFogf(GL_FOG_DENSITY, 0.1f);
	}
	else if (m_pMinecraft->m_pMobPersp->isUnderLiquid(Material::lava))
	{
	#ifdef ORIGINAL_CODE
		glFogx(GL_FOG_MODE, 0x0800);
	#else
		glFogi(GL_FOG_MODE, GL_EXP);
	#endif

		glFogf(GL_FOG_DENSITY, 2.0f);
	}
	else
	{
	#ifdef ORIGINAL_CODE
		glFogx(GL_FOG_MODE, GL_LINEAR);
	#else
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, field_8 * 0.25f);
		glFogf(GL_FOG_END,   field_8);
		if (i < 0)
		{
			glFogf(GL_FOG_START, 0.0f);
			glFogf(GL_FOG_END,   field_8 * 0.8f);
		}

		if (m_pMinecraft->m_pLevel->m_pDimension->field_C)
		{
			glFogf(GL_FOG_START, 0.0f);
		}
	#endif
	}

	glEnable(GL_COLOR_MATERIAL);
}

float GameRenderer::getFov(float f)
{
	Mob* pMob = m_pMinecraft->m_pMobPersp;
	
	float x1 = 70.0f;

	if (pMob->isUnderLiquid(Material::water))
		x1 = 60.0f;

	if (pMob->m_health <= 0)
	{
		float x2 = 1.0f + (-500.0f / ((pMob->field_110 + f) + 500.0f));
		x1 /= (1.0f + 2.0f * x2);
	}

	return field_54 + x1 + f * (field_50 - field_54);
}

void GameRenderer::renderLevel(float f)
{
	if (!m_pMinecraft->m_pMobPersp)
	{
		m_pMinecraft->m_pMobPersp = m_pMinecraft->m_pLocalPlayer;

		if (!m_pMinecraft->m_pMobPersp)
		{
		#ifndef ORIGINAL_CODE
			renderNoCamera();
		#endif
			return;
		}
	}

	pick(f);

	Mob* pMob = m_pMinecraft->m_pMobPersp;
	Vec3 fCamPos;

	fCamPos.x = pMob->field_98.x + (pMob->m_pos.x - pMob->field_98.x) * f;
	fCamPos.y = pMob->field_98.y + (pMob->m_pos.y - pMob->field_98.y) * f;
	fCamPos.z = pMob->field_98.z + (pMob->m_pos.z - pMob->field_98.z) * f;

	bool bAnaglyph = m_pMinecraft->m_options.m_bAnaglyphs;

	LevelRenderer* pLR = m_pMinecraft->m_pLevelRenderer;
	ParticleEngine* pPE = m_pMinecraft->m_pParticleEngine;

	for (int i = 0; i < 2; i++)
	{
		if (bAnaglyph)
		{
			if (i > 0)
				glColorMask(true, false, false, false);
			else
				glColorMask(false, true, true, false);
		}

		glViewport(0, 0, Minecraft::width, Minecraft::height);
		setupClearColor(f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		setupCamera(f, i);
		saveMatrices();

		if (m_pMinecraft->m_options.field_10 <= 1)
		{
#ifndef ORIGINAL_CODE
			// @NOTE: For whatever reason, Minecraft doesn't enable GL_FOG right away.
			// It appears to work in bluestacks for whatever reason though...
			glEnable(GL_FOG);
#endif
			setupFog(-1);
			pLR->renderSky(f);
		}

		glEnable(GL_FOG);
		setupFog(1);

		if (m_pMinecraft->m_options.field_18)
			glShadeModel(GL_SMOOTH);

		Frustum& frust = Frustum::frustum;
		Frustum::doOurJobInGameRenderer();

		FrustumCuller frustumCuller;
		frustumCuller.m_frustumData.x = frust;
		frustumCuller.prepare(fCamPos.x, fCamPos.y, fCamPos.z);

		pLR->cull(&frustumCuller, f);
		pLR->updateDirtyChunks(pMob, false);

		setupFog(0);
		glEnable(GL_FOG);

		m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		// render the opaque layer
		pLR->render(pMob, 0, f);

		glShadeModel(GL_FLAT);
		pLR->renderEntities(pMob->getPos(f), &frustumCuller, f);
		pPE->render(pMob, f);

		glEnable(GL_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		setupFog(0);

#ifndef ORIGINAL_CODE
		glShadeModel(GL_SMOOTH);
#endif

		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		// glDepthMask(false); -- added in 0.1.1j. Introduces more issues than fixes

		// render the alpha layer
		m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		pLR->render(pMob, 1, f);

		glDepthMask(true);


#ifndef ORIGINAL_CODE
		glShadeModel(GL_FLAT);
#endif
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		if (field_44 == 1.0f && pMob->isPlayer() && m_pMinecraft->m_hitResult.m_hitType != HitResult::NONE && !pMob->isUnderLiquid(Material::water))
		{
			glDisable(GL_ALPHA_TEST);
			pLR->renderHitSelect((Player*)pMob, m_pMinecraft->m_hitResult, 0, nullptr, f);
			glEnable(GL_ALPHA_TEST);
		}

		// @BUG?
		glDisable(GL_FOG);

		if (field_44 == 1.0f)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			renderItemInHand(f, i);
		}

		if (!bAnaglyph)
			break;
	}

	if (bAnaglyph)
		glColorMask(true, true, true, false);
}

void GameRenderer::render(float f)
{
	if (m_pMinecraft->m_pLocalPlayer && m_pMinecraft->m_bGrabbedMouse)
	{
		auto pMC = m_pMinecraft;
		auto delta = pMC->m_pTurnInput->getTurnDelta();
		pMC->field_D20 = delta.x;
		pMC->field_D24 = delta.y;

#ifndef ENH_DISABLE_TURN_ACCEL
		float multPitch = -1.0f;
		float mult1 = 2.0f * (0.2f + pMC->m_options.field_8 * 0.6f);
		mult1 = mult1 * mult1 * mult1;

		float xd = 4.0f * mult1 * pMC->field_D20;
		float yd = 4.0f * mult1 * pMC->field_D24;

		float old_field_84 = field_84;
		field_84 = float(field_C) + f;
		float diff_field_84 = field_84 - old_field_84;
		field_74 += xd;
		field_78 += yd;

		if (diff_field_84 > 3.0f)
			diff_field_84 = 3.0f;

		if (pMC->m_options.m_bInvertMouse)
			multPitch = 1.0f;

		if (!pMC->m_options.field_240)
		{
			// @TODO: untangle this code
			float v17 = xd + field_14;
			float v18 = field_18;
			float v19 = field_1C;
			field_14 = v17;
			float v20 = mult1 * 0.25f * (v17 - v18);
			float v21 = v19 + (v20 - v19) * 0.5f;
			field_1C = v21;
			if ((v20 <= 0.0 || v20 <= v21) && (v20 >= 0.0 || v20 >= v21))
				v21 = mult1 * 0.25f * (v17 - v18);
			float v22 = yd + field_20;
			field_18 = v18 + v21;
			float v23 = field_24;
			field_20 = v22;
			float v24 = mult1 * 0.15f * (v22 - v23);
			float v25 = field_28 + (v24 - field_28) * 0.5f;
			field_28 = v25;
			if ((v24 <= 0.0 || v24 <= v25) && (v24 >= 0.0 || v24 >= v25))
				v25 = v24;
			field_24 = v23 + v25;
		}
#else
		float multPitch = -1.0f;
		if (pMC->m_options.m_bInvertMouse)
			multPitch = 1.0f;

		float diff_field_84 = 1.0f;
		field_7C = pMC->field_D20;
		field_80 = pMC->field_D24;
#endif

		pMC->m_pLocalPlayer->turn(diff_field_84 * field_7C, diff_field_84 * multPitch * field_80);
	}

	if (m_pMinecraft->isLevelGenerated())
	{
		if (t_keepPic < 0)
		{
			renderLevel(f);
			if (m_pMinecraft->m_options.field_23C)
			{
				if (!m_pMinecraft->m_pScreen)
					return;
			}

			m_pMinecraft->m_gui.render(f, m_pMinecraft->m_pScreen != nullptr, int(Mouse::_x * Gui::InvGuiScale), int(Mouse::_y * Gui::InvGuiScale));
		}
	}
	else
	{
		glViewport(0, 0, Minecraft::width, Minecraft::height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		setupGuiScreen();
	}

	if (m_pMinecraft->m_pScreen)
	{
		glClear(GL_ACCUM);
		m_pMinecraft->m_pScreen->render(int(Mouse::_x * Gui::InvGuiScale), int(Mouse::_y * Gui::InvGuiScale), f);

		if (m_pMinecraft->m_pScreen && !m_pMinecraft->m_pScreen->isInGameScreen())
		{
#ifdef ORIGINAL_CODE
			sleepMs(15);
#endif
		}
	}
}

void GameRenderer::tick()
{
	--t_keepPic;
#ifndef ORIGINAL_CODE
	// @BUG: If the game is left on for approximately 1,242 days, the counter will underflow,
	// causing the screen to appear frozen, and the level to not render.
	if (t_keepPic < -100)
		t_keepPic = -100;
#endif

	if (m_pMinecraft->m_pLocalPlayer)
	{
		float x1 = powf(fabsf(field_74), 1.2f);
		field_7C = x1 * 0.4f;
		if (field_74 < 0.0f)
			field_7C = -field_7C;

		float x2 = powf(fabsf(field_78), 1.2f);
		field_80 = x2 * 0.4f;
		if (field_78 < 0.0f)
			field_80 = -field_80;

		field_74 = 0.0f;
		field_78 = 0.0f;
		field_6C = field_70;
		field_30 = field_2C;
		field_38 = field_34;
		field_40 = field_3C;
		field_54 = field_50;
		field_5C = field_58;

		Mob* pMob = m_pMinecraft->m_pMobPersp;
		if (!pMob)
		{
			pMob = m_pMinecraft->m_pMobPersp = m_pMinecraft->m_pLocalPlayer;
		}

		float bright = m_pMinecraft->m_pLevel->getBrightness(Mth::floor(pMob->m_pos.x), Mth::floor(pMob->m_pos.y), Mth::floor(pMob->m_pos.z));
		float x3 = float(3 - m_pMinecraft->m_options.field_10);

		field_C++;

		float x4 = x3 / 3.0f;
		float x5 = (x4 + bright * (1.0f - x4) - field_70) * 0.1f;

		field_70 += x5;

		m_pItemInHandRenderer->tick();
	}
}

void GameRenderer::renderItemInHand(float f, int i)
{
	glLoadIdentity();

	if (m_pMinecraft->m_options.m_bAnaglyphs)
		glTranslatef(float(2 * i - 1) * 0.1f, 0.0f, 0.0f);

	glPushMatrix();
	bobHurt(f);

	if (m_pMinecraft->m_options.field_14)
		bobView(f);

	if (!m_pMinecraft->m_options.field_23D && !m_pMinecraft->m_options.field_23C)
		m_pItemInHandRenderer->render(f);

	glPopMatrix();

	if (!m_pMinecraft->m_options.field_23D)
	{
		m_pItemInHandRenderer->renderScreenEffect(f);
		bobHurt(f);
	}

	if (m_pMinecraft->m_options.field_14)
		bobView(f);
}

void GameRenderer::onGraphicsReset()
{

}

void GameRenderer::pick(float f)
{
	if (!m_pMinecraft->m_pMobPersp || !m_pMinecraft->m_pLevel)
		return;

	HitResult& mchr = m_pMinecraft->m_hitResult;

	Mob* pMob = m_pMinecraft->m_pMobPersp;

	float dist = m_pMinecraft->m_pGameMode->getPickRange();

	HitResult hrMob = pMob->pick(dist, f);
	mchr = hrMob;

	Vec3 mobPos = pMob->getPos(f);

	if (m_pMinecraft->m_hitResult.m_hitType != HitResult::NONE)
	{
		float dX = mobPos.x - mchr.m_hitPos.x;
		float dY = mobPos.y - mchr.m_hitPos.y;
		float dZ = mobPos.z - mchr.m_hitPos.z;

		dist = sqrtf(dX * dX + dY * dY + dZ * dZ);
	}

	if (m_pMinecraft->m_pGameMode->isCreativeType())
		dist = 32.0f;
	else if (dist > 3.0f)
		dist = 3.0f;

	Vec3 view = pMob->getViewVector(f);
	Vec3 exp  = view * dist;
	Vec3 limit = mobPos + view * dist;

	field_10 = nullptr;

	AABB scanAABB = pMob->m_hitbox;

	if (exp.x < 0) scanAABB.min.x += exp.x;
	if (exp.x > 0) scanAABB.max.x += exp.x;
	if (exp.y < 0) scanAABB.min.y += exp.y;
	if (exp.y > 0) scanAABB.max.y += exp.y;
	if (exp.z < 0) scanAABB.min.z += exp.z;
	if (exp.z > 0) scanAABB.max.z += exp.z;

	scanAABB.grow(1, 1, 1);

	EntityVector* pEnts = m_pMinecraft->m_pLevel->getEntities(pMob, scanAABB);

	float fDist = 0.0f;
	for (auto pEnt : *pEnts)
	{
		if (!pEnt->isPickable())
			continue;

		AABB checkAABB = pEnt->m_hitbox;
		checkAABB.grow(pEnt->getPickRadius());

		HitResult hrMobChk = checkAABB.clip(mobPos, limit);

		if (checkAABB.contains(mobPos))
		{
			if (fDist >= 0.0f)
			{
				//this is it brother
				field_10 = pEnt;
				fDist = 0.0f;
			}
			continue;
		}

		if (hrMobChk.m_hitType != HitResult::NONE)
		{
			float dX = hrMobChk.m_hitPos.x - mobPos.x;
			float dY = hrMobChk.m_hitPos.y - mobPos.y;
			float dZ = hrMobChk.m_hitPos.z - mobPos.z;
			float fNewDist = sqrtf(dX * dX + dY * dY + dZ * dZ);

			if (fDist > fNewDist || fDist == 0.0f)
			{
				field_10 = pEnt;
				fDist = fNewDist;
			}
		}
	}

	// picked entities take priority over tiles (?!)
	if (field_10)
	{
		if (!m_pMinecraft->m_pGameMode->isCreativeType())
		{
			m_pMinecraft->m_hitResult = HitResult(field_10);
		}

		return;
	}

	if (m_pMinecraft->m_hitResult.m_hitType != HitResult::NONE || view.y >= -0.7f)
		return;

	mobPos = pMob->getPos(f);
	Vec3 checkVec = mobPos;
	checkVec.translate(0, -2, 0);

	HitResult hrLevelChk = m_pMinecraft->m_pLevel->clip(mobPos, checkVec);

	if (hrLevelChk.m_hitType == HitResult::NONE)
		return;

	mchr = hrLevelChk;
	mchr.m_bUnk24 = true;

	if (fabsf(view.x) <= fabsf(view.z))
	{
		m_pMinecraft->m_hitResult.m_hitSide = view.z >= 0.0f ? HitResult::MAXZ : HitResult::MINZ;
	}
	else
	{
		m_pMinecraft->m_hitResult.m_hitSide = view.x >= 0.0f ? HitResult::MAXX : HitResult::MINX;
	}
}

