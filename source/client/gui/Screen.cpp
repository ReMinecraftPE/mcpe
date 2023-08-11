/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Screen.hpp"

Screen::Screen()
{
	m_width = 1;
	m_height = 1;
	field_10 = false;
	m_tabButtonIndex = 0;
	m_pClickedButton = 0;
}

Screen::~Screen()
{
	m_pClickedButton = nullptr;
	m_buttons.clear();
}

void Screen::init(Minecraft* pMinecraft, int a3, int a4)
{
	m_width  = a3;
	m_height = a4;
	m_pMinecraft = pMinecraft;
	m_pFont = pMinecraft->m_pFont;
	init();
	updateTabButtonSelection();
}

void Screen::init()
{

}

void Screen::buttonClicked(Button* pButton)
{

}

void Screen::confirmResult(bool b, int i)
{

}

bool Screen::handleBackEvent(bool b)
{
	return false;
}

bool Screen::isPauseScreen()
{
	return true;
}

bool Screen::isErrorScreen()
{
	return false;
}

bool Screen::isInGameScreen()
{
	return true;
}

void Screen::keyPressed(int key)
{
	if (key == AKEYCODE_MENU)//escape
	{
		m_pMinecraft->setScreen(nullptr);
	}

	if (m_buttonTabList.size())
	{
#ifndef ENH_HIGHLIGHT_BY_HOVER
		if (m_pMinecraft->m_options.m_keyBinds[Options::MENU_NEXT].value == key)
		{
			m_tabButtonIndex++;
			if (m_tabButtonIndex == int(m_buttonTabList.size()))
				m_tabButtonIndex = 0;
		}
		if (m_pMinecraft->m_options.m_keyBinds[Options::MENU_PREVIOUS].value == key)
		{
			m_tabButtonIndex--;
			if (m_tabButtonIndex == -1)
				m_tabButtonIndex = int(m_buttonTabList.size() - 1);
		}
		if (m_pMinecraft->m_options.m_keyBinds[Options::MENU_OK].value == key)
		{
			if (m_buttonTabList[m_tabButtonIndex]->m_bEnabled)
			{
				m_pMinecraft->m_pSoundEngine->play("random.click");
				buttonClicked(m_buttonTabList[m_tabButtonIndex]);
			}
		}

		updateTabButtonSelection();
#endif
	}

	for (auto textInput : m_textInputs)
	{
		textInput->keyPressed(m_pMinecraft, key);
	}
}

void Screen::charInput(char chr)
{
	for (auto textInput : m_textInputs)
		textInput->charPressed(chr);
}

static const char* g_panoramaList[] =
{
	"gui/background/panorama_0.png",
	"gui/background/panorama_1.png",
	"gui/background/panorama_2.png",
	"gui/background/panorama_3.png",
	"gui/background/panorama_4.png",
	"gui/background/panorama_5.png",
};

static float g_panoramaAngle = 0.0f;
bool g_bIsMenuBackgroundAvailable = false;

