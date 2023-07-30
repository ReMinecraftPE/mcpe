#pragma once

#include "ScrolledSelectionList.hpp"
#include "PingedCompatibleServer.hpp"

class AvailableGamesList : public ScrolledSelectionList
{
public:
	AvailableGamesList(Minecraft*, int, int, int, int, int);
	int getNumberOfItems() override;
	bool isSelectedItem(int i) override;
	void renderBackground() override;
	void renderItem(int, int, int, int, Tesselator& t) override;
	void selectItem(int, bool) override;

public:
	int m_selectedIndex;
	std::vector<PingedCompatibleServer> m_games;
};

