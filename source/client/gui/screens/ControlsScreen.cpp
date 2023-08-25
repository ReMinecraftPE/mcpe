/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ControlsScreen.hpp"
#include "OptionsScreen.hpp"
//#include "network/ServerSideNetworkHandler.hpp"

static const std::string g_keymapToString[] =
{
	"Forward",
	"Left",
	"Back",
	"Right",
	"Jump",
	"Inventory",
	"Drop",
	"Chat",
	"Toggle Fog",
	"Sneak",
	"Destroy",
	"Place",
	"Menu Next",
	"Menu Previous",
	"Menu OK",
	"Pause",
	"Select Slot 1",
	"Select Slot 2",
	"Select Slot 3",
	"Select Slot 4",
	"Select Slot 5",
	"Select Slot 6",
	"Select Slot 7",
	"Select Slot 8",
	"Select Slot 9",
	"Select Left Slot",
	"Select Right Slot",
	"Hide GUI",
	"Screenshot",
	"Debug Info",
	"Smooth Lighting",
	"Perspective",
	"Fly Up",
	"Fly Down",
	"Open Command"
};

ControlsScreen::ControlsScreen() :
	m_iCurKeybindIndex(-1),
	m_iFirstRenderable(-1),
	m_iLastRenderable(-1),
	m_iHighestScrollHeight(-1),
	m_iScrollHeight(0),
	m_btnBack(KM_COUNT, 0, 0, 100, 20, "Back")
{
	bool flipLeft = false;
	int height = TOP_CUTOFF;
	for (int i = 0; i < KM_COUNT; i++)
	{
		RebindingOption newOption;
		newOption.button = new Button(i, 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "");
		newOption.ypos = height;
		if(flipLeft)
			height += BUTTON_GAP;

		flipLeft = !flipLeft;
		m_rebindingOptions[i] = newOption;
	}

}

ControlsScreen::~ControlsScreen()
{
	for (int i = 0; i < KM_COUNT; i++)
	{
		delete m_rebindingOptions[i].button;
	}
}

void ControlsScreen::UpdateText()
{
	Options& o = m_pMinecraft->m_options;

	for (int i = 0; i < KM_COUNT; i++)
	{
		Button* button = m_rebindingOptions[i].button;
		button->m_text = GetCharacterText(button->m_buttonId);
	}
}

void ControlsScreen::GetRenderableButtons()
{
	int firstRenderable = (m_iScrollHeight / BUTTON_GAP) * 2;
	int visibleArea = (m_height - TOP_CUTOFF - BOTTOM_CUTOFF - 15);
	int lastRenderable = firstRenderable + ((visibleArea + BUTTON_GAP ) / BUTTON_GAP) * 2;

	if (lastRenderable >= KM_COUNT)
		lastRenderable = KM_COUNT;
	for (int i = 0; i < KM_COUNT; i++)
	{
		if (i >= firstRenderable && i < lastRenderable)
		{
			m_rebindingOptions[i].button->m_bEnabled = true;
			continue;
		}
		m_rebindingOptions[i].button->m_bEnabled = false;
	}

	m_iFirstRenderable = firstRenderable;
	m_iLastRenderable = lastRenderable;
}

std::string ControlsScreen::GetCharacterText(int keycode)
{
	Options& o = m_pMinecraft->m_options;

	int keyCode = o.m_keyMappings[keycode].value;
	return m_pMinecraft->m_pPlatform->convertKeycodeToString(keyCode);
}

void ControlsScreen::mouseEvent()
{
	MouseAction* pAction = Mouse::getEvent();
	if (pAction->_buttonType == Mouse::ButtonType::SCROLLWHEEL)
	{

		if (Mouse::getEventButtonState() <= 0) // @NOTE: Scroll up
		{
			m_iScrollHeight -= BUTTON_GAP;
			if (m_iScrollHeight <= 0)
			{
				m_iScrollHeight = 0;
			}
		}
		else
		{
			m_iScrollHeight += BUTTON_GAP;
			if (m_iScrollHeight > m_iHighestScrollHeight)
			{
				m_iScrollHeight = m_iHighestScrollHeight;
			}
		}
	}

	GetRenderableButtons();
	Screen::mouseEvent();
}

