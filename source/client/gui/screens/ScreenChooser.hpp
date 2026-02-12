#pragma once

struct TilePos;
enum UITheme;
class Player;
class Minecraft;
class Container;
class Screen;

//@NOTE: This is just based on MCPE, not really a decompilation, make it accurate if necessary
class ScreenChooser
{
public:
    ScreenChooser(Minecraft*, UITheme uiTheme);

    virtual void pushStartScreen();
    virtual void pushPauseScreen();
    virtual void pushCreateWorldScreen(Screen*);
    virtual void pushOptionsScreen(Screen*);
    virtual void pushProgressScreen();
    virtual void pushCraftingScreen(Player*, const TilePos&); //pushWorkbenchScreen
    virtual void pushChestScreen(Player*, Container*);
    virtual void pushCreditsScreen(Screen*);

    static ScreenChooser* create(Minecraft*);

protected:
    Minecraft* m_pMinecraft;

public:
    UITheme m_uiTheme;
};

class ConsoleScreenChooser : public ScreenChooser
{
public:
    ConsoleScreenChooser(Minecraft*);

    void pushStartScreen() override;
    void pushPauseScreen() override;
    void pushOptionsScreen(Screen*) override;
    void pushProgressScreen() override;
    void pushCreateWorldScreen(Screen*) override;
    void pushCraftingScreen(Player*, const TilePos&) override;
};