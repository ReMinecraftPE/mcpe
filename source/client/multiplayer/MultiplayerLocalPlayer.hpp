#pragma once

#include "client/player/LocalPlayer.hpp"

class MultiplayerLocalPlayer : public LocalPlayer
{
public:
	MultiplayerLocalPlayer(Minecraft* pMinecraft, Level* pLevel, User* pUser, GameType gameType, int dimensionId);

protected:
	void reallyDrop(ItemEntity* itemEntity) override;

public:
	bool hurt(Entity*, int) override;
	void heal(int health) override;
	void drop() override;
	void hurtTo(int newHealth) override;

private:
	bool m_flashOnSetHealth;
};

