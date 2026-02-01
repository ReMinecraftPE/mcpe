/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "thirdparty/rapidjson/document.h"
#include "client/resources/Resource.hpp"
#include "ProgressScreen.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/LogoRenderer.hpp"

bool ProgressScreen::isInGameScreen()
{
	return false;
}

void ProgressScreen::init()
{
	m_uiTheme = m_pMinecraft->getOptions()->m_uiTheme;
	Screen::init();
}

void ProgressScreen::render(float f)
{

	if (m_uiTheme == UI_CONSOLE)
	{
		renderMenuBackground(f);

		int loadingBarX = m_width / 2 - 320;
		int loadingBarY = m_height / 2 + 30;
		m_pFont->drawScalableShadow(m_pMinecraft->getProgressMessage(), loadingBarX + 7, loadingBarY - 15, Color::WHITE, 1.5f);
		blitTexture(*m_pMinecraft->m_pTextures, "gui/loading_background.png", loadingBarX, loadingBarY, 0, 0, 640, 20);

		float prog = Mth::clamp(m_pMinecraft->m_progressPercent / 100.0f, 0.0f, 1.0f);

		if (prog >= 0)
			blitTexture(*m_pMinecraft->m_pTextures, "gui/loading_bar.png", loadingBarX + 2, loadingBarY + 2, 0, 0, int(636 * Mth::clamp(prog, 0.0f, 1.0f)), 16, 636, 16);

		const LoadingTip& tip = LoadingTips::singleton().getActual();
		if (!tip.text.empty())
		{
			int panelWidth = 801;
			int panelHeight = 81;
			int loadingBarBottom = loadingBarY + 20;
			int panelY = loadingBarBottom + ((m_height - loadingBarBottom) - panelHeight) / 2;

			blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::POINTER_TEXT_PANEL_SLICES, (m_width - panelWidth) / 2, panelY, panelWidth, panelHeight, 8);

			std::vector<std::string> lines = m_pFont->split(tip.text, (panelWidth - 60) / 2);

			for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
			{
				std::string& line = *it;

				int lineWidth = m_pFont->width(line);

				m_pFont->drawScalable(line, m_width / 2 - lineWidth, panelY + 11, Color::WHITE);

				panelY += 24;
			}
		}

		std::string header = Language::singleton().get("legacy.initializing");

		m_pFont->drawOutlinedString(header, (m_width - m_pFont->width(header) * 4) / 2, loadingBarY - 105, Color::WHITE, Color::BLACK);

		LogoRenderer::singleton().render(f);

		return;
	}

	renderDirtBackground(0);

	int yPos = m_height / 2;

	if (m_pMinecraft->m_progressPercent >= 0)
	{
		float lX = float(m_width) / 2 - 50, rX = float(m_width) / 2 + 50;
		float prog = float(m_pMinecraft->m_progressPercent);

		// gray background
		fill(lX, yPos + 16.0f, rX, yPos + 18.0f, 0xFF808080);
		// the green stuff
		fill(lX, yPos + 16.0f, lX + prog, yPos + 18.0f, 0xFF80FF80);
	}

	//! Using m_pMinecraft->m_pFont instead of m_pFont.
	Font* pFont = m_pMinecraft->m_pFont;

	int width = pFont->width("Generating world");
	pFont->drawShadow("Generating world", (m_width - width) / 2, yPos - 20, 0xFFFFFF);

	width = pFont->width(m_pMinecraft->getProgressMessage());
	pFont->drawShadow(m_pMinecraft->getProgressMessage(), (m_width - width) / 2, yPos + 4, 0xFFFFFF);

#ifdef ORIGINAL_CODE
	sleepMs(50);
#endif
}

void ProgressScreen::updateEvents()
{
	if (m_pMinecraft->isLevelGenerated())
	{

		m_pMinecraft->setScreen(nullptr);
		return;
	}

	Screen::updateEvents();
}

LoadingTip LoadingTip::EMPTY(Util::EMPTY_STRING);

LoadingTips* LoadingTips::instance = nullptr;

LoadingTips::LoadingTips() :
	m_actual(LoadingTip::EMPTY),
	m_actualTime(0)
{
}

void LoadingTips::init()
{
	m_all.clear();
	m_using.clear();

	using namespace rapidjson;

	std::string file;
	if (!Resource::load("texts/tips.json", file)) return;
	Document d;
	d.Parse(file.c_str());

	if (d.Empty()) return;

	for (Value::ConstValueIterator it = d.Begin(); it != d.End(); ++it)
	{
		if (it->IsString())
			m_all.push_back(LoadingTip(it->GetString()));
		else if (it->IsObject())
		{
			Value::ConstObject translateValue = it->GetObj();
			Value::ConstMemberIterator translate = translateValue.FindMember("translate");
			Value::ConstMemberIterator time = translateValue.FindMember("time");

			if (translate != translateValue.MemberEnd())
			{
				std::string translation = Language::singleton().get(translate->value.GetString());
				if (time == translateValue.MemberEnd())
					m_all.push_back(LoadingTip(translation));
				else
					m_all.push_back(LoadingTip(translation, time->value.GetInt()));
			}
		}
	}
}

const LoadingTip& LoadingTips::getActual()
{
	if (m_using.empty())
	{
		if (m_all.empty()) return LoadingTip::EMPTY;
		else
		{
			for (std::vector<LoadingTip>::iterator it = m_all.begin(); it != m_all.end(); ++it)
			{
				m_using.push_back(&(*it));
			}
		}
	}

	if (m_actual.text.empty())
	{
		int i = m_random.nextInt(m_using.size());
		m_actual = *m_using[i];
		m_actualTime = getTimeMs();
	}
	else if (getTimeMs() - m_actualTime >= m_actual.timeOnScreen)
	{
		m_actual = LoadingTip::EMPTY;
		m_actualTime = 0;
		return getActual();
	}

	return m_actual;
}
