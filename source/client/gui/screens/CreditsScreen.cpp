#include "CreditsScreen.hpp"
#include "StartMenuScreen.hpp"
#include "client/resources/Resource.hpp"

CreditsScreen::CreditsScreen(Screen* parent) :
	m_pParent(parent),
	m_btnBack("Back")
{
	m_bDeletePrevious = false;
	_initCreditsText();
}

CreditsScreen::~CreditsScreen()
{
}

void CreditsScreen::_initCreditsText()
{
	std::string credits;
	Resource::load("credits.txt", credits);
	std::istringstream credits_stream(credits);

	std::string line;
	while (std::getline(credits_stream, line))
	{
		if (line.empty())
			continue;
		if (line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		m_credits.push_back(line);
	}

	if (m_credits.empty())
	{
		m_credits.push_back("Failed to load credits.txt");
	}
}

void CreditsScreen::init()
{
	m_btnBack.m_yPos = m_height - 28;
	m_btnBack.m_width = 84;
	m_btnBack.m_height = 24;

	m_btnBack.m_xPos = m_width / 2 - m_btnBack.m_width / 2;

	if (!_useController())
		_addElement(m_btnBack);
}

bool CreditsScreen::isInGameScreen()
{
	return true;
}

void CreditsScreen::keyPressed(int code)
{
	Screen::keyPressed(code);
}

void CreditsScreen::tick()
{
}

void CreditsScreen::render(float f)
{
	renderBackground();

	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");

	// Add dark background
	Tesselator& t = Tesselator::instance;
	float widthf = float(m_width);
	float heightf = float(m_height);
	float heightsub32 = heightf - 32;
	t.begin(4);
	t.color(0x505050, 255);
	t.vertexUV(0.0f, heightf, 0.0f, 0.0f, heightf / 32.0f);
	t.vertexUV(widthf, heightf, 0.0f, widthf / 32.0f, heightf / 32.0f);
	t.color(0x505050, 255);
	t.vertexUV(widthf, heightsub32, 0.0f, widthf / 32.0f, heightsub32 / 32.0f);
	t.vertexUV(0.0f, heightsub32, 0.0f, 0.0f, heightsub32 / 32.0f);
	t.color(0x202020);
	t.vertexUV(0.0f, heightsub32, 0.0f, 0.0f, heightsub32 / 32.0f);
	t.vertexUV(widthf, heightsub32, 0.0f, widthf / 32.0f, heightsub32 / 32.0f);
	t.vertexUV(widthf, 26.0f, 0.0f, widthf / 32.0f, 26.0f / 32.0f);
	t.vertexUV(0.0f, 26.0f, 0.0f, 0.0f, 26.0f / 32.0f);
	t.draw(m_materials.ui_texture_and_color);

	Screen::render(f);

	drawCenteredString(*m_pMinecraft->m_pFont, "Credits", m_width / 2, 8, Color::WHITE);

	int lineheight = 10;
	int height = (m_height - (m_credits.size() * lineheight)) / 2 - 2;
	for (size_t i = 0; i < m_credits.size(); ++i)
	{
		drawCenteredString(*m_pMinecraft->m_pFont, m_credits[i], m_width / 2, height, Color::WHITE);
		height += lineheight;
	}
}

bool CreditsScreen::handleBackEvent(bool b)
{
	if (b)
		return true;

	m_pMinecraft->setScreen(m_pParent);
	return true;
}

void CreditsScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_btnBack.getId())
		handleBackEvent(false);
}
