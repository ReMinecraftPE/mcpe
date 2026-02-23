#pragma once

#include <string>
#include <vector>
#include "common/Random.hpp"

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

class LoadingTipManager
{
public:
	LoadingTipManager();

	void init();

	const LoadingTip& getActual();

	static LoadingTipManager& singleton()
	{
		if (!instance)
		{
			instance = new LoadingTipManager;
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

	static LoadingTipManager* instance;
};