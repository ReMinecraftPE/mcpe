#include "ConsolePauseScreen.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/LogoRenderer.hpp"

ConsolePauseScreen::ConsolePauseScreen() :
	m_btnResume(Language::get("pauseMenu.resume")),
	m_btnHelpAndOptions(Language::get("mainMenu.helpAndOptions")),
	m_btnLeaderboards(Language::get("mainMenu.leaderboards")),
	m_btnAchievements(Language::get("mainMenu.achievements")),
	m_btnSaveGame(Language::get("pauseMenu.saveGame")),
	m_btnExitGame(Language::get("mainMenu.exitGame"))
{
	m_bDeletePrevious = false;
	m_btnLeaderboards.setEnabled(false);
	m_btnAchievements.setEnabled(false);

	m_uiTheme = UI_CONSOLE;
}

void ConsolePauseScreen::init()
{
	Button* layoutButtons[] = { &m_btnResume, &m_btnLeaderboards, &m_btnAchievements, &m_btnHelpAndOptions, &m_btnSaveGame, &m_btnExitGame };

	int buttonsWidth = 400;
	int buttonsHeight = 40;
	int y = m_height / 3 + 10;
	int ySpacing = 50;

	for (size_t i = 0; i < 6; ++i)
	{
		Button* button = layoutButtons[i];
		button->m_width = buttonsWidth;
		button->m_height = buttonsHeight;
		button->m_xPos = (m_width - button->m_width) / 2;
		button->m_yPos = y + ySpacing * i;
		_addElement(*button);
	}
}

void ConsolePauseScreen::render(float f)
{
	renderBackground();
	LogoRenderer::singleton().render(f);
	Screen::render(f);
}

void ConsolePauseScreen::_buttonClicked(Button* btn)
{
	if (btn->getId() == m_btnResume.getId())
		m_pMinecraft->resumeGame();
	else if (btn->getId() == m_btnHelpAndOptions.getId())
		m_pMinecraft->getScreenChooser()->pushOptionsScreen(this);
	else if (btn->getId() == m_btnSaveGame.getId())
		m_pMinecraft->m_pLevel->saveGame();
	else if (btn->getId() == m_btnExitGame.getId())
		m_pMinecraft->leaveGame(false);
}