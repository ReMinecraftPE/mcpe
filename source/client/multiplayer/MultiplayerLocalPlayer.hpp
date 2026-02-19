#pragma once

#include "client/player/LocalPlayer.hpp"
#include "world/inventory/ContainerListener.hpp"

class MultiplayerLocalPlayer : public LocalPlayer, public ContainerListener
{
public:
	MultiplayerLocalPlayer(Minecraft* pMinecraft, Level* pLevel, User* pUser, GameType gameType, int dimensionId);

protected:
	void reallyDrop(ItemEntity* itemEntity) override;
	void _handleOpenedContainerMenu() override;

public:
	bool hurt(Entity*, int) override;
	void heal(int health) override;
	//void drop() override;
	void hurtTo(int newHealth) override;
	void die(Entity* pCulprit) override;
	void drop(const ItemStack& item, bool randomly = false) override;
	void closeContainer() override;

	void refreshContainer(ContainerMenu* menu, const std::vector<ItemStack>& items) override;
	void slotChanged(ContainerMenu* menu, int index, Slot* slot, ItemStack& item, bool isResultSlot) override;

private:
	bool m_flashOnSetHealth;
};

