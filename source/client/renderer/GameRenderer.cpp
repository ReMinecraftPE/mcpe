/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GameRenderer.hpp"
#include "GameMods.hpp"
#include "client/app/Minecraft.hpp"
#include "client/player/input/Multitouch.hpp"
#include "client/player/input/GameControllerManager.hpp"
#include "Frustum.hpp"
#include "renderer/GL/GL.hpp"
#include "renderer/GlobalConstantBuffers.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "thirdparty/glm/glm.hpp"

//#define SHOW_VERTEX_COUNTER_GRAPHIC

#if defined SHOW_VERTEX_COUNTER_GRAPHIC && !defined _DEBUG
#undef  SHOW_VERTEX_COUNTER_GRAPHIC
#endif

#define C_MENU_POINTER_WIDTH 16
#define C_MENU_POINTER_HEIGHT 16

static int t_keepHitResult; // that is its address in v0.1.1j

void GameRenderer::_init()
{
	field_14 = 0.0f;
	field_18 = 0.0f;
	field_1C = 0.0f;
	field_20 = 0.0f;
	field_24 = 0.0f;
	field_28 = 0.0f;
	field_2C = 4.0f;
	field_30 = 4.0f;
	field_34 = 0.0f;
	field_38 = 0.0f;
	field_3C = 0.0f;
	field_40 = 0.0f;
	m_zoom = 1.0f;
	field_50 = 0.0f;
	field_54 = 0.0f;
	field_58 = 0.0f;
	field_5C = 0.0f;
	field_84 = 0.0f;

	//ItemInHandRenderer* m_pItemInHandRenderer = nullptr;
	m_pLevel = nullptr;

	m_renderDistance = 0.0f;
	field_C = 0;
	m_pHovered = nullptr;

	m_shownFPS = m_shownChunkUpdates = m_lastUpdatedMS = 0;

	m_keepPic = 0;

	m_envTexturePresence = 0;
}

GameRenderer::GameRenderer(Minecraft* pMinecraft) :
	m_pMinecraft(pMinecraft)
{
	_init();
	_initResources();

	saveMatrices();

	m_pItemInHandRenderer = new ItemInHandRenderer(pMinecraft);
	EntityRenderDispatcher::getInstance()->m_pItemInHandRenderer = m_pItemInHandRenderer;

#ifdef FEATURE_GFX_SHADERS
	mce::GlobalConstantBuffers::getInstance().init();
#endif
}

GameRenderer::~GameRenderer()
{
	delete m_pItemInHandRenderer;
}

void GameRenderer::_buildPointerMesh()
{
	IntRectangle rect;
	{
		rect.x = 0;
		rect.y = 0;
		rect.w = C_MENU_POINTER_WIDTH;
		rect.h = C_MENU_POINTER_HEIGHT;
	}

	ScreenRenderer::singleton().blit(m_pointerMesh, rect);
}

void GameRenderer::_initResources()
{
	_buildPointerMesh();
}

void GameRenderer::_clearFrameBuffer()
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	renderContext.setViewport(Minecraft::width, Minecraft::height, 0.0f, 0.7f);
	renderContext.setRenderTarget();
	renderContext.clearFrameBuffer(Color(0.0f, 0.3f, 0.2f, 0.0f));
	renderContext.clearDepthStencilBuffer();
}

void GameRenderer::_renderItemInHand(float f, int i)
{
	Matrix& viewMtx = MatrixStack::View.getTop();
	viewMtx = Matrix::IDENTITY;

	if (m_pMinecraft->getOptions()->m_anaglyphs.get())
	{
		viewMtx.translate(Vec3(float(2 * i - 1) * 0.1f, 0.0f, 0.0f));
	}

	MatrixStack::Ref matrix = MatrixStack::World.push();

	bobHurt(matrix, f);

	if (m_pMinecraft->getOptions()->m_viewBobbing.get())
	{
		bobView(matrix, f);
	}

	if (!m_pMinecraft->getOptions()->m_thirdPerson.get() && !m_pMinecraft->getOptions()->m_hideGui.get())
	{
		mce::RenderContextImmediate::get().clearDepthStencilBuffer();
		m_pItemInHandRenderer->render(f);
	}

	if (!m_pMinecraft->getOptions()->m_thirdPerson.get())
	{
		m_pItemInHandRenderer->renderScreenEffect(f);
		bobHurt(matrix, f);
	}

	if (m_pMinecraft->getOptions()->m_viewBobbing.get())
	{
		bobView(matrix, f);
	}
}

