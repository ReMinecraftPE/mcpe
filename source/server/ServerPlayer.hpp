#include "world/entity/Player.hpp"
#include "world/ContainerListener.hpp"

class ServerPlayer : public Player, public ContainerListener
{
public:
	ServerPlayer(Level* pLevel, GameType playerGameType);

protected:
	void _nextContainerCounter();

public:
	void tick() override;
	void startCrafting(const TilePos& pos) override;
	void openContainer(Container* container) override;
	void closeContainer() override;
	void take(Entity* pEnt, int count) override;
	BedSleepingProblem startSleepInBed(const TilePos& pos) override;
	void stopSleepInBed(bool resetCounter, bool update, bool setSpawn) override;

	void refreshContainer(ContainerMenu* menu, const std::vector<ItemStack>& items) override;
	void slotChanged(ContainerMenu* menu, int index, ItemStack& item, bool isResultSlot) override;
	void setContainerData(ContainerMenu* menu, int id, int value) override;

	void doCloseContainer();
	void setContainerMenu(ContainerMenu* menu);

private:
	int m_lastHealth;
	int m_containerId;
};
