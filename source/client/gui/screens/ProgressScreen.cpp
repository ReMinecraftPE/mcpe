/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ProgressScreen.hpp"
#include "client/app/Minecraft.hpp"

bool ProgressScreen::isInGameScreen()
{
	return false;
}

void ProgressScreen::render(float f)
{
	renderDirtBackground(0);

	int yPos = m_height / 2;

	if (m_pMinecraft->m_progressPercent >= 0)
	{
		float lX = float(m_width) / 2 - 50, rX = float(m_width) / 2 + 50;
		float prog = float(m_pMinecraft->m_progressPercent);

		// gray background
		fill(lX, yPos + 16.0f, rX, yPos + 18.0f, 0xFF808080);
		// the green stuff
		fill(lX, yPos + 16.0f, lX + prog, yPos + 18.0f, 0xFF80FF80);
	}

	//! Using m_pMinecraft->m_pFont instead of m_pFont.
	Font* pFont = m_pMinecraft->m_pFont;

	int width = pFont->width("Generating world");
	pFont->drawShadow("Generating world", (m_width - width) / 2, yPos - 20, 0xFFFFFF);

	width = pFont->width(m_pMinecraft->getProgressMessage());
	pFont->drawShadow(m_pMinecraft->getProgressMessage(), (m_width - width) / 2, yPos + 4, 0xFFFFFF);

#ifdef ORIGINAL_CODE
	sleepMs(50);
#endif
}

void ProgressScreen::updateEvents()
{
	if (m_pMinecraft->isLevelGenerated())
	{

		m_pMinecraft->setScreen(nullptr);
		return;
	}

	Screen::updateEvents();
}