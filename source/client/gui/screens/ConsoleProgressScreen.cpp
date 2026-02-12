#include "ConsoleProgressScreen.hpp"
#include "client/resources/LoadingTipManager.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/LogoRenderer.hpp"
#include "client/app/Minecraft.hpp"

bool ConsoleProgressScreen::isInGameScreen()
{
	return false;
}

void ConsoleProgressScreen::init()
{
	m_uiTheme = UI_CONSOLE;
	Screen::init();

	m_header = Language::get("loading.initializing");
}

void ConsoleProgressScreen::render(float f)
{
	renderMenuBackground(f);

	int loadingBarX = m_width / 2 - 320;
	int loadingBarY = m_height / 2 + 30;
	m_pFont->drawScalableShadow(m_pMinecraft->getProgressMessage(), loadingBarX + 7, loadingBarY - 15, Color::WHITE, 1.5f);
	blitTexture(*m_pMinecraft->m_pTextures, "gui/loading_background.png", loadingBarX, loadingBarY, 0, 0, 640, 20);

	float prog = Mth::clamp(m_pMinecraft->m_progressPercent / 100.0f, 0.0f, 1.0f);

	if (prog >= 0)
		blitTexture(*m_pMinecraft->m_pTextures, "gui/loading_bar.png", loadingBarX + 2, loadingBarY + 2, 0, 0, int(636 * Mth::clamp(prog, 0.0f, 1.0f)), 16, 636, 16);

	const LoadingTip& tip = LoadingTipManager::singleton().getActual();
	if (!tip.text.empty())
	{
		int panelWidth = 801;
		int panelHeight = 81;
		int loadingBarBottom = loadingBarY + 20;
		int panelY = loadingBarBottom + ((m_height - loadingBarBottom) - panelHeight) / 2;

		blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::POINTER_TEXT_PANEL_SLICES, (m_width - panelWidth) / 2, panelY, panelWidth, panelHeight, 8);

		if (m_loadingTip != tip.text)
		{
			m_loadingTip = tip.text;
			m_loadingTipLines = m_pFont->split(tip.text, (panelWidth - 60) / 2);
		}

		for (std::vector<std::string>::iterator it = m_loadingTipLines.begin(); it != m_loadingTipLines.end(); ++it)
		{
			std::string& line = *it;

			int lineWidth = m_pFont->width(line);

			m_pFont->drawScalable(line, m_width / 2 - lineWidth, panelY + 11, Color::WHITE);

			panelY += 24;
		}
	}

	m_pFont->drawOutlinedString(m_header, (m_width - m_pFont->width(m_header) * 4) / 2, loadingBarY - 105, Color::WHITE, Color::BLACK);

	LogoRenderer::singleton().render(f);
}

void ConsoleProgressScreen::updateEvents()
{
	if (m_pMinecraft->isLevelGenerated())
	{

		m_pMinecraft->setScreen(nullptr);
		return;
	}

	Screen::updateEvents();
}