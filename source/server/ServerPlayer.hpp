#include "world/entity/Player.hpp"

class ServerPlayer : public Player
{
public:
	ServerPlayer(Level* pLevel, GameType playerGameType);

public:
	void tick() override;
	void take(Entity* pEnt, int count) override;
	BedSleepingProblem sleep(const TilePos& pos) override;
	void wake(bool resetCounter, bool update, bool setSpawn) override;

private:
	int m_lastHealth;
};
