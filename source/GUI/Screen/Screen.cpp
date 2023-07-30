/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Screen.hpp"

Screen::Screen()
{
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
	if (key == '\x1B')//escape
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

#ifndef ORIGINAL_CODE
	for (auto textInput : m_textInputs)
	{
		textInput->keyPressed(m_pMinecraft, key);
	}
#endif
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
}

void Screen::removed()
{

}

void Screen::setSize(int width, int height)
{
	m_width = width;
	m_height = height;
}

void Screen::updateEvents()
{
	if (field_10) return;

	for (int i = Mouse::_index + 1; i<int(Mouse::_inputs.size()); i++)
	{
		Mouse::_index = i;
		mouseEvent();
	}

	for (int i = Keyboard::_index + 1; i<int(Keyboard::_inputs.size()); i++)
	{
		Keyboard::_index = i;
		keyboardEvent();
	}
}

void Screen::keyboardEvent()
{
	// @UB: This probably behaves in an unexpected way if _inputs is empty

#ifndef ORIGINAL_CODE
	if (Keyboard::_inputs.empty() || Keyboard::_index < 0)
		return;
#endif

	if (Keyboard::_inputs[Keyboard::_index].field_0)
		keyPressed(Keyboard::_inputs[Keyboard::_index].field_4);
}

void Screen::mouseEvent()
{
	MouseInput& inp = Mouse::_inputs[Mouse::_index];

	if (1 <= inp.field_0 && inp.field_0 <= 2)
	{
		if (inp.field_4 == 1)
			mouseClicked(m_width * Mouse::_x / Minecraft::width, m_height * Mouse::_y / Minecraft::height - 1, inp.field_0);
		else
			mouseReleased(m_width * Mouse::_x / Minecraft::width, m_height * Mouse::_y / Minecraft::height - 1, inp.field_0);
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
	t.vertexUV(0.0f,           float(m_height), 0, 0,                                   float(unk) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), float(m_height), 0, float(unk) + float(m_width) / 32.0f, float(unk) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), 0,               0, float(unk) + float(m_width) / 32.0f, 0);
	t.vertexUV(0.0f,           0,               0, 0,                                   0);
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
