/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

struct LoadingTip
{
	std::string text;
	int timeOnScreen;

	LoadingTip(const std::string text, int timeOnScreen) :
		text(text),
		timeOnScreen(timeOnScreen)
	{
	}

	LoadingTip(const std::string text) :
		text(text),
		timeOnScreen(text.size() * 80)
	{
	}

	bool operator==(const LoadingTip& other) const {
		return text == other.text && timeOnScreen == other.timeOnScreen;
	}

	static LoadingTip EMPTY;
};

class LoadingTips
{
public:
	LoadingTips();

	void init();

	const LoadingTip& getActual();

	static LoadingTips& singleton()
	{
		if (!instance)
		{
			instance = new LoadingTips;
		}
		
		return *instance;
	}

private:
	Random m_random;
	LoadingTip m_actual;
	int m_actualTime;

public:
	std::vector<LoadingTip> m_all;
	std::vector<LoadingTip*> m_using;

	static LoadingTips* instance;
};

class ProgressScreen : public Screen
{
public:
	void render(float f) override;
	void updateEvents() override;
	bool isInGameScreen() override;
	void init() override;
};

