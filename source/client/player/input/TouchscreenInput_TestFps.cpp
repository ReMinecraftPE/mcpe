/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TouchscreenInput_TestFps.hpp"
#include "Multitouch.hpp"
#include "GameMods.hpp"
#include "client/app/Minecraft.hpp"
#include "client/options/Options.hpp"
#include "renderer/ShaderConstants.hpp"
#include "world/entity/Player.hpp"

TouchscreenInput_TestFps::TouchscreenInput_TestFps(Minecraft* pMinecraft, Options* pOptions)
	: m_rectArea(0.0f, 0.0f, 1.0f, 1.0f)
	, m_pOptions(pOptions)
	, m_bForwardBeingHeld(false)
	, m_bJumpBeingHeld(false)
	, m_pMinecraft(pMinecraft)
	, m_pAreaLeft(nullptr)
	, m_pAreaRight(nullptr)
	, m_pAreaForward(nullptr)
	, m_pAreaBackward(nullptr)
	, m_pAreaJump(nullptr)
#ifdef ENH_NEW_TOUCH_CONTROLS
	, m_pAreaForwardLeft(nullptr)
	, m_pAreaForwardRight(nullptr)
#endif
{
	for (int i = 0; i < 10; i++)
		field_30[i] = 0;

	// Note! Only the first five button entries are used.
	for (int i = 0; i < 8; i++)
		field_6C[i] = 0;

	setScreenSize(Minecraft::width, Minecraft::height);
}

void TouchscreenInput_TestFps::releaseAllKeys()
{
	m_horzInput = 0.0f;
	m_vertInput = 0.0f;
	for (int i = 0; i < 8; i++)
		field_6C[i] = false;
}

void TouchscreenInput_TestFps::setKey(int eventKey, bool eventKeyState)
{
}

static void CopyCoordinateArray(int count, float* xs, float* ys, float* xd, float* yd)
{
	for (int i = 0; i < count; i++)
	{
		xd[i] = xs[i];
		yd[i] = ys[i];
	}
}

static void MultiplyCoordinateArray(int count, float* x, float* y, float xm, float ym)
{
	for (int i = 0; i < count; i++)
	{
		x[i] *= xm;
		y[i] *= ym;
	}
}

static void AddCoordinateArray(int count, float* x, float* y, float xd, float yd)
{
	for (int i = 0; i < count; i++)
	{
		x[i] += xd;
		y[i] += yd;
	}
}

static void TransformArray(int count, float* x1, float* y1, float* x2, float* y2, float xd, float yd, float xm, float ym)
{
	CopyCoordinateArray(count, x1, y1, x2, y2);
	MultiplyCoordinateArray(count, x2, y2, xm, ym);
	AddCoordinateArray(count, x2, y2, xd, yd);
}

void TouchscreenInput_TestFps::setScreenSize(int width, int height)
{
	m_touchAreaModel.clear();

	float widthM = float(width) * 0.094f;
	float heightM = float(width) * 0.094f;

	float x1[4], y1[4], x2[4], y2[4];

	x1[0] = 0.0f; x1[1] = widthM; x1[2] = widthM;  x1[3] = 0.0f;
	y1[0] = 0.0f; y1[1] = 0.0f;   y1[2] = heightM; y1[3] = heightM;
	x2[0] = 0.0f; x2[1] = 0.0f;   x2[2] = 0.0f;    x2[3] = 0.0f;
	y2[0] = 0.0f; y2[1] = 0.0f;   y2[2] = 0.0f;    y2[3] = 0.0f;

	float rx1, rx2, ry1, ry2;
	float offX = 8.0f;
	rx1 = 0.0f;
	ry1 = height - 8.0f - heightM * 3.0f;
	rx2 = widthM * 3.0f + 8.0f;
	ry2 = ry1 + heightM * 3.0f;

	m_rectArea = RectangleArea(rx1, ry1, rx2, ry2);

	float middleX = offX + widthM;
	float middleY = ry1 + heightM;

	TransformArray(4, x1, y1, x2, y2, middleX, middleY - heightM, 1.0f, 1.0f);
	m_pAreaForward = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_FORWARD, m_pAreaForward);

	TransformArray(4, x1, y1, x2, y2, middleX, middleY, 1.0f, 1.0f);
	m_pAreaJump = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_JUMP, m_pAreaJump);

	TransformArray(4, x1, y1, x2, y2, middleX, middleY + heightM, 1.0f, 1.0f);
	m_pAreaBackward = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_BACKWARD, m_pAreaBackward);

	TransformArray(4, x1, y1, x2, y2, middleX - widthM, ry1 + heightM, 1.0f, 1.0f);
	m_pAreaLeft = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_LEFT, m_pAreaLeft);

	TransformArray(4, x1, y1, x2, y2, middleX + widthM, ry1 + heightM, 1.0f, 1.0f);
	m_pAreaRight = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_RIGHT, m_pAreaRight);

