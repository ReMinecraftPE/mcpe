#pragma once

#include "client/options/Options.hpp"

struct TilePos;
class Player;
class Minecraft;
class Container;
class Screen;
class FurnaceTileEntity;

//@NOTE: This is just based on MCPE, not really a decompilation, make it accurate if necessary
class ScreenChooser
{
public:
    ScreenChooser(Minecraft*, UITheme uiTheme);
    virtual ~ScreenChooser();

public:
    virtual void pushStartScreen();
    virtual void pushPauseScreen();
    virtual void pushCreateWorldScreen(Screen*);
    virtual void pushOptionsScreen(Screen*);
    virtual void pushProgressScreen();
    virtual void pushCraftingScreen(Player*, const TilePos&); // originally pushWorkbenchScreen
    virtual void pushFurnaceScreen(Player*, FurnaceTileEntity*);
    virtual void pushChestScreen(Player*, Container*);
    virtual void pushCreditsScreen(Screen*);

    static ScreenChooser* Create(Minecraft*);

protected:
    Minecraft* m_pMinecraft;

public:
    UITheme m_uiTheme;
};
