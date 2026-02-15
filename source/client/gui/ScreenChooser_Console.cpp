#include "ScreenChooser_Console.hpp"
#include "client/app/Minecraft.hpp"

#include "screens/StartMenuScreen_Console.hpp"
#include "screens/PauseScreen_Console.hpp"
#include "screens/inventory/ClassicCraftingScreen_Console.hpp"
#include "screens/OptionsScreen_Console.hpp"
#include "screens/CreateWorldScreen_Console.hpp"
#include "screens/ProgressScreen_Console.hpp"

ScreenChooser_Console::ScreenChooser_Console(Minecraft* mc) : ScreenChooser(mc, UI_CONSOLE)
{
}

ScreenChooser_Console::~ScreenChooser_Console()
{
}

void ScreenChooser_Console::pushStartScreen()
{
	m_pMinecraft->setScreen(new StartMenuScreen_Console);
}

void ScreenChooser_Console::pushPauseScreen()
{
	m_pMinecraft->setScreen(new PauseScreen_Console);
}

void ScreenChooser_Console::pushOptionsScreen(Screen* parent)
{
	m_pMinecraft->setScreen(new OptionsScreen_Console(parent));
}

void ScreenChooser_Console::pushProgressScreen()
{
	m_pMinecraft->setScreen(new ProgressScreen_Console);
}

void ScreenChooser_Console::pushCreateWorldScreen(Screen* parent)
{
	m_pMinecraft->setScreen(new CreateWorldScreen_Console(*m_pMinecraft->getOptions(), parent));
}

void ScreenChooser_Console::pushCraftingScreen(Player* player, const TilePos& pos)
{
	m_pMinecraft->setScreen(new ClassicCraftingScreen_Console(player->m_pInventory, pos, player->m_pLevel));
}
