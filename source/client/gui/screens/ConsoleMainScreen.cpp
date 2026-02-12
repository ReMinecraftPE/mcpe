#include "ConsoleMainScreen.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/LogoRenderer.hpp"
#include "client/resources/SplashManager.hpp"
#include "PlayGameScreen.hpp"

ConsoleMainScreen::ConsoleMainScreen() :
	m_btnPlayGame(Language::get("mainMenu.playGame")),
	m_btnLeaderboards(Language::get("mainMenu.leaderboards")),
	m_btnAchievements(Language::get("mainMenu.achievements")),
	m_btnHelpAndOptions(Language::get("mainMenu.helpAndOptions")),
	m_btnDownload(Language::get("mainMenu.download")),
	m_btnExitGame(Language::get("mainMenu.exitGame"))
{
	m_bDeletePrevious = false;
	m_btnLeaderboards.setEnabled(false);
	m_btnAchievements.setEnabled(false);
	m_btnDownload.setEnabled(false);

	m_uiTheme = UI_CONSOLE;

	m_splash = SplashManager::singleton().getSplash();
}

void ConsoleMainScreen::init()
{
	Button* layoutButtons[] = { &m_btnPlayGame, &m_btnLeaderboards, &m_btnAchievements, &m_btnHelpAndOptions, &m_btnDownload, &m_btnExitGame };

	int buttonsWidth = 450;
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

void ConsoleMainScreen::render(float f)
{
	renderBackground();
	LogoRenderer::singleton().render(f);
	MatrixStack::Ref mtx = MatrixStack::World.push();

	int textWidth = m_pFont->width(m_splash);
	mtx->translate(Vec3(float(m_width) / 2.0f + 230.0f, 170.0f, 0.0f));
	mtx->rotate(-15.0f, Vec3::UNIT_Z);
	float timeMS = float(getTimeMs() % 1000) / 1000.0f;
	float scale = 1.8f - Mth::abs(0.1f * Mth::sin(2.0f * float(M_PI) * timeMS));
	scale = (scale * 100.0f) / (32.0f + textWidth) * 3;
	mtx->scale(scale);

	drawCenteredString(*m_pFont, m_splash, 0, -8, Color::YELLOW);
	mtx.release();
	Screen::render(f);
}

void ConsoleMainScreen::_buttonClicked(Button* btn)
{
	if (btn->getId() == m_btnPlayGame.getId())
	{
		m_pMinecraft->locateMultiplayer();
		m_pMinecraft->setScreen(new PlayGameScreen(m_pMinecraft, this));
	}
	else if (btn->getId() == m_btnHelpAndOptions.getId())
		m_pMinecraft->getScreenChooser()->pushOptionsScreen(this);
	else if (btn->getId() == m_btnExitGame.getId())
		m_pMinecraft->quit();
}