/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ProgressScreen.hpp"

bool ProgressScreen::isInGameScreen()
{
	return false;
}

void ProgressScreen::render(int a, int b, float c)
{
	if (m_pMinecraft->isLevelGenerated())
	{
		m_pMinecraft->setScreen(nullptr);
		return;
	}

	renderBackground();

	// render the dirt background
	// for some reason, this was manually written:

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");

	//! why not use the screen stuff
	int x_width  = int(Minecraft::width  * Gui::InvGuiScale);
	int x_height = int(Minecraft::height * Gui::InvGuiScale);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x404040);
	t.vertexUV(0.0f,           float(x_height), 0, 0,                      float(x_height) / 32.0f);
	t.vertexUV(float(x_width), float(x_height), 0, float(x_width) / 32.0f, float(x_height) / 32.0f);
	t.vertexUV(float(x_width), 0,               0, float(x_width) / 32.0f, 0);
	t.vertexUV(0.0f,           0,               0, 0,                      0);
	t.draw();

	int yPos = x_height / 2;

	if (m_pMinecraft->m_progressPercent >= 0)
	{
		float lX = float(x_width) / 2 - 50, rX = float(x_width) / 2 + 50;
		float prog = float(m_pMinecraft->m_progressPercent);

		// disable the texturing
		glDisable(GL_TEXTURE_2D);

		t.begin();

		t.color(0x808080); // gray background
		t.vertex(lX, float(yPos + 16), 0);
		t.vertex(lX, float(yPos + 18), 0);
		t.vertex(rX, float(yPos + 18), 0);
		t.vertex(rX, float(yPos + 16), 0);

		t.color(0x80FF80); // the green stuff
		t.vertex(lX,        float(yPos + 16), 0);
		t.vertex(lX,        float(yPos + 18), 0);
		t.vertex(lX + prog, float(yPos + 18), 0);
		t.vertex(lX + prog, float(yPos + 16), 0);

		t.draw();

		// restore old state
		glEnable(GL_TEXTURE_2D);
	}

	//! Using m_pMinecraft->m_pFont instead of m_pFont.
	Font* pFont = m_pMinecraft->m_pFont;

	int width = pFont->width("Generating world");
	pFont->drawShadow("Generating world", (x_width - width) / 2, yPos - 20, 0xFFFFFF);

	width = pFont->width(m_pMinecraft->getProgressMessage());
	pFont->drawShadow(m_pMinecraft->getProgressMessage(), (x_width - width) / 2, yPos + 4, 0xFFFFFF);

#ifdef ORIGINAL_CODE
	sleepMs(50);
#endif
}