void GameRenderer::_renderDebugOverlay(float a)
{
	Font& font = *m_pMinecraft->m_pFont;

	std::stringstream debugText;
	debugText << "ReMinecraftPE " << m_pMinecraft->getVersionString();
	debugText << " (" << m_shownFPS << " fps, " << m_shownChunkUpdates << " chunk updates)" << "\n";

	/*
	 * The "!m_pMinecraft->m_bPreparingLevel" check *needs* to be here.
	 * If said check is not here, when getBiome() is called for the biome display,
	 * it would allocate an array with a size of 1 before the level was even generated.
	 * Then, during level generation, said array would be written to as if it had a size
	 * of 256, leading to a heap corruption that took ASan to debug successfully.
	 * Unfortunately, ASan and DirectSound don't mix, and Microsoft's ASan team has stated that they don't even know why:
	 * https://developercommunity.visualstudio.com/t/ASAN-x64-causes-unhandled-exception-at-0/1365655#T-N10460750
	 * Since all SoundSystems are backed with DirectSound, SoundSystemNull is needed to use ASan.
	 * This heap corruption bug, which (only if the F3 menu was open) would cause multiplayer functionality to be entirely
	 * based on luck, had been around since Commit 53200be, on March 5th of 2025, and was fixed on September 30th of 2025.
	 */
	if (m_pMinecraft->m_pLocalPlayer && !m_pMinecraft->m_bPreparingLevel)
	{
		char posStr[96];
		Vec3 pos = m_pMinecraft->m_pLocalPlayer->getPos(a);
		sprintf(posStr, "%.2f / %.2f / %.2f", pos.x, pos.y, pos.z);

		debugText << m_pMinecraft->m_pLevelRenderer->gatherStats1();
		debugText << m_pMinecraft->m_pLevelRenderer->gatherStats2() << "\n";
		debugText << "XYZ: " << posStr << "\n";
		debugText << "Biome: " << m_pMinecraft->m_pLevel->getBiomeSource()->getBiome(pos)->m_name << "\n";
	}

#ifdef SHOW_VERTEX_COUNTER_GRAPHIC
	extern int g_nVertices; // Tesselator.cpp
	debugText << "\nverts: " << g_nVertices;

	_renderVertexGraph(g_nVertices, int(Minecraft::height * Gui::InvGuiScale));
#endif

	/*debugText << "\nGameControllerManager::stickValuesX[1]: " << GameControllerManager::stickValuesX[1];
	debugText << "\nGameControllerManager::stickValuesY[1]: " << GameControllerManager::stickValuesY[1];
	debugText << "\nGameRenderer::m_turnDelta.x: "      << m_turnDelta.x;
	debugText << "\nGameRenderer::m_turnDelta.y: "      << m_turnDelta.y;*/

	font.drawShadow(debugText.str(), 2, 2, Color::WHITE);

#ifdef SHOW_VERTEX_COUNTER_GRAPHIC
	g_nVertices = 0;
#endif
}

void GameRenderer::_renderVertexGraph(int vertices, int h)
{
	ScreenRenderer& screenRenderer = ScreenRenderer::singleton();
	Font& font = *m_pMinecraft->m_pFont;

	static int vertGraph[200];
	memmove(vertGraph, vertGraph + 1, sizeof(vertGraph) - sizeof(int));
	vertGraph[(sizeof(vertGraph) / sizeof(vertGraph[0])) - 1] = vertices;

	Tesselator& t = Tesselator::instance;

	int max = 0;
	for (int i = 0; i < 200; i++)
		max = std::max(max, vertGraph[i]);

	int maxht = 100;

	//glClear(GL_DEPTH_BUFFER_BIT);
	currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color::WHITE;

	t.begin(4);
	t.color(1.0f, 1.0f, 1.0f, 0.15f);
	t.vertex(000, h - maxht, 0);
	t.vertex(000, h, 0);
	t.vertex(200, h, 0);
	t.vertex(200, h - maxht, 0);
	t.draw(screenRenderer.m_materials.ui_fill_color);

	t.begin(200 * 4);
	t.color(0.0f, 1.0f, 0.0f, 1.0f);

	for (int i = 0; i < 200 && max != 0; i++)
	{
		t.vertex(i + 0, h - (vertGraph[i] * maxht / max), 0);
		t.vertex(i + 0, h - 0, 0);
		t.vertex(i + 1, h - 0, 0);
		t.vertex(i + 1, h - (vertGraph[i] * maxht / max), 0);
	}

	t.draw(screenRenderer.m_materials.ui_fill_color);

	screenRenderer.drawString(font, SSTR(max), 200, h - maxht);
}

void GameRenderer::zoomRegion(float zoom, const Vec2& region)
{
	m_zoom = zoom;
	m_zoomRegion = region;
}

void GameRenderer::unZoomRegion()
{
	m_zoom = 1.0f;
}

void GameRenderer::setupCamera(float f, int i)
{
	m_renderDistance = float(256 >> m_pMinecraft->getOptions()->m_viewDistance.get());

	Matrix& projMtx = MatrixStack::Projection.getTop();
	projMtx = Matrix::IDENTITY;

	if (m_pMinecraft->getOptions()->m_anaglyphs.get())
	{
		projMtx.translate(Vec3(float(1 - 2 * i) * 0.07f, 0.0f, 0.0f));
	}

	if (m_zoom != 1.0)
	{
		projMtx.translate(Vec3(m_zoomRegion.x, -m_zoomRegion.y, 0.0f));
		projMtx.scale(Vec3(m_zoom, m_zoom, 1.0f));
	}

	float fov = getFov(f);
	// Java
	//projMtx.setPerspective(fov, float(Minecraft::width) / float(Minecraft::height), 0.05f, m_renderDistance);
	// PE (0.12.1)
	projMtx.setPerspective(fov, float(Minecraft::width) / float(Minecraft::height), 0.05f, m_renderDistance * 1.2f);

	Matrix& viewMtx = MatrixStack::View.getTop();
	viewMtx = Matrix::IDENTITY;

	if (m_pMinecraft->getOptions()->m_anaglyphs.get())
	{
		viewMtx.translate(Vec3(float(2 * i - 1) * 0.1f, 0.0f, 0.0f));
	}

	bobHurt(viewMtx, f);
	if (m_pMinecraft->getOptions()->m_viewBobbing.get())
	{
		bobView(viewMtx, f);
	}

	moveCameraToPlayer(viewMtx, f);
}

