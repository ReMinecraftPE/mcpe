/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "IMoveInput.hpp"
#include "client/gui/GuiComponent.hpp"
#include "RectangleArea.hpp"
#include "PolygonArea.hpp"
#include "TouchAreaModel.hpp"

class Minecraft;
class Options;

class TouchscreenInput_TestFps : public IMoveInput, public GuiComponent
{
public:
	TouchscreenInput_TestFps(Minecraft*, Options*);

	// IMoveInput
	void releaseAllKeys() override;
	void setKey(int eventKey, bool eventKeyState) override;
	void setScreenSize(int width, int height) override;
	void tick(Player*) override;
	void render(float f) override;

	RectangleArea getRectangleArea();
	bool isButtonDown(int key);

private:
	RectangleArea m_rectArea;
	bool field_30[10];
	Options* m_pOptions;
	bool field_40;
	bool m_bJumpBeingHeld;
	TouchAreaModel m_touchAreaModel;
	Minecraft* m_pMinecraft;
	PolygonArea* m_pAreaLeft;
	PolygonArea* m_pAreaRight;
	PolygonArea* m_pAreaForward;
	PolygonArea* m_pAreaBackward;
	PolygonArea* m_pAreaJump;
	PolygonArea* m_pAreaForwardLeft;
	PolygonArea* m_pAreaForwardRight;
	bool field_6C[8];
};

