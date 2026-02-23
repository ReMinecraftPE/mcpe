#pragma once

#include "ScreenChooser.hpp"

class ScreenChooser_Console : public ScreenChooser
{
public:
    ScreenChooser_Console(Minecraft*);
    ~ScreenChooser_Console();

public:
    void pushStartScreen() override;
    void pushPauseScreen() override;
    void pushOptionsScreen(Screen*) override;
    void pushProgressScreen() override;
    void pushCreateWorldScreen(Screen*) override;
    void pushCraftingScreen(Player*, const TilePos&) override;
};