void GameRenderer::moveCameraToPlayer(Matrix& matrix, float f)
{
	Mob* pMob = m_pMinecraft->m_pCameraEntity;

	float headHeightDiff = pMob->m_heightOffset - 1.62f;

	float posX = Mth::Lerp(pMob->m_oPos.x, pMob->m_pos.x, f);
	float posY = Mth::Lerp(pMob->m_oPos.y, pMob->m_pos.y, f);
	float posZ = Mth::Lerp(pMob->m_oPos.z, pMob->m_pos.z, f);

	matrix.rotate(field_5C + f * (field_58 - field_5C), Vec3::UNIT_Z);

	if (m_pMinecraft->getOptions()->m_thirdPerson.get())
	{
		float v11 = field_30 + (field_2C - field_30) * f;
		if (m_pMinecraft->getOptions()->field_241)
		{
			matrix.translate(Vec3::NEG_UNIT_Z * v11);
			matrix.rotate(field_38 + (field_34 - field_38) * f, Vec3::UNIT_X);
			matrix.rotate(field_40 + (field_3C - field_40) * f, Vec3::UNIT_Y);
		}
		else
		{
			float mob_yaw = pMob->m_rot.x;
			float mob_pitch = pMob->m_rot.y;

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
					float dist = Mth::sqrt(dX * dX + dY * dY + dZ * dZ);
					if (v11 > dist)
						v11 = dist;
				}
			}

			matrix.rotate(pMob->m_rot.y - mob_pitch, Vec3::UNIT_X);
			matrix.rotate(pMob->m_rot.x - mob_yaw, Vec3::UNIT_Y);
			matrix.translate(Vec3::NEG_UNIT_Z * v11);
			matrix.rotate(mob_yaw - pMob->m_rot.x, Vec3::UNIT_Y);
			matrix.rotate(mob_pitch - pMob->m_rot.y, Vec3::UNIT_X);
		}
	}
	else
	{
		matrix.translate(Vec3(0.0f, 0.0f, -0.1f));
	}

	if (!m_pMinecraft->getOptions()->field_241)
	{
		matrix.rotate(pMob->m_oRot.y + f * (pMob->m_rot.y - pMob->m_oRot.y), Vec3::UNIT_X);
		matrix.rotate(pMob->m_oRot.x + f * (pMob->m_rot.x - pMob->m_oRot.x) + 180.0f, Vec3::UNIT_Y);
	}

	matrix.translate(Vec3::UNIT_Y * headHeightDiff);
}

void GameRenderer::saveMatrices()
{
	if (m_pMinecraft->useSplitControls() || !m_pMinecraft->m_pInputHolder->allowPicking())
		return;

	// Save projection matrix
	m_mtxProj = MatrixStack::Projection.top();

	// Save modelview matrix
	const Matrix& worldMatrix = MatrixStack::World.top();
	const Matrix& viewMatrix = MatrixStack::View.top();
	// Order matters!
	m_mtxView = viewMatrix * worldMatrix;
}

void GameRenderer::setupGuiScreen()
{
	float x = Gui::InvGuiScale * Minecraft::width;
	float y = Gui::InvGuiScale * Minecraft::height;

	Matrix& projMtx = MatrixStack::Projection.getTop();
	projMtx.setOrtho(0, x, y, 0, 1000.0f, 3000.0f); // 1000 for the znear is accurate to the original b1.7.3, and causes less depth problems

	Matrix& viewMtx = MatrixStack::View.getTop();
	viewMtx = Matrix::IDENTITY;
	viewMtx.translate(Vec3(0.0f, 0.0f, -2000.0f)); // -2000 to -3000
}

void GameRenderer::bobHurt(Matrix& matrix, float f)
{
	Mob* pMob = m_pMinecraft->m_pCameraEntity;

	if (pMob->m_health <= 0)
	{
		matrix.rotate(-8000.0f / (float(pMob->m_deathTime) + f + 200.0f) + 40.0f, Vec3::UNIT_Z);
	}

	if (pMob->m_hurtTime > 0)
	{
		float val = (pMob->m_hurtTime - f) / pMob->m_hurtDuration;

		matrix.rotate(-pMob->m_hurtDir, Vec3::UNIT_Y);
		matrix.rotate(Mth::sin(val * val * val * val * 3.1416f) * -14.0f, Vec3::UNIT_Z);
		matrix.rotate(pMob->m_hurtDir, Vec3::UNIT_Y);
	}
}

