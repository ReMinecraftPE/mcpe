#include "ConsoleSettingsScreen.hpp"
#include "client/locale/Language.hpp"
#include "PlayGameScreen.hpp"
#include "CreateWorldScreen.hpp"
#include "ProgressScreen.hpp"
#include "renderer/ShaderConstants.hpp"

PlayGameScreen::PlayGameScreen(Minecraft* mc, Screen* parent) : PanelScreen(parent),
	m_joinLayout(this)
{
	fillSaves(mc);
}

void PlayGameScreen::init()
{
	m_panel.w = 1040;
	m_panel.h = 426;
	m_panel.x = (m_width - m_panel.w) / 2;
	m_panel.y = (m_height - m_panel.h) / 2 + 59;
	m_startPanel.x = m_panel.x + 18;
	m_startPanel.y = m_panel.y + 22;
	m_startPanel.w = 500;
	m_startPanel.h = 380;
	m_joinPanel = m_startPanel;
	m_joinPanel.x = m_panel.x + m_panel.w - m_joinPanel.w - 16;
	m_layout.init(m_startPanel.x + 20, m_startPanel.y + 42, 460, 300, 0);
	m_joinLayout.init(m_joinPanel.x + 20, m_joinPanel.y + 42, 460, 300, 0);
}

void PlayGameScreen::fillSaves(Minecraft* mc)
{
	m_layout.clear();

	m_layout.m_elements.push_back(new CreateButton(Language::get("playGame.createNewWorld")));
	ListButton* playTutorial = new ListButton(Language::get("playGame.tutorial"));
	playTutorial->setEnabled(false);
	m_layout.m_elements.push_back(playTutorial);

	std::vector<LevelSummary> levels;

	mc->getLevelSource()->getLevelList(levels);
	std::sort(levels.begin(), levels.end());

	for (size_t i = 0; i < levels.size(); ++i)
	{
		LevelSummary& level = levels[i];
		if (level.m_fileName == "_LastJoinedServer")
			continue;

		m_layout.m_elements.push_back(new SaveButton(level));
	}

	if (m_layout.m_elements.size() > 2)
		m_layout.selectElement(2, false);
}

void PlayGameScreen::fillServers()
{
	m_joinLayout.clear();

	m_joinLayout.setNavigable(!m_servers.empty());

	for (size_t i = 0; i < m_servers.size(); ++i)
	{
		m_joinLayout.m_elements.push_back(new JoinButton(m_servers[i]));
	}
	m_joinLayout.organize();
}

void PlayGameScreen::tick()
{
	std::vector<PingedCompatibleServer>* serverList, serverListFiltered;
	serverList = m_pMinecraft->m_pRakNetInstance->getServerList();

	for (size_t i = 0; i < serverList->size(); i++)
	{
		const PingedCompatibleServer& pcs = (*serverList)[i];
		if (pcs.m_name.GetLength())
			serverListFiltered.push_back(pcs);
	}

	if (serverListFiltered.size() != m_servers.size())
	{
		PingedCompatibleServer selectedItem;

		m_servers = serverListFiltered;
		fillServers();
	}
	else if (!serverListFiltered.empty())
	{
		bool changed = false;
		for (int i = int(m_servers.size() - 1); i >= 0; i--)
		{
			size_t j = 0;
			for (; j < serverListFiltered.size(); j++)
			{
				if (serverListFiltered[j].m_address == m_servers[i].m_address)
					break;
			}

			if (j == serverListFiltered.size())
				continue;

			if (m_servers[i].m_name != serverListFiltered[j].m_name)
			{
				m_servers[i].m_name = serverListFiltered[j].m_name;
				changed = true;
			}
		}

		if (changed)
			fillServers();
	}
}

bool PlayGameScreen::handleBackEvent(bool b)
{
	if (!b)
		m_pMinecraft->cancelLocateMultiplayer();
	return PanelScreen::handleBackEvent(b);
}

void PlayGameScreen::renderPanel(float f)
{
	PanelScreen::renderPanel(f);
	blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::PANEL_RECESS_SLICES, m_startPanel.x, m_startPanel.y, m_startPanel.w, m_startPanel.h, 16);
	currentShaderColor.a = 0.5f;
	blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::PANEL_RECESS_SLICES, m_joinPanel.x, m_joinPanel.y, m_joinPanel.w, m_joinPanel.h, 16, &m_materials.ui_textured_and_glcolor);
	currentShaderColor.a = 1.0f;

	const std::string& startGame = Language::get("playGame.start");
	m_pFont->drawScalable(startGame, m_startPanel.x + m_startPanel.w / 2 - m_pFont->width(startGame), m_startPanel.y + 14, Color::GREY_TEXT);

	const std::string& joinGame = Language::get("playGame.join");
	m_pFont->drawScalable(joinGame, m_joinPanel.x + m_joinPanel.w / 2 - m_pFont->width(joinGame), m_joinPanel.y + 14, Color::GREY_TEXT);

	if (m_servers.empty())
		m_pFont->drawScalable(Language::get("playGame.noGamesFound"), m_joinPanel.x + 151, m_joinPanel.y + 192, Color::GREY_TEXT);
}

ListButton::ListButton(const std::string& text) : Button(0, 0, 460, 60, text)
{
}

void ListButton::render(Minecraft* mc, const MenuPointer& pointer)
{
	if (!mc->m_pScreen->doElementTabbing())
		setSelected(isHovered(mc, pointer));

	Textures& texs = *mc->m_pTextures;
	if (!isEnabled())
		currentShaderColor.a *= 0.5f;
	blitSprite(texs, isSelected() && !hasFocus() ? "gui/console/Graphics/ListButton_Over.png" : "gui/console/Graphics/ListButton_Norm.png", m_xPos, m_yPos, m_width, m_height, &m_materials.ui_textured_and_glcolor);

	Font& font = *mc->m_pFont;

	Color textColor;
	if (isSelected())
	{
		textColor = Color(220, 220, 0, currentShaderColor.a); // 0xDCDC00
	}
	else
		textColor = Color(224, 224, 224, currentShaderColor.a); // 0xE0E0E0U
	renderMessage(*mc->m_pFont, textColor);

	currentShaderColor = Color::WHITE;
}

void ListButton::renderMessage(Font& font, const Color& textColor)
{
	font.drawScalableShadow(getMessage(), m_xPos + 67, m_yPos + (m_height - 16) / 2, textColor);
}

CreateButton::CreateButton(const std::string& text) : ListButton(text)
{
}

void CreateButton::pressed(Minecraft* mc)
{
	//@TODO: Change this with a Console UI themed Create World screen
	mc->setScreen(new CreateWorldScreen(mc->m_pScreen));
}

SaveButton::SaveButton(const LevelSummary& summary) : ListButton(summary.m_levelName),
	m_summary(summary)
{
}

void SaveButton::pressed(Minecraft* mc)
{
	//@TODO: Replace this with Load Save screen
	mc->selectLevel(m_summary);
}

JoinButton::JoinButton(const PingedCompatibleServer& server) : ListButton(Util::format(Language::get("playGame.joinButton").c_str(), server.m_name.C_String())),
	m_server(server)
{
}

void JoinButton::renderMessage(Font& font, const Color& textColor)
{
	font.drawScalableShadow(getMessage(), m_xPos + 10, m_yPos + (m_height - 16) / 2, textColor);
}

void JoinButton::pressed(Minecraft* mc)
{
	mc->joinMultiplayer(m_server);
	mc->setScreen(new ProgressScreen);
}