void ControlsScreen::init()
{ 
	m_iHighestScrollHeight = (BUTTON_GAP * ((int)round(KM_COUNT / 2.0f) - ((m_height - TOP_CUTOFF - BOTTOM_CUTOFF / 2) / BUTTON_GAP)));

	m_btnBack.m_xPos = m_width - m_btnBack.m_width - 5;
	m_btnBack.m_yPos = m_height - 27;
	m_buttons.push_back(&m_btnBack);

	bool flipLeft = false;
	for (int i = 0; i < KM_COUNT; i++)
	{
		if (flipLeft)
			m_rebindingOptions[i].button->m_xPos = m_width / 2;
		else
			m_rebindingOptions[i].button->m_xPos = m_width / 12;
		
		m_rebindingOptions[i].button->m_yPos = m_rebindingOptions->ypos;
		flipLeft = !flipLeft;
	}

	for (int i = 0; i < KM_COUNT; i++)
		m_buttons.push_back(m_rebindingOptions[i].button);

	for (int i = 0; i < KM_COUNT; i++)
		m_buttonTabList.push_back(m_rebindingOptions[i].button);
		
	GetRenderableButtons();
	UpdateText();
}

void ControlsScreen::render(int a, int b, float c)
{
	if (!m_pMinecraft->isLevelGenerated())
		renderMenuBackground(c);

	fillGradient(0, TOP_CUTOFF - 2, m_width, m_height - BOTTOM_CUTOFF + 7, 0xC0101010, 0xD0101010);
	//Won't work?
	//fill(0, 0, m_width, TOP_CUTOFF, 0xFFFFFF);


	drawCenteredString(m_pFont, "Controls", m_width / 2, 7, 0xFFFFFF);
	if(!m_bChangingKeybinds)
		drawCenteredString(m_pFont, "Click to rebind", m_width / 2, 19, 0xFFFFFF);
	else
		drawCenteredString(m_pFont, "Rebinding...", m_width / 2, 19, 0xFFFFFF);

	m_btnBack.render(m_pMinecraft, a, b);

	for (int i = m_iFirstRenderable; i < m_iLastRenderable; i++)
	{
		Button* button = m_rebindingOptions[i].button;
		button->m_yPos = m_rebindingOptions[i].ypos - m_iScrollHeight;
		button->render(m_pMinecraft, a, b);
		drawString(m_pFont, g_keymapToString[i], button->m_xPos + BUTTON_WIDTH + 5, button->m_yPos + 5, 0xFFFFFF);
	}
}

void ControlsScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnBack.m_buttonId)
	{
		m_pMinecraft->setScreen(new OptionsScreen);
	}
	else if (pButton->m_buttonId >= 0 && pButton->m_buttonId < KM_COUNT)
	{
		//The buttonIDs should be the same as the keymapEnum due to it creating the buttons from KM_COUNT
		m_iCurKeybindIndex = pButton->m_buttonId;
		m_bChangingKeybinds = true;

		pButton->m_text = "> " + GetCharacterText(pButton->m_buttonId) + " <";
	}
}

void ControlsScreen::keyPressed(int keycode)
{
	if (!m_bChangingKeybinds)
	{
		Screen::keyPressed(keycode);
		return;
	}

	if (m_iCurKeybindIndex == -1)
	{
		m_bChangingKeybinds = false;
		return;
	}

	m_pMinecraft->m_options.m_keyMappings[m_iCurKeybindIndex].value = keycode;
	m_iCurKeybindIndex = -1;
	m_bChangingKeybinds = false;

	UpdateText();
}