void GameRenderer::bobView(Matrix& matrix, float f)
{
	if (!m_pMinecraft->m_pCameraEntity->isPlayer())
		return;

	Player* player = (Player*)m_pMinecraft->m_pCameraEntity;
	float f1 = Mth::Lerp(player->m_oBob, player->m_bob, f);
	float f2 = Mth::Lerp(player->m_oTilt, player->m_tilt, f);
	// @NOTE: Multiplying by M_PI inside of the paren makes it stuttery for some reason? Anyways it works now :)
	float f3 = -(player->m_walkDist + (player->m_walkDist - player->field_90) * f) * float(M_PI);
	float f4 = Mth::sin(f3);
	float f5 = Mth::cos(f3);
	float f6 = Mth::cos(f3 - 0.2f);

	matrix.translate(Vec3((f4 * f1) * 0.5f, -fabsf(f5 * f1), 0.0f));
	matrix.rotate((f4 * f1) * 3.0f, Vec3::UNIT_Z);
	matrix.rotate(fabsf(f6 * f1) * 5.0f, Vec3::UNIT_X);
	matrix.rotate(f2, Vec3::UNIT_X);
}

#ifndef ORIGINAL_CODE
void GameRenderer::renderNoCamera()
{
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	renderContext.clearFrameBuffer(Color::WHITE);
	renderContext.clearDepthStencilBuffer();
}
#endif

float GameRenderer::getFov(float f)
{
	Mob* pMob = m_pMinecraft->m_pCameraEntity;

	float x1 = 70.0f;

	if (pMob->isUnderLiquid(Material::water))
		x1 = 60.0f;

	if (pMob->m_health <= 0)
	{
		float x2 = 1.0f + (-500.0f / ((pMob->m_deathTime + f) + 500.0f));
		x1 /= (1.0f + 2.0f * x2);
	}

	return field_54 + x1 + f * (field_50 - field_54);
}

void GameRenderer::renderLevel(float f)
{
	if (!m_pLevel)
		return;

	Mob*& pCamera = m_pMinecraft->m_pCameraEntity;
	if (!pCamera)
	{
		pCamera = m_pMinecraft->m_pLocalPlayer;

		if (!pCamera)
		{
		#ifndef ORIGINAL_CODE
			renderNoCamera();
		#endif
			return;
		}
	}

	pick(f);

	const Entity& camera = *pCamera;
	Vec3 camPos = camera.m_posPrev + (camera.m_pos - camera.m_posPrev) * f;

	bool bAnaglyph = m_pMinecraft->getOptions()->m_anaglyphs.get();

	LevelRenderer& levelRenderer = *m_pMinecraft->m_pLevelRenderer;
	ParticleEngine& particleEngine = *m_pMinecraft->m_pParticleEngine;
	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	for (int i = 0; i < 2; i++)
	{
		if (bAnaglyph)
		{
#if MCE_GFX_API_OGL
			if (i > 0)
				glColorMask(true, false, false, false);
			else
				glColorMask(false, true, true, false);
#endif
		}

		renderContext.setViewport(Minecraft::width, Minecraft::height, 0.0f, 0.7f);
		renderContext.setRenderTarget();
		const Color& clearColor = levelRenderer.setupClearColor(f);
		renderContext.clearFrameBuffer(clearColor);
		renderContext.clearDepthStencilBuffer();

		setupCamera(f, i);
		saveMatrices();

		renderFramedItems(camPos, levelRenderer, camera, f, particleEngine, i);

		if (!bAnaglyph)
			break;
	}

	if (bAnaglyph)
	{
#if MCE_GFX_API_OGL
		glColorMask(true, true, true, false);
#endif
	}
}

void GameRenderer::renderFramedItems(const Vec3& camPos, LevelRenderer& levelRenderer, const Entity& camera, float f, ParticleEngine& particleEngine, float i)
{
	/*
	if (m_pMinecraft->getOptions()->m_viewDistance <= 1)
	{
#ifndef ORIGINAL_CODE
			// @NOTE: For whatever reason, Minecraft doesn't enable GL_FOG right away.
			// It appears to work in bluestacks for whatever reason though...
			Fog::enable();
#endif
			setupFog(-1);
			pLR->renderSky(f);
		}
		*/

	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	if (m_pMinecraft->getOptions()->m_ambientOcclusion.get())
	{
		renderContext.setShadeMode(mce::SHADE_MODE_SMOOTH);
	}

	Frustum& frust = Frustum::frustum;
	Frustum::calculateFrustum();

	FrustumCuller frustumCuller;
	frustumCuller.m_frustumData.x = frust;
	frustumCuller.prepare(camPos);

	levelRenderer.renderLevel(camera, frustumCuller, m_renderDistance, f);

	if (m_zoom == 1.0f && camera.isPlayer() && m_pMinecraft->m_hitResult.m_hitType != HitResult::NONE && !camera.isUnderLiquid(Material::water))
	{
		levelRenderer.renderCracks(camera, m_pMinecraft->m_hitResult, 0, nullptr, f);

		if (m_pMinecraft->getOptions()->m_blockOutlines.get())
			levelRenderer.renderHitOutline(camera, m_pMinecraft->m_hitResult, 0, nullptr, f);
		else
			levelRenderer.renderHitSelect(camera, m_pMinecraft->m_hitResult, 0, nullptr, f);
	}

	if (m_zoom == 1.0f)
	{
		_renderItemInHand(f, i);
	}
}

