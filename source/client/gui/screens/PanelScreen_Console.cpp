#include "PanelScreen_Console.hpp"
#include "client/renderer/LogoRenderer.hpp"
#include "renderer/ShaderConstants.hpp"

PanelScreen_Console::PanelScreen_Console(Screen* parent) :
	m_pParent(parent)
	, m_layout(this)
{
	m_uiTheme = UI_CONSOLE;
	m_bDeletePrevious = false;
}

void PanelScreen_Console::init()
{
	m_panel.w = 354;
	m_panel.h = 325;
	m_panel.x = (m_width - m_panel.w) / 2;
	m_panel.y = (m_height - m_panel.h) / 2 - 22;
	m_layout.init(IntRectangle(m_panel.x + 15, m_panel.y + 14, 324, 272));
}

void PanelScreen_Console::render(float f)
{
	currentShaderColor = Color::WHITE;

	// @TODO: abstract this out into a Screen base class, since there's tons of console screens that use this
	renderBackground();
	LogoRenderer::singleton().render(f);

	renderPanel(f);
	Screen::render(f);
}

void PanelScreen_Console::renderPanel(float f)
{
	blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::SMALL_PANEL_SLICES, m_panel.x, m_panel.y, m_panel.w, m_panel.h, 16);
}

bool PanelScreen_Console::handleBackEvent(bool b)
{
	if (!b)
	{
		m_pMinecraft->setScreen(m_pParent);
	}

	return true;
}