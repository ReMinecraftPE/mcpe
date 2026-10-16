#include "OptionsScreen_Console.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/LogoRenderer.hpp"

OptionsScreen_Console::OptionsScreen_Console(Screen* screen) :
	m_pParent(screen),
	m_btnHowToPlay(Language::get("settingsMenu.howToPlay")),
	m_btnControls(Language::get("settingsMenu.controls")),
	m_btnSettings(Language::get("settingsMenu.settings")),
	m_btnCredits(Language::get("settingsMenu.credits")),
	m_btnResetToDefaults(Language::get("settingsMenu.resetToDefaults"))
{
	m_bDeletePrevious = false;
	m_btnHowToPlay.setEnabled(false);

	m_uiTheme = UI_CONSOLE;
}

void OptionsScreen_Console::_buttonClicked(Button* btn)
{
	if (btn->getId() == m_btnControls.getId())
		m_pMinecraft->setScreen(new ControlsPanelScreen(this, *m_pMinecraft));
	else if (btn->getId() == m_btnSettings.getId())
		m_pMinecraft->setScreen(new SettingsPanelScreen(this, *m_pMinecraft));
	else if (btn->getId() == m_btnCredits.getId())
		m_pMinecraft->getScreenChooser()->pushCreditsScreen(this);
	else if (btn->getId() == m_btnResetToDefaults.getId())
	{
		m_pMinecraft->getOptions()->reset();
		//Certainly you wouldn't want to reset this option
		m_pMinecraft->getOptions()->m_uiTheme.set(m_uiTheme);
		m_pMinecraft->saveOptionsAsync();
	}
}

void OptionsScreen_Console::init()
{
	Button* layoutButtons[] = { &m_btnHowToPlay, &m_btnControls, &m_btnSettings, &m_btnCredits, &m_btnResetToDefaults };

	constexpr int buttonsWidth = 450;
	constexpr int buttonsHeight = 40;
	int y= m_height / 3 + 10;
	constexpr int ySpacing = 50;

	for (size_t i = 0; i < 5; ++i)
	{
		Button* button = layoutButtons[i];
		button->m_width = buttonsWidth;
		button->m_height = buttonsHeight;
		button->m_xPos = (m_width - button->m_width) / 2;
		button->m_yPos = y + ySpacing * i;
		_addElement(*button);
	}
}

void OptionsScreen_Console::render(float f)
{
	renderBackground();
	LogoRenderer::singleton().render(f);
	Screen::render(f);
}

bool OptionsScreen_Console::handleBackEvent(bool b)
{
	if (!b)
	{
		m_pMinecraft->setScreen(m_pParent);
	}

	return true;
}

bool OptionsScreen_Console::validate(Minecraft* mc)
{
	if (mc->getOptions()->getUiTheme() != UI_CONSOLE)
	{
		mc->getScreenChooser()->pushOptionsScreen(m_pParent);
		return false;
	}
	return true;
}

#define HEADER(text) do { m_layout.m_elements.push_back(new OptionHeader_Console(text)); currentIndex++; } while (0)
#define OPTION(name) do { options.name.addGuiElement(m_layout.m_elements, m_uiTheme); currentIndex++; } while (0)

ControlsPanelScreen::ControlsPanelScreen(Screen* parent, Minecraft& mc) : PanelScreen_Console(parent)
{
	Options& options = *mc.getOptions();
	int currentIndex = -1;

	OPTIONS_LIST_CONTROLS_CONTROLS;
	OPTIONS_LIST_CONTROLS_FEEDBACK;
	OPTIONS_LIST_CONTROLS_EXPERIMENTAL;

	(void)currentIndex; // compiler will warn about an unused variable sometimes if this isn't here
}

void ControlsPanelScreen::removed()
{
	m_pMinecraft->saveOptionsAsync();
}

SettingsPanelScreen::SettingsPanelScreen(Screen* parent, Minecraft& mc) : PanelScreen_Console(parent)
{
	m_pMinecraft = &mc;

	int currentIndex = -1;

	Options& options = *mc.getOptions();

	OPTIONS_LIST_GAMEPLAY_GAME;
	OPTIONS_LIST_GAMEPLAY_AUDIO;
	OPTIONS_LIST_VIDEO_GRAPHICS;
	OPTIONS_LIST_VIDEO_EXPERIMENTAL;

#ifdef ENH_MENU_BACKGROUND
	if (!Screen::isMenuPanoramaAvailable())
		m_layout.m_elements[idxPano]->setEnabled(false);
#endif

	if (!AppPlatform::singleton()->isVSyncSwitchable())
		m_layout.m_elements[idxVSync]->setEnabled(false);

	// @HACK: 360 is the only platform that supports gamma for the time being
#if !MC_PLATFORM_XBOX360
	m_layout.m_elements[idxGamma]->setEnabled(false);
#endif

	(void)currentIndex; // compiler will warn about an unused variable sometimes if this isn't here
}

void SettingsPanelScreen::render(float f)
{
	PanelScreen_Console::render(f);

	constexpr int difficultyPanelWidth = 701;
	constexpr int difficultyPanelHeight = 111;
	int difficultyPanelX = (m_width - difficultyPanelWidth) / 2;
	int difficultyPanelY = m_panel.y + m_panel.h + 17;

	blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::POINTER_TEXT_PANEL_SLICES, difficultyPanelX, difficultyPanelY, difficultyPanelWidth, difficultyPanelHeight, 8);
	
	m_pFont->drawWordWrap(Language::get(m_pMinecraft->getOptions()->m_difficulty.getValue() + ".desc"), difficultyPanelX + 11, difficultyPanelY + 15, Color::WHITE, (difficultyPanelWidth - 22) / 2, 22, false);
}

void SettingsPanelScreen::removed()
{
	m_pMinecraft->saveOptionsAsync();
}

OptionHeader_Console::OptionHeader_Console(const std::string& text)
	: m_text(text)
{
	m_height = 22;
	setNavigable(false);
}

void OptionHeader_Console::render(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	pMinecraft->m_pFont->drawScalable(
		m_text,
		m_xPos,
		m_yPos + 8,
		Color::TEXT_GREY);
}