void Screen::renderMenuBackground(float f)
{
	if (!g_bIsMenuBackgroundAvailable)
	{
		renderDirtBackground(0);
		return;
	}

	g_panoramaAngle += float(30.0 * m_pMinecraft->m_fDeltaTime);

	// not in 0.8
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(120.0f, 1.0f, 0.05f, 10.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(Mth::sin((f + g_panoramaAngle) / 400.0f) * 25.0f + 20.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-0.1f * (f + g_panoramaAngle), 0.0f, 1.0f, 0.0f);

	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();

		float xm = 0.0f, ym = 0.0f, ang = 0.0f;
		switch (i)
		{
			case 1:
				ang = 90.0f;
				xm = 0.0f;
				ym = 1.0f;
				break;
			case 2:
				ang = 180.0f;
				xm = 0.0f;
				ym = 1.0f;
				break;
			case 3:
				ang = -90.0f;
				xm = 0.0f;
				ym = 1.0f;
				break;
			case 4:
				ang = 90.0f;
				ym = 0.0f;
				xm = 1.0f;
				break;
			case 5:
				ang = -90.0f;
				ym = 0.0f;
				xm = 1.0f;
				break;
		}

		glRotatef(ang, xm, ym, 0.0f);

		m_pMinecraft->m_pTextures->setSmoothing(true);
		m_pMinecraft->m_pTextures->setClampToEdge(true);
		m_pMinecraft->m_pTextures->loadAndBindTexture(std::string(g_panoramaList[i]));
		m_pMinecraft->m_pTextures->setSmoothing(false);
		m_pMinecraft->m_pTextures->setClampToEdge(false);

		Tesselator& t = Tesselator::instance;
		t.begin();
		t.vertexUV(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f);
		t.vertexUV(+1.0f, -1.0f, 1.0f, 1.0f, 0.0f);
		t.vertexUV(+1.0f, +1.0f, 1.0f, 1.0f, 1.0f);
		t.vertexUV(-1.0f, +1.0f, 1.0f, 0.0f, 1.0f);
		t.draw();

		glPopMatrix();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	fillGradient(0, 0, m_width, m_height, 0x89000000, 0x89FFFFFF);
}

void Screen::mouseClicked(int xPos, int yPos, int d) // d = clicked?
{
	if (!d) return;

	for (auto button : m_buttons)
	{
		if (button->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pClickedButton = button;
			m_pMinecraft->m_pSoundEngine->play("random.click");

			buttonClicked(button);
		}
	}

#ifndef ORIGINAL_CODE
	for (auto textInput : m_textInputs)
	{
		textInput->onClick(xPos, yPos);
	}
#endif
}

void Screen::mouseReleased(int xPos, int yPos, int d)
{
	if (!d) return;

	if (m_pClickedButton)
	{
		m_pClickedButton->released(xPos, yPos);
		m_pClickedButton = nullptr;
	}
}

void Screen::render(int xPos, int yPos, float unused)
{
	for (auto button : m_buttons)
	{
		button->render(m_pMinecraft, xPos, yPos);
	}

#ifndef ORIGINAL_CODE
	for (auto textInput : m_textInputs)
	{
		textInput->tick();
		textInput->render();
	}
#endif
}

void Screen::tick()
{
	g_panoramaAngle++;
}

void Screen::removed()
{

}

void Screen::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	// not original code. Will need to re-init again
	m_buttons.clear();
	m_textInputs.clear();
	init();
}

void Screen::updateEvents()
{
	if (field_10) return;

	while (Mouse::next())
		mouseEvent();

	while (Keyboard::next())
		keyboardEvent();
}

void Screen::keyboardEvent()
{
	if (Keyboard::getEventKeyState())
		keyPressed(Keyboard::getEventKey());
}

void Screen::mouseEvent()
{
	MouseAction* pAction = Mouse::getEvent();
	if (pAction->isButton())
	{
		if (Mouse::getEventButtonState())
			mouseClicked (m_width * pAction->field_8 / Minecraft::width, m_height * pAction->field_C / Minecraft::height - 1, Mouse::getEventButton());
		else
			mouseReleased(m_width * pAction->field_8 / Minecraft::width, m_height * pAction->field_C / Minecraft::height - 1, Mouse::getEventButton());
	}
}

void Screen::renderBackground(int unk)
{
	if (m_pMinecraft->isLevelGenerated())
	{
		fillGradient(0, 0, m_width, m_height, 0xC0101010, 0xD0101010);
	}
	else
	{
		renderDirtBackground(unk);
	}
}

void Screen::renderBackground()
{
	renderBackground(0);
}

void Screen::renderDirtBackground(int unk)
{
	glDisable(GL_FOG);

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");
	glColor4f(1, 1, 1, 1);

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.color(0x404040);
	t.vertexUV(0.0f,           float(m_height), 0, 0,                      float(unk) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), float(m_height), 0, float(m_width) / 32.0f, float(unk) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), 0,               0, float(m_width) / 32.0f, float(unk) + 0.0f);
	t.vertexUV(0.0f,           0,               0, 0,                      float(unk) + 0.0f);
	t.draw();
}


void Screen::updateTabButtonSelection()
{
#ifndef ENH_HIGHLIGHT_BY_HOVER
	for (int i = 0; i < int(m_buttonTabList.size()); i++)
	{
		m_buttonTabList[i]->field_36 = m_tabButtonIndex == i;
	}
#endif
}