#ifdef ENH_NEW_TOUCH_CONTROLS
	TransformArray(4, x1, y1, x2, y2, middleX - widthM, middleY - heightM, 1.0f, 1.0f);
	m_pAreaForwardLeft = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_FORWARDLEFT, m_pAreaForwardLeft);

	TransformArray(4, x1, y1, x2, y2, middleX + widthM, middleY - heightM, 1.0f, 1.0f);
	m_pAreaForwardRight = new PolygonArea(4, x2, y2);
	m_touchAreaModel.addArea(100 + INPUT_FORWARDRIGHT, m_pAreaForwardRight);
#endif
	// NOTE: We are not leaking memory! Since by default IArea's constructor sets
	// m_vertices to true, TouchAreaModel owns the pointers, so when it's destroyed,
	// so are these areas we allocated.
}

void TouchscreenInput_TestFps::tick(Player* pPlayer)
{
	if (m_pMinecraft->m_pScreen) return; // If we're in another screen, disable d-pad
	
	m_horzInput = 0.0f;
	m_vertInput = 0.0f;
	m_bJumping = false;
	m_bFlyUp = false;
	m_bSneaking = false;
	m_bForwardBeingHeld = false;

	for (int i = 0; i < 8; i++)
		field_6C[i] = false;
	
	const int* activePointers;
	int activePointerCount = Multitouch::getActivePointerIds(&activePointers);

	bool bJumpPressed = false, bForwardPressed = false;
	bool flyUpPressed = false;
	bool flyDownPressed = false;
	bool flyJumpTap = false;
	bool flying = m_pOptions->m_flightHax.get() || m_pMinecraft->m_pLocalPlayer->m_bFlying;

	for (int i = 0; i < activePointerCount; i++)
	{
		int finger = activePointers[i];
		int x = Multitouch::getX(finger);
		int y = Multitouch::getY(finger);
		int pointerId = m_touchAreaModel.getPointerId(x, y, finger);

		if (pointerId > 99)
			field_6C[pointerId - 100] = true;

		if (pointerId == 100 + INPUT_SNEAK) // Unused
		{
			if (pPlayer->isInWater())
				m_bJumping = true;
			else
				bJumpPressed = true;

			pointerId = 100; // forward
		}

		if (pointerId == 100 + INPUT_JUMP) // jump
		{
			if (pPlayer->isInWater())
				m_bJumping = true;
			else if (flying && Multitouch::isPressed(finger))
				flyJumpTap = true;
			else if (Multitouch::isPressed(finger))
				m_bJumping = true;
			else if (m_bForwardBeingHeld && !flying)
			{
				pointerId = 100; // forward
				m_vertInput += 1.0f;
			}
			bJumpPressed = true;
		}

		switch (pointerId)
		{
			case 100 + INPUT_FORWARD:
				if (m_bJumpBeingHeld && flying)
				{
					m_bJumpBeingHeld = true;
					m_bWasJumping = false;
					bJumpPressed = true;
					flyUpPressed = true;
				} 
				else
				{
					if (pPlayer->isInWater())
						m_bJumping = true;
					else
						bForwardPressed = true;

					m_vertInput += 1.0f;
				}
				break;

			case 100 + INPUT_BACKWARD:
				if (m_bJumpBeingHeld && flying)
				{
					m_bJumpBeingHeld = true;
					m_bWasJumping = false;
					bJumpPressed = true;
					flyDownPressed = true;
				} 
				else
				{
					m_vertInput -= 1.0f;
				}
				break;

			case 100 + INPUT_FORWARDLEFT:
				if (m_bForwardBeingHeld)
				{
					bForwardPressed = true;
					m_vertInput += 1.0f;
					m_horzInput += 1.0f;
				}
				break;
			case 100 + INPUT_FORWARDRIGHT:
				if (m_bForwardBeingHeld)
				{
					bForwardPressed = true;
					m_vertInput += 1.0f;
					m_horzInput -= 1.0f;
				}
				break;


			case 100 + INPUT_LEFT:
				m_horzInput += 1.0f;
				break;

			case 100 + INPUT_RIGHT:
				m_horzInput -= 1.0f;
				break;
		}

		IMoveInput::tick(pPlayer);
	}

	m_bForwardBeingHeld = bForwardPressed;

	if (bJumpPressed)
	{
		// Don't allow the player to hold jump to repeatedly jump.
		// Only let them jump once - have them jump again
		if (!m_bJumpBeingHeld)
			m_bJumping = true;
		m_bJumpBeingHeld = true;

		if (m_bWasJumping && m_pMinecraft->m_pGameMode->isCreativeType())
		{
			m_bWasJumping = false;
			m_bJumpBeingHeld = false;
		}

	}
	else
	{
		if (m_bJumpBeingHeld)
			m_bWasJumping = true;
		else
			m_bWasJumping = false;

		m_bJumpBeingHeld = false;
	}

	if (flying)
	{
		m_bJumping = flyJumpTap;
		m_bFlyUp = flyUpPressed;
		m_bSneaking = flyDownPressed;
	}
}