void GameRenderer::render(float f)
{
	if (m_pMinecraft->m_pLocalPlayer && m_pMinecraft->m_bGrabbedMouse)
	{
		Minecraft *pMC = m_pMinecraft;
		pMC->m_mouseHandler.poll();

		float multPitch = -1.0f;
		float diff_field_84;

		if (pMC->getOptions()->m_invertMouse.get())
			multPitch = 1.0f;

		float mult1 = 2.0f * (0.2f + pMC->getOptions()->m_sensitivity.get() * 0.6f);
		mult1 = pow(mult1, 3);

		if (pMC->m_mouseHandler.smoothTurning())
		{
			Vec2 d = pMC->m_mouseHandler.m_delta * (4.0f * mult1);

			float old_field_84 = field_84;
			field_84 = float(field_C) + f;
			diff_field_84 = field_84 - old_field_84;
			m_smoothTurnDelta += d;

			if (diff_field_84 > 3.0f)
				diff_field_84 = 3.0f;

			if (!pMC->getOptions()->field_240)
			{
				// @TODO: untangle this code
				float v17 = d.x + field_14;
				float v18 = field_18;
				float v19 = field_1C;
				field_14 = v17;
				float v20 = mult1 * 0.25f * (v17 - v18);
				float v21 = v19 + (v20 - v19) * 0.5f;
				field_1C = v21;
				if ((v20 <= 0.0 || v20 <= v21) && (v20 >= 0.0 || v20 >= v21))
					v21 = mult1 * 0.25f * (v17 - v18);
				float v22 = d.y + field_20;
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
		}
		else
		{
			diff_field_84 = mult1;
			m_turnDelta = pMC->m_mouseHandler.m_delta;
		}

		Vec2 rot(m_turnDelta.x * diff_field_84,
			     m_turnDelta.y * diff_field_84 * multPitch);
		m_pItemInHandRenderer->turn(rot);
		pMC->m_pLocalPlayer->turn(rot);
	}

	int mouseX = -9999;
	int mouseY = -9999;
	bool bMouseData = false;

	if (m_pMinecraft->isTouchscreen())
	{
		int pointerId = Multitouch::getFirstActivePointerIdExThisUpdate();
		if (pointerId >= 0)
		{
			mouseX = int(float(Multitouch::getX(pointerId)) * Gui::InvGuiScale);
			mouseY = int(float(Multitouch::getY(pointerId)) * Gui::InvGuiScale);
			bMouseData = true;
		}
	}
	else if (m_pMinecraft->useController())
	{
		if (m_pMinecraft->m_pScreen)
		{
			m_pMinecraft->m_pScreen->controllerEvent(1, f);
		}
	}
	else
	{
		mouseX = int(Mouse::getX() * Gui::InvGuiScale);
		mouseY = int(Mouse::getY() * Gui::InvGuiScale);
		bMouseData = true;
	}

	if (m_pMinecraft->isLevelGenerated())
	{
		if (m_keepPic < 0)
		{
			renderLevel(f);
			currentShaderColor = Color::WHITE;
			currentShaderDarkColor = Color::WHITE;
			if (m_pMinecraft->getOptions()->m_hideGui.get())
			{
				if (!m_pMinecraft->m_pScreen)
					return;
			}

			m_pMinecraft->m_pGui->render(f, m_pMinecraft->m_pScreen != nullptr, mouseX, mouseY);
		}
	}
	else
	{
		_clearFrameBuffer();

		MatrixStack::Projection.getTop() = Matrix::IDENTITY;
		MatrixStack::View.getTop()       = Matrix::IDENTITY;
		MatrixStack::World.getTop()      = Matrix::IDENTITY;

		setupGuiScreen();
	}

	LocalPlayer* pLocalPlayer = m_pMinecraft->m_pLocalPlayer;
	if (pLocalPlayer && pLocalPlayer->m_pMoveInput)
		pLocalPlayer->m_pMoveInput->render(f);

	Screen* pScreen = m_pMinecraft->m_pScreen;
	if (pScreen)
	{
		mce::RenderContextImmediate::get().clearDepthStencilBuffer();
		if (bMouseData)
		{
			pScreen->handlePointerLocation(mouseX, mouseY);
			pScreen->handlePointerPressed(Mouse::getButtonState(MOUSE_BUTTON_LEFT));
		}
		pScreen->onRender(f);
	}

	if (m_pMinecraft->getOptions()->m_debugText.get())
	{
		_renderDebugOverlay(f);
	}

	int timeMs = getTimeMs();
	if (timeMs - m_lastUpdatedMS >= 1000)
	{
		m_lastUpdatedMS = timeMs;
		m_shownFPS = m_pMinecraft->getFpsIntlCounter();
		m_shownChunkUpdates = Chunk::updates;
		Chunk::updates = 0;
	}
}

void GameRenderer::tick()
{
	// Prevents underflow
	if (m_keepPic > -100)
		--m_keepPic;

	if (!m_pMinecraft->m_pLocalPlayer)
		return;
	
	if (--t_keepHitResult == 0)
		m_pMinecraft->m_hitResult.m_hitType = HitResult::NONE;

#ifndef ORIGINAL_CODE
	// Not harmless to let it underflow, but we won't anyway
	if (t_keepHitResult < -100)
		t_keepHitResult = -100;
#endif

	if (m_pMinecraft->m_mouseHandler.smoothTurning())
	{
		float x1 = powf(fabsf(m_smoothTurnDelta.x), 1.2f);
		m_turnDelta.x = x1 * 0.4f;
		if (m_smoothTurnDelta.x < 0.0f)
			m_turnDelta.x = -m_turnDelta.x;

		float x2 = powf(fabsf(m_smoothTurnDelta.y), 1.2f);
		m_turnDelta.y = x2 * 0.4f;
		if (m_smoothTurnDelta.y < 0.0f)
			m_turnDelta.y = -m_turnDelta.y;
	}

	m_smoothTurnDelta.x = 0.0f;
	m_smoothTurnDelta.y = 0.0f;
	field_30 = field_2C;
	field_38 = field_34;
	field_40 = field_3C;
	field_54 = field_50;
	field_5C = field_58;

	Mob* pMob = m_pMinecraft->m_pCameraEntity;
	if (!pMob)
	{
		pMob = m_pMinecraft->m_pCameraEntity = m_pMinecraft->m_pLocalPlayer;
	}

	field_C++;

	m_pItemInHandRenderer->tick();
}

void GameRenderer::renderWeather(float f)
{
	/*if (m_envTexturePresence == 0)
	{
		bool bLoadedSuccessfully = m_pMinecraft->m_pTextures->loadTexture("environment/snow.png", false) != nullptr;
		m_envTexturePresence = bLoadedSuccessfully ? 2 : 1;
	}
	
	if (m_envTexturePresence == 1)
		return;

	LocalPlayer* pLP = m_pMinecraft->m_pLocalPlayer;
	int bPosX = Mth::floor(pLP->m_pos.x);
	int bPosY = Mth::floor(pLP->m_pos.y);
	int bPosZ = Mth::floor(pLP->m_pos.z);
	Vec3 pos = pLP->getPos(f);
	Tesselator& t = Tesselator::instance;
	Level* pLevel = m_pMinecraft->m_pLevel;

	m_pMinecraft->m_pTextures->loadAndBindTexture("environment/snow.png");

	int range = m_pMinecraft->getOptions()->m_fancyGraphics ? 10 : 5;

	TilePos tp(bPosX - range, 128, bPosZ - range);
	for (tp.x = bPosX - range; tp.x <= bPosX + range; tp.x++)
	{
		for (tp.z = bPosZ - range; tp.z <= bPosZ + range; tp.z++)
		{
			int tsb = pLevel->getTopSolidBlock(tp);
			if (tsb < 0)
				tsb = 0;

			int minY = bPosY - range;
			int maxY = bPosY + range;

			if (minY < tsb)
				minY = tsb;
			if (maxY < tsb)
				maxY = tsb;

			float offs = 2.0f;
			if (minY == maxY)
				continue;

			m_random.setSeed(tp.x * tp.x * 3121 + tp.x * 45238971 + tp.z * tp.z * 418711 + tp.z * 13761);

			float x1 = float(m_y0) + f;
			float x2 = (float(m_y0 & 0x1FF) + f) / 512.0f;
			float x3 = m_random.nextFloat() + x1 * 0.01f * m_random.nextGaussian();
			float x4 = m_random.nextFloat() + x1 * 0.001f * m_random.nextGaussian();
			float f1 = float(tp.x + 0.5f) - pLP->m_pos.x;
			float f2 = float(tp.z + 0.5f) - pLP->m_pos.z;
			float f3 = Mth::sqrt(f1 * f1 + f2 * f2) / float(range);
			float f4 = pLevel->getBrightness(tp);
			t.begin(8);
			currentShaderColor = Color(f4, f4, f4, (1.0f - f3 * f3) * 0.7f);
			currentShaderDarkColor = Color::WHITE;
			t.setOffset(-pos.x, -pos.y, -pos.z);
			t.vertexUV(float(tp.x + 0), float(minY), float(tp.z + 0), 0.0f * offs + x3, float(minY) * offs / 8.0f + x2 * offs + x4);
			t.vertexUV(float(tp.x + 1), float(minY), float(tp.z + 1), 1.0f * offs + x3, float(minY) * offs / 8.0f + x2 * offs + x4);
			t.vertexUV(float(tp.x + 1), float(maxY), float(tp.z + 1), 1.0f * offs + x3, float(maxY) * offs / 8.0f + x2 * offs + x4);
			t.vertexUV(float(tp.x + 0), float(maxY), float(tp.z + 0), 0.0f * offs + x3, float(maxY) * offs / 8.0f + x2 * offs + x4);
			t.vertexUV(float(tp.x + 0), float(minY), float(tp.z + 1), 0.0f * offs + x3, float(minY) * offs / 8.0f + x2 * offs + x4);
			t.vertexUV(float(tp.x + 1), float(minY), float(tp.z + 0), 1.0f * offs + x3, float(minY) * offs / 8.0f + x2 * offs + x4);
			t.vertexUV(float(tp.x + 1), float(maxY), float(tp.z + 0), 1.0f * offs + x3, float(maxY) * offs / 8.0f + x2 * offs + x4);
			t.vertexUV(float(tp.x + 0), float(maxY), float(tp.z + 1), 0.0f * offs + x3, float(maxY) * offs / 8.0f + x2 * offs + x4);
			t.setOffset(0.0f, 0.0f, 0.0f);
			t.draw(); // use "snow" or "weather" material
		}
	}*/
}

void GameRenderer::renderPointer(const MenuPointer& pointer)
{
	Textures& textures = *m_pMinecraft->m_pTextures;

	MatrixStack::Ref mtx = MatrixStack::World.push();
	mtx->translate(Vec3(pointer.x, pointer.y, 0));

	if (m_pMinecraft->m_pScreen && m_pMinecraft->m_pScreen->m_uiTheme == UI_CONSOLE)
		mtx->scale(2.0f);

	mtx->translate(Vec3(-(C_MENU_POINTER_WIDTH / 2), -(C_MENU_POINTER_HEIGHT / 2), 0));

	textures.loadAndBindTexture("gui/pointer.png", true);
	m_pointerMesh.render(ScreenRenderer::singleton().m_materials.ui_textured);
}

void GameRenderer::setLevel(Level* pLevel, Dimension* pDimension)
{
	m_pLevel = pLevel;

	if (!pLevel || !pDimension)
		return;

	//_tickLightTexture(pDimension, 1.0f);
}

void GameRenderer::onGraphicsReset()
{

}

void GameRenderer::pick(float f)
{
	if (!m_pMinecraft->m_pCameraEntity || !m_pMinecraft->m_pLevel)
		return;

	Mob* pMob = m_pMinecraft->m_pCameraEntity;
	HitResult& mchr = m_pMinecraft->m_hitResult;
	float dist = m_pMinecraft->m_pGameMode->getBlockReachDistance();
	bool isFirstPerson = !m_pMinecraft->getOptions()->m_thirdPerson.get();
	Vec3 touchPosNear, touchPosFar;
	bool bUseTouchCoords = false;

	if (!m_pMinecraft->useSplitControls())
	{
		Vec3 mobPos = pMob->getPos(f);
		Vec3 foundPosNear, foundPosFar;
		bool flag = true;
		float offset = isFirstPerson ? 6.0f : 12.0f;

		if (m_pMinecraft->m_pInputHolder->allowPicking())
		{
			int viewport[4] = { 0 };
			viewport[2] = Minecraft::width;
			viewport[3] = Minecraft::height;
			float obj_coord[3] = { 0 };

			if (glhUnProjectf(m_pMinecraft->m_pInputHolder->m_feedbackX,
				              Minecraft::height - m_pMinecraft->m_pInputHolder->m_feedbackY,
				              1.0f,
				              m_mtxView.ptr(),
				              m_mtxProj.ptr(),
				              viewport,
				              obj_coord))
			{
				foundPosFar = mobPos + Vec3(obj_coord[0], obj_coord[1], obj_coord[2]);

				glhUnProjectf(m_pMinecraft->m_pInputHolder->m_feedbackX,
				              Minecraft::height - m_pMinecraft->m_pInputHolder->m_feedbackY,
				              0.0f,
				              m_mtxView.ptr(),
				              m_mtxProj.ptr(),
				              viewport,
				              obj_coord);

				foundPosNear = mobPos + Vec3(obj_coord[0], obj_coord[1], obj_coord[2]);

				Vec3 diff = foundPosFar - foundPosNear;
				Vec3 normDiff = diff.normalize();
				Vec3 normScaledDiff = normDiff.scale(offset);
				
				mobPos = foundPosNear + normScaledDiff;

				foundPosFar = mobPos;

				// For touch entity detection
				touchPosNear = foundPosNear;
				touchPosFar = foundPosFar;
				bUseTouchCoords = true;
			}
			
			// keep the hit result forever
			t_keepHitResult = -1;
		}
		else
		{
			t_keepHitResult = 1; // keep the tick result for exactly one frame
			flag = false;
		}

		if (flag)
		{
			if (isFirstPerson)
			{
				mchr = m_pMinecraft->m_pLevel->clip(foundPosNear, foundPosFar, false);
			}
			else
			{
				HitResult hr = m_pMinecraft->m_pLevel->clip(foundPosNear, foundPosFar, false);

				float diffX = float(hr.m_tilePos.x) - m_pMinecraft->m_pCameraEntity->m_pos.x;
				float diffY = float(hr.m_tilePos.y) - m_pMinecraft->m_pCameraEntity->m_pos.y;
				float diffZ = float(hr.m_tilePos.z) - m_pMinecraft->m_pCameraEntity->m_pos.z;

				if (hr.m_hitType == HitResult::NONE || diffX * diffX + diffY * diffY + diffZ * diffZ > offset * offset)
					mchr.m_hitType = HitResult::NONE;
				else
					mchr = hr;
			}
		}
	}
	else
	{
		// easy case: pick from the middle of the screen
		HitResult hrMob = pMob->pick(dist, f);
		mchr = hrMob;
	}

	Vec3 mobPos = pMob->getPos(f);

	if (mchr.m_hitType != HitResult::NONE)
		dist = mchr.m_hitPos.distanceTo(mobPos);

	float maxEntityDist = m_pMinecraft->m_pGameMode->getEntityReachDistance();
	/*if (m_pMinecraft->m_pGameMode->isCreativeType())
		dist = 7.0f;
	else */if (dist > maxEntityDist)
		dist = maxEntityDist;

	Vec3 view = pMob->getViewVector(f);
	Vec3 exp;
	Vec3 limit;
	Vec3 rayStart;
	
	// When not using split controls and touch coords are valid, use touch-based raycasting for entities
	bool shouldDetectEntities = bUseTouchCoords || m_pMinecraft->useSplitControls();
	
	if (shouldDetectEntities)
	{
		if (bUseTouchCoords)
		{
			rayStart = touchPosNear;
			Vec3 touchDir = (touchPosFar - touchPosNear).normalize();
			limit = rayStart + (touchDir * maxEntityDist);
			exp = limit - rayStart;
		}
		else
		{
			// For split controls - use center-based raycasting
			rayStart = mobPos;
			limit = mobPos + view * dist;
			exp = view * dist;
		}
	}

	m_pHovered = nullptr;

	AABB scanAABB;
	
	if (shouldDetectEntities)
	{
		if (bUseTouchCoords)
		{
			// Touch
			scanAABB.min.x = rayStart.x < limit.x ? rayStart.x : limit.x;
			scanAABB.min.y = rayStart.y < limit.y ? rayStart.y : limit.y;
			scanAABB.min.z = rayStart.z < limit.z ? rayStart.z : limit.z;
			scanAABB.max.x = rayStart.x > limit.x ? rayStart.x : limit.x;
			scanAABB.max.y = rayStart.y > limit.y ? rayStart.y : limit.y;
			scanAABB.max.z = rayStart.z > limit.z ? rayStart.z : limit.z;
			scanAABB.grow(1, 1, 1);
		}
		else
		{
			// Split
			scanAABB = pMob->m_hitbox;
			if (exp.x < 0) scanAABB.min.x += exp.x;
			if (exp.x > 0) scanAABB.max.x += exp.x;
			if (exp.y < 0) scanAABB.min.y += exp.y;
			if (exp.y > 0) scanAABB.max.y += exp.y;
			if (exp.z < 0) scanAABB.min.z += exp.z;
			if (exp.z > 0) scanAABB.max.z += exp.z;
			scanAABB.grow(1, 1, 1);
		}

		EntityVector ents = m_pMinecraft->m_pLevel->getEntities(pMob, scanAABB);

		float fDist = 0.0f;
		for (size_t i = 0; i < ents.size(); i++)
		{
			Entity *pEnt = (ents)[i];
			if (!pEnt->isPickable())
				continue;

			AABB checkAABB = pEnt->m_hitbox;
			checkAABB.grow(pEnt->getPickRadius());

			HitResult hrMobChk = checkAABB.clip(rayStart, limit);

			if (checkAABB.contains(rayStart))
			{
				if (fDist >= 0.0f)
				{
					//this is it brother
					m_pHovered = pEnt;
					fDist = 0.0f;
				}
				continue;
			}

			if (hrMobChk.m_hitType != HitResult::NONE)
			{
				float dX = hrMobChk.m_hitPos.x - rayStart.x;
				float dY = hrMobChk.m_hitPos.y - rayStart.y;
				float dZ = hrMobChk.m_hitPos.z - rayStart.z;
				float fNewDist = Mth::sqrt(dX * dX + dY * dY + dZ * dZ);

				if (fDist > fNewDist || fDist == 0.0f)
				{
					m_pHovered = pEnt;
					fDist = fNewDist;
				}
			}
		}

		// picked entities take priority over tiles (?!)
		if (m_pHovered)
		{
			mchr = HitResult(m_pHovered);
			return;
		}
	}

	if (mchr.m_hitType != HitResult::NONE || view.y >= -0.7f)
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
		mchr.m_hitSide = view.z >= 0.0f ? Facing::SOUTH : Facing::NORTH;
	}
	else
	{
		mchr.m_hitSide = view.x >= 0.0f ? Facing::EAST : Facing::WEST;
	}
}

void GameRenderer::applyTurnDelta(const Vec2& turnDelta)
{
	m_turnDelta = turnDelta;
}
