/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CreditsScreen.hpp"
#include "StartMenuScreen.hpp"

CreditsScreen::CreditsScreen() :
	m_btnBack(3, "Back")
{
}

CreditsScreen::~CreditsScreen()
{
}

void CreditsScreen::init()
{
	m_btnBack.m_yPos   = m_height - 28;
	m_btnBack.m_width  = 84;
	m_btnBack.m_height = 24;

	m_btnBack.m_xPos = m_width / 2 - m_btnBack.m_width / 2;

	m_buttons.push_back(&m_btnBack);

	m_buttonTabList.push_back(&m_btnBack);

	std::istringstream credits_stream(m_pMinecraft->m_pPlatform->readAssetFileStr("credits.txt", false));
	if (credits_stream.str().empty())
		credits_stream.str("Failed to load credits.txt");

	std::string line;
	while (std::getline(credits_stream, line))
		m_credits.push_back(line);
}

bool CreditsScreen::isInGameScreen()
{
	return true;
}

void CreditsScreen::keyPressed(int code)
{
	Screen::keyPressed(code);
}

void CreditsScreen::tick()
{
}

void CreditsScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();

	// Add dark background
	Tesselator& t = Tesselator::instance;
	float widthf = float(m_width);
	float heightf = float(m_height);
	float heightsub32 = heightf - 32;
	t.begin(4);
	t.color(0x505050, 255);
	t.vertexUV(0.0f, heightf, 0.0f, 0.0f, heightf / 32.0f);
	t.vertexUV(widthf, heightf, 0.0f, widthf / 32.0f, heightf / 32.0f);
	t.color(0x505050, 255);
	t.vertexUV(widthf, heightsub32, 0.0f, widthf / 32.0f, heightsub32 / 32.0f);
	t.vertexUV(0.0f, heightsub32, 0.0f, 0.0f, heightsub32 / 32.0f);
	t.color(0x202020);
	t.vertexUV(0.0f, heightsub32, 0.0f, 0.0f, heightsub32 / 32.0f);
	t.vertexUV(widthf, heightsub32, 0.0f, widthf / 32.0f, heightsub32 / 32.0f);
	t.vertexUV(widthf, 26.0f, 0.0f, widthf / 32.0f, 26.0f / 32.0f);
	t.vertexUV(0.0f, 26.0f, 0.0f, 0.0f, 26.0f / 32.0f);
	t.draw(m_materials.ui_texture_and_color);

	Screen::render(mouseX, mouseY, f);

	drawCenteredString(*m_pMinecraft->m_pFont, "Credits", m_width / 2, 8, Color::WHITE);

	int lineheight = 10;
	int height = (m_height - (m_credits.size() * lineheight)) / 2 - 2;
	for (size_t i = 0; i < m_credits.size(); ++i) {
		drawCenteredString(*m_pMinecraft->m_pFont, m_credits[i], m_width / 2, height, Color::WHITE);
		height += lineheight;
	}
}

bool CreditsScreen::handleBackEvent(bool b)
{
	if (b)
		return true;

	m_pMinecraft->setScreen(new StartMenuScreen);
	return true;
}

void CreditsScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnBack.m_buttonId)
		m_pMinecraft->setScreen(new StartMenuScreen);
}
