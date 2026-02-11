#pragma once

#include "PanelScreen.hpp"
#include "client/gui/components/Button.hpp"
#include "network/PingedCompatibleServer.hpp"

class ListButton : public Button
{
public:
	ListButton(const std::string&);

	virtual void render(Minecraft*, const MenuPointer&) override;
	virtual void renderMessage(Font&, const Color&);
};

class CreateButton : public ListButton
{
public:
	CreateButton(const std::string&);

	void pressed(Minecraft*) override;
};

class SaveButton : public ListButton
{
public:
	SaveButton(const LevelSummary&);

	void pressed(Minecraft*) override;

public:
	LevelSummary m_summary;
};

class JoinButton : public ListButton
{
public:
	JoinButton(const PingedCompatibleServer&);

	void renderMessage(Font&, const Color&) override;
	void pressed(Minecraft*) override;

public:
	PingedCompatibleServer m_server;
};

class PlayGameScreen : public PanelScreen
{
public:
	PlayGameScreen(Minecraft*, Screen*);

	void init() override;
	void fillSaves(Minecraft*);
	void fillServers();
	void tick() override;
	bool handleBackEvent(bool) override;
	void renderPanel(float) override;

public:
	VerticalLayout m_joinLayout;
	IntRectangle m_startPanel;
	IntRectangle m_joinPanel;
	std::vector<PingedCompatibleServer> m_servers;
};