#include "world/entity/Player.hpp"

class ServerPlayer : public Player
{
public:
	ServerPlayer(Level* pLevel, GameType playerGameType);

public:
	void tick() override;
	void take(Entity* pEnt, int x) override;

private:
	int m_lastHealth;
};