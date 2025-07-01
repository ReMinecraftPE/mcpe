/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Screen.hpp"

bool Screen::_isPanoramaAvailable = false;

Screen::Screen()
{
	m_width = 1;
	m_height = 1;
	field_10 = false;
	m_tabButtonIndex = 0;
	m_pClickedButton = 0;
	m_yOffset = -1;
	m_pFont = nullptr;
	m_pMinecraft = nullptr;
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

void Screen::keyPressed(int key)
{
	if (m_pMinecraft->getOptions()->isKey(KM_MENU_CANCEL, key))
	{
		m_pMinecraft->handleBack(false);
	}

	if (m_pMinecraft->useController() && m_buttonTabList.size() > 0)
	{
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_NEXT, key))
		{
			m_tabButtonIndex++;
			if (m_tabButtonIndex == int(m_buttonTabList.size()))
				m_tabButtonIndex = 0;
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_PREVIOUS, key))
		{
			m_tabButtonIndex--;
			if (m_tabButtonIndex == -1)
				m_tabButtonIndex = int(m_buttonTabList.size() - 1);
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_OK, key))
		{
			if (m_buttonTabList[m_tabButtonIndex]->m_bEnabled)
			{
				m_pMinecraft->m_pSoundEngine->playUI("random.click");
				buttonClicked(m_buttonTabList[m_tabButtonIndex]);
			}
		}

		updateTabButtonSelection();
	}
	
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->keyPressed(key);
	}
}

void Screen::keyboardNewChar(char chr)
{
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->charPressed(chr);
	}
}

void Screen::handleScroll(bool down)
{
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

void Screen::renderMenuBackground(float f)
{
	if (!m_pMinecraft->getOptions()->m_bMenuPanorama)
	{
		renderDirtBackground(0);
		return;
	}

	g_panoramaAngle += float(30.0 * m_pMinecraft->m_fDeltaTime);

	float aspectRatio;

	aspectRatio = 1.0f;
	//aspectRatio = float(m_width) / float(m_height);

	// not in 0.8
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(120.0f, aspectRatio, 0.05f, 10.0f);
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
			default:
				goto skip_rotate;
		}

		glRotatef(ang, xm, ym, 0.0f);

	skip_rotate:
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
	
	for (int i = 0; i < int(m_buttons.size()); i++)
	{
		Button* button = m_buttons[i];
		if (button->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pClickedButton = button;

			if (!m_pMinecraft->isTouchscreen())
			{
				m_pMinecraft->m_pSoundEngine->playUI("random.click");
				buttonClicked(button);
			}
		}
	}

#ifndef ORIGINAL_CODE
	// Iterate over focused text inputs first. This is because if changing
	// focus, the previous focus must hide the OSK, before the new focus
	// shows it.
	for (int phase = 0; phase < 2; phase++)
	{
		bool handleFocused = phase == 0;
		for (int i = 0; i < int(m_textInputs.size()); i++)
		{
			TextInputBox* textInput = m_textInputs[i];
			if (textInput->m_bFocused == handleFocused)
			{
				textInput->onClick(xPos, yPos);
			}
		}
	}

#ifdef USE_NATIVE_ANDROID
	// if the keyboard is shown:
	if (m_pMinecraft->platform()->getKeyboardUpOffset())
	{
		// if there are none focused at the moment:
		bool areAnyFocused = false;
		for (int i = 0; i < int(m_textInputs.size()); i++)
		{
			TextInputBox* textInput = m_textInputs[i];
			if (textInput->m_bFocused)
			{
				areAnyFocused = true;
				break;
			}
		}

		if (!areAnyFocused)
			m_pMinecraft->platform()->hideKeyboard();
	}
#endif
#endif
}

void Screen::mouseReleased(int xPos, int yPos, int d)
{
	if (!d) return;

	if (m_pClickedButton)
	{
		if (m_pMinecraft->isTouchscreen() && m_pClickedButton->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pMinecraft->m_pSoundEngine->playUI("random.click");
			buttonClicked(m_pClickedButton);
		}
		m_pClickedButton->released(xPos, yPos);
		m_pClickedButton = nullptr;
	}
}

void Screen::render(int xPos, int yPos, float unused)
{
	for (int i = 0; i < int(m_buttons.size()); i++)
	{
		Button* button = m_buttons[i];
		button->render(m_pMinecraft, xPos, yPos);
	}

#ifndef ORIGINAL_CODE
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->tick();
		textInput->render();
	}
#endif
}

void Screen::tick()
{
	g_panoramaAngle++;
}

void Screen::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	// not original code. Will need to re-init again
	m_buttons.clear();
	m_buttonTabList.clear();
	m_textInputs.clear();
	init();
}

void Screen::onRender(int mouseX, int mouseY, float f)
{
	m_yOffset = getYOffset();
	if (m_yOffset != 0) {
		// push the entire screen up
		glPushMatrix();
		glTranslatef(0.0f, -float(m_yOffset), 0.0f);
	}

	render(mouseX, mouseY, f);

	if (m_yOffset != 0)
		glPopMatrix();
}

int Screen::getYOffset()
{
#ifdef USE_NATIVE_ANDROID
	int keybOffset = m_pMinecraft->platform()->getKeyboardUpOffset();
	if (!keybOffset)
		return 0;

	int offset = 0;

	// look through every text box, see if one's open
	// and determine its offset from there
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* pBox = m_textInputs[i];

		if (!pBox->m_bFocused)
			continue;
		
		int heightLeft = m_height - int(float(keybOffset) * Gui::InvGuiScale);

		// we want to keep the center of the text box in the center of the screen
		int textCenterY = pBox->m_yPos + pBox->m_height / 2;
		int scrnCenterY = heightLeft / 2;

		int diff = textCenterY - scrnCenterY;
		
		// Prevent the difference from revealing the outside of the screen.
		if (diff > m_height - heightLeft)
			diff = m_height - heightLeft;
		if (diff < 0)
			diff = 0;

		offset = diff;
		break;
	}

	return offset;
#else
	return 0;
#endif
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
			mouseClicked (m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset(), Mouse::getEventButton());
		else
			mouseReleased(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset(), Mouse::getEventButton());
	}
	if (pAction->_buttonType == BUTTON_SCROLLWHEEL)
		handleScroll(Mouse::getEventButtonState());
}

void Screen::renderBackground(int unk)
{
	if (m_pMinecraft->isLevelGenerated())
	{
		// draw the background offset by the Y offset so that the smaller virtual
		// keyboards don't reveal undrawn areas
		fillGradient(0, m_yOffset, m_width, m_height, 0xC0101010, 0xD0101010);
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
	t.offset(0, m_yOffset, 0);
	t.color(0x404040);
	t.vertexUV(0.0f,           float(m_height), 0, 0,                      float(unk) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), float(m_height), 0, float(m_width) / 32.0f, float(unk) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), 0,               0, float(m_width) / 32.0f, float(unk) + 0.0f);
	t.vertexUV(0.0f,           0,               0, 0,                      float(unk) + 0.0f);
	t.offset(0, 0, 0);
	t.draw();
}


void Screen::updateTabButtonSelection()
{
	if (m_pMinecraft->useController())
	{
		for (int i = 0; i < int(m_buttonTabList.size()); i++)
		{
			m_buttonTabList[i]->field_36 = m_tabButtonIndex == i;
		}
	}
}
