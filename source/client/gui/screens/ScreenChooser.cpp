#include "ScreenChooser.hpp"
#include "client/app/Minecraft.hpp"
#include "StartMenuScreen.hpp"
#include "ConsoleMainScreen.hpp"
#include "PauseScreen.hpp"
#include "ConsolePauseScreen.hpp"
#include "inventory/CraftingScreen.hpp"
#include "inventory/ChestScreen.hpp"
#include "OptionsScreen.hpp"
#include "CreateWorldScreen.hpp"
#include "ConsoleCreateWorldScreen.hpp"
#include "ConsoleSettingsScreen.hpp"
#include "ProgressScreen.hpp"
#include "ConsoleProgressScreen.hpp"
#include "CreditsScreen.hpp"

ScreenChooser::ScreenChooser(Minecraft* mc, UITheme uiTheme) :
	m_pMinecraft(mc)
	, m_uiTheme(uiTheme)
{
}

void ScreenChooser::pushStartScreen()
{
	m_pMinecraft->setScreen(new StartMenuScreen);
}

void ScreenChooser::pushPauseScreen()
{
	m_pMinecraft->setScreen(new PauseScreen);
}

void ScreenChooser::pushCreateWorldScreen(Screen* parent)
{
	m_pMinecraft->setScreen(new CreateWorldScreen(parent));
}

void ScreenChooser::pushOptionsScreen(Screen* parent)
{
	m_pMinecraft->setScreen(new OptionsScreen(parent));
}

void ScreenChooser::pushProgressScreen()
{
	m_pMinecraft->setScreen(new ProgressScreen);
}

void ScreenChooser::pushCraftingScreen(Player* player, const TilePos& pos)
{
	m_pMinecraft->setScreen(new CraftingScreen(player->m_pInventory, pos, player->m_pLevel));
}

void ScreenChooser::pushChestScreen(Player* player, Container* container)
{
	m_pMinecraft->setScreen(new ChestScreen(player->m_pInventory, container));
}

void ScreenChooser::pushCreditsScreen(Screen* parent)
{
	m_pMinecraft->setScreen(new CreditsScreen(parent));
}

ScreenChooser* ScreenChooser::create(Minecraft* mc)
{
	switch (mc->getOptions()->getUiTheme())
	{
	case UI_CONSOLE:
		return new ConsoleScreenChooser(mc);
	default:
		return new ScreenChooser(mc, mc->getOptions()->getUiTheme());
	}
}

ConsoleScreenChooser::ConsoleScreenChooser(Minecraft* mc) : ScreenChooser(mc, UI_CONSOLE)
{
}

void ConsoleScreenChooser::pushStartScreen()
{
	m_pMinecraft->setScreen(new ConsoleMainScreen);
}

void ConsoleScreenChooser::pushPauseScreen()
{
	m_pMinecraft->setScreen(new ConsolePauseScreen);
}

void ConsoleScreenChooser::pushOptionsScreen(Screen* parent)
{
	m_pMinecraft->setScreen(new ConsoleSettingsScreen(parent));
}

void ConsoleScreenChooser::pushProgressScreen()
{
	m_pMinecraft->setScreen(new ConsoleProgressScreen);
}

void ConsoleScreenChooser::pushCreateWorldScreen(Screen* parent)
{
	m_pMinecraft->setScreen(new ConsoleCreateWorldScreen(*m_pMinecraft->getOptions(), parent));
}

void ConsoleScreenChooser::pushCraftingScreen(Player* player, const TilePos& pos)
{
	m_pMinecraft->setScreen(new ConsoleClassicCraftingScreen(player->m_pInventory, pos, player->m_pLevel));
}
