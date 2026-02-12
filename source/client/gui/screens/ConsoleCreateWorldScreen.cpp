#include "ConsoleCreateWorldScreen.hpp"
#include "common/Util.hpp"
#include "client/locale/Language.hpp"
#include "CreateWorldScreen.hpp"

ConsoleCreateWorldScreen::ConsoleCreateWorldScreen(Options& options, Screen* parent) :
	PanelScreen(parent),
	m_onlineGame(0, 0, true, Language::get("playGame.online")),
	m_inviteOnly(0, 0, false, Language::get("playGame.inviteOnly")),
	m_textName(this, 0, 0, 400, 38, "", "New World"),
	m_textSeed(this, 0, 0, 400, 38, ""),
	m_difficultySlider(0, 0, 400, 32, &options.m_difficulty, options.m_difficulty.getMessage(), options.m_difficulty.toFloat()),
	m_btnCreate(0, 0, 400, 40, Language::get("playGame.createNewWorld"))
{
	m_onlineGame.setEnabled(false);
	m_inviteOnly.setEnabled(false);
}

void ConsoleCreateWorldScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_btnCreate.getId())
	{
		std::string nameStr = m_textName.getText();
		std::string seedStr = m_textSeed.getText();

		std::string levelNickname = Util::stringTrim(nameStr);
		std::string levelUniqueName = CreateWorldScreen::GetUniqueLevelName(m_pMinecraft->m_pLevelStorageSource, levelNickname);

		int seed = int(getEpochTimeS());

		std::string seedThing = Util::stringTrim(seedStr);
		if (!seedThing.empty())
		{
			int num;
			if (sscanf(seedThing.c_str(), "%d", &num) > 0)
				seed = num;
			else
				seed = Util::hashCode(seedThing);
		}

		LevelSettings levelSettings(seed);
		m_pMinecraft->selectLevel(levelUniqueName, levelNickname, levelSettings);
	}
}

void ConsoleCreateWorldScreen::init()
{
	m_panel.w = 450;
	m_panel.h = 390;
	m_panel.x = (m_width - m_panel.w) / 2;
	m_panel.y = (m_height - m_panel.h) / 2 + 45;

	int left = m_panel.x + 25;
	m_onlineGame.m_xPos = left;
	m_onlineGame.m_yPos = m_panel.y + 17;
	_addElement(m_onlineGame);

	m_inviteOnly.m_xPos = left;
	m_inviteOnly.m_yPos = m_panel.y + 53;
	_addElement(m_inviteOnly);

	m_textName.m_xPos = left;
	m_textName.m_yPos = m_panel.y + 113;
	_addElement(m_textName);

	m_textSeed.m_xPos = left;
	m_textSeed.m_yPos = m_panel.y + 188;
	_addElement(m_textSeed);

	m_difficultySlider.m_xPos = left;
	m_difficultySlider.m_yPos = m_panel.y + 267;
	_addElement(m_difficultySlider);

	m_btnCreate.m_xPos = left;
	m_btnCreate.m_yPos = m_panel.y + 322;
	_addElement(m_btnCreate);

	m_textName.init(m_pFont);
	m_textSeed.init(m_pFont);
}

void ConsoleCreateWorldScreen::render(float f)
{
	PanelScreen::render(f);

	Font& font = *m_pFont;
	font.drawScalable(Language::get("selectWorld.enterName"), m_textName.m_xPos + 1, m_textName.m_yPos - 19, Color::TEXT_GREY);
	font.drawScalable(Language::get("selectWorld.enterSeed"), m_textSeed.m_xPos + 1, m_textSeed.m_yPos - 19, Color::TEXT_GREY);
	font.drawScalable(Language::get("selectWorld.seedInfo"), m_textSeed.m_xPos + 1, m_textSeed.m_yPos + 41, Color::TEXT_GREY);
}

void ConsoleCreateWorldScreen::renderPanel(float f)
{
	blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::PANEL_SLICES, m_panel.x, m_panel.y, m_panel.w, m_panel.h, 32);
}
