#include "ConsoleSettingsScreen.hpp"
#include "CreditsScreen.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/LogoRenderer.hpp"

ConsoleSettingsScreen::ConsoleSettingsScreen(Screen* screen) :
	m_pParent(screen),
	m_btnHTP(Language::get("settingsMenu.howToPlay")),
	m_btnControls(Language::get("settingsMenu.controls")),
	m_btnSettings(Language::get("settingsMenu.settings")),
	m_btnCredits(Language::get("settingsMenu.credits")),
	m_btnResetToDefaults(Language::get("settingsMenu.resetToDefaults"))
{
	m_bDeletePrevious = false;
	m_btnHTP.setEnabled(false);

	m_uiTheme = UI_CONSOLE;
}

void ConsoleSettingsScreen::init()
{
	std::vector<Button*> layoutButtons;

	layoutButtons.push_back(&m_btnHTP);
	layoutButtons.push_back(&m_btnControls);
	layoutButtons.push_back(&m_btnSettings);
	layoutButtons.push_back(&m_btnCredits);
	layoutButtons.push_back(&m_btnResetToDefaults);

	int buttonsWidth = 450;
	int buttonsHeight = 40;
	int y= m_height / 3 + 10;
	int ySpacing = 50;

	for (size_t i = 0; i < layoutButtons.size(); ++i)
	{
		Button* button = layoutButtons[i];
		button->m_width = buttonsWidth;
		button->m_height = buttonsHeight;
		button->m_xPos = (m_width - button->m_width) / 2;
		button->m_yPos = y + ySpacing * i;
		_addElement(*button);
	}
}

void ConsoleSettingsScreen::render(float f)
{
	renderBackground();
	LogoRenderer::singleton().render(f);
	Screen::render(f);
}

bool ConsoleSettingsScreen::handleBackEvent(bool b)
{
	if (!b)
	{
		m_pMinecraft->setScreen(m_pParent);
	}

	return true;
}

void ConsoleSettingsScreen::_buttonClicked(Button* btn)
{
	if (btn->getId() == m_btnControls.getId())
		m_pMinecraft->setScreen(new ControlsPanelScreen(this, m_pMinecraft));

	if (btn->getId() == m_btnSettings.getId())
		m_pMinecraft->setScreen(new SettingsPanelScreen(this, *m_pMinecraft->getOptions()));

	if (btn->getId() == m_btnCredits.getId())
		m_pMinecraft->setScreen(new CreditsScreen(this));

	if (btn->getId() == m_btnResetToDefaults.getId())
	{
		m_pMinecraft->getOptions()->reset();
		//Certainly you wouldn't want to reset this option
		m_pMinecraft->getOptions()->m_uiTheme.set(m_uiTheme);
		m_pMinecraft->saveOptionsAsync();
	}
}

#define OPTION(name) do { options.name.addGuiElement(m_layout.m_elements, m_uiTheme); currentIndex++; } while (0)

ControlsPanelScreen::ControlsPanelScreen(Screen* parent, Minecraft* mc) : PanelScreen(parent)
{
	Options& options = *mc->getOptions();
	int currentIndex = -1;
	int idxSplit = -1, idxController = -1;

	OPTION(m_autoJump);
	OPTION(m_invertMouse);
	OPTION(m_splitControls); idxSplit = currentIndex;
	OPTION(m_bUseController); idxController = currentIndex;
	OPTION(m_flightHax);

	if (!mc->isTouchscreen())
		m_layout.m_elements[idxSplit]->setEnabled(false);
	m_layout.m_elements[idxController]->setEnabled(false);
}

void ControlsPanelScreen::removed()
{
	m_pMinecraft->saveOptionsAsync();
}

SettingsPanelScreen::SettingsPanelScreen(Screen* parent, Options& options) : PanelScreen(parent)
{
	int currentIndex = 0;

	OPTION(m_musicVolume);
	OPTION(m_masterVolume);
	OPTION(m_sensitivity);
	OPTION(m_hudSize);
	OPTION(m_uiTheme);
	OPTION(m_logoType);
	OPTION(m_viewDistance);
	OPTION(m_thirdPerson);
	OPTION(m_ambientOcclusion);
	OPTION(m_fancyGraphics);
	OPTION(m_viewBobbing);
	OPTION(m_anaglyphs);
	OPTION(m_blockOutlines);
	OPTION(m_fancyGrass);
	OPTION(m_biomeColors);
	OPTION(m_hideGui);
	OPTION(m_difficulty);
}

void SettingsPanelScreen::render(float f)
{
	PanelScreen::render(f);

	constexpr int difficultyPanelWidth = 701;
	constexpr int difficultyPanelHeight = 111;
	int difficultyPanelX = (m_width - difficultyPanelWidth) / 2;
	int difficultyPanelY = m_panel.y + m_panel.h + 17;

	blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::POINTER_TEXT_PANEL_SLICES, difficultyPanelX, difficultyPanelY, difficultyPanelWidth, difficultyPanelHeight, 8);
	
	m_pFont->drawWordWrap(Language::get(m_pMinecraft->getOptions()->m_difficulty.getValue() + ".desc"), difficultyPanelX + 11, difficultyPanelY + 15, Color::WHITE, (difficultyPanelWidth - 22) / 2, 22, false, true);
}

void SettingsPanelScreen::removed()
{
	m_pMinecraft->saveOptionsAsync();
}