static void RenderTouchButton(Tesselator* t, PolygonArea* pArea, int srcX, int srcY)
{
	float tc[8];

	tc[0] = float(srcX) / 256.0f;
	tc[1] = float(srcY) / 256.0f;
#ifdef ENH_NEW_TOUCH_CONTROLS
	tc[2] = tc[0] + 26.0f / 256.0f;
#else
	tc[2] = tc[0] + 64.0f / 256.0f;
#endif
	tc[3] = tc[1];
	tc[4] = tc[2];
#ifdef ENH_NEW_TOUCH_CONTROLS
	tc[5] = tc[1] + 26.0f / 256.0f;
#else
	tc[5] = tc[1] + 64.0f / 256.0f;
#endif
	tc[6] = tc[0];
	tc[7] = tc[5];

	for (int i = 0; i < pArea->m_count; i++)
	{
		t->vertexUV(
			Gui::GuiScale * pArea->m_xPos[i],
			Gui::GuiScale * pArea->m_yPos[i],
			0.0f,
			tc[(2 * i) % 8],
			tc[(2 * i + 1) % 8]
		);
	}
}

void TouchscreenInput_TestFps::render(float f)
{
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/gui2.png");

    currentShaderColor = Color::WHITE;
    currentShaderDarkColor = Color::WHITE;

	bool flying = m_pOptions->m_flightHax.get() || m_pMinecraft->m_pLocalPlayer->m_bFlying;
	int horizontalAlpha = (flying && (m_bFlyUp || m_bJumpBeingHeld || m_bSneaking)) ? 0x20 : 0x80;
    
	Tesselator& t = Tesselator::instance;
	t.begin(0);
#ifdef ENH_NEW_TOUCH_CONTROLS
	if (m_bForwardBeingHeld && !isButtonDown(100 + INPUT_JUMP)) 
	{
		t.color(isButtonDown(100 + INPUT_FORWARDLEFT) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaForwardLeft, 0, 132);
		
		t.color(isButtonDown(100 + INPUT_FORWARDRIGHT) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaForwardRight, 26, 132);
	}

	t.color(isButtonDown(100 + INPUT_LEFT) ? 0xC0C0C0 : 0xFFFFFF, horizontalAlpha);
	RenderTouchButton(&t, m_pAreaLeft, 26, 106);

	t.color(isButtonDown(100 + INPUT_RIGHT) ? 0xC0C0C0 : 0xFFFFFF, horizontalAlpha);
	RenderTouchButton(&t, m_pAreaRight, 78, 106);

	t.color(isButtonDown(100 + INPUT_JUMP) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	(flying) ?
		RenderTouchButton(&t, m_pAreaJump, 104, 132) : RenderTouchButton(&t, m_pAreaJump, 104, 106);

	if (flying && m_bJumpBeingHeld) 
	{
		t.color(isButtonDown(100 + INPUT_FORWARD) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaForward, 52, 132);

		t.color(isButtonDown(100 + INPUT_BACKWARD) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaBackward, 78, 132);
	}
	else 
	{
		t.color(isButtonDown(100 + INPUT_FORWARD) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaForward, 0, 106);

		t.color(isButtonDown(100 + INPUT_BACKWARD) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaBackward, 52, 106);
	}
	// @TODO: implement double tap sneak button
	/*if (m_bSneaking)
	{
		// toggled on
		t.color(m_bSneaking ? 0xFFFFFF : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaSneak, 0, 0); // @TODO: Find sneaking button texture coords
	}
	else
	{
		// toggled off
		t.color(m_bSneaking ? 0xFFFFFF : 0xFFFFFF, 0x80);
		RenderTouchButton(&t, m_pAreaSneak, 0, 0); // @TODO: Find sneaking button texture coords
	}*/
#else
	// Original pocket edition buttons
	t.color(isButtonDown(100 + INPUT_LEFT) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaLeft, 64, 112);

	t.color(isButtonDown(100 + INPUT_RIGHT) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaRight, 192, 112);

	t.color(isButtonDown(100 + INPUT_FORWARD) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaForward, 0, 112);

	t.color(isButtonDown(100 + INPUT_BACKWARD) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaBackward, 128, 112);

	t.color(isButtonDown(100 + INPUT_JUMP) ? 0xC0C0C0 : 0xFFFFFF, 0x80);
	RenderTouchButton(&t, m_pAreaJump, 0, 176);
#endif
	t.draw(m_materials.ui_texture_and_color_nocull);
}

RectangleArea TouchscreenInput_TestFps::getRectangleArea()
{
	return m_rectArea;
}

bool TouchscreenInput_TestFps::isButtonDown(int key)
{
	if (key == 7) // mp
		return m_bJumpBeingHeld;
	return field_6C[key - 100];
}
