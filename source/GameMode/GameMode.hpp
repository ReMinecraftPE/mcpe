#pragma once

#include "Level.hpp"
#include "ItemInstance.hpp"

class Minecraft;

class GameMode
{
public:
	GameMode(Minecraft* pMinecraft);
	virtual ~GameMode();
	virtual void initLevel(Level*);
	virtual void startDestroyBlock(int, int, int, int);
	virtual bool destroyBlock(int, int, int, int);
	virtual void continueDestroyBlock(int, int, int, int);
	virtual void stopDestroyBlock();
	virtual void tick();
	virtual void render(float f);
	virtual float getPickRange();
	virtual bool useItem(Player*, Level*, ItemInstance*);
	virtual bool useItemOn(Player*, Level*, ItemInstance*, int, int, int, int);
	virtual LocalPlayer* createPlayer(Level*);
	virtual void initPlayer(Player*);
	virtual void adjustPlayer(Player*);
	virtual bool canHurtPlayer();
	virtual void interact(Player*, Entity*);
	virtual void attack(Player*, Entity*);
	virtual int handleInventoryMouseClick(int, int, int, Player*);
	virtual void handleCloseInventory(int, Player*);
	virtual bool isCreativeType();
	virtual bool isSurvivalType();

public:
	Minecraft* m_pMinecraft;
	uint8_t field_8 = 0;
};
