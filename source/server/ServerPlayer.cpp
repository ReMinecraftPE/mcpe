#include "ServerPlayer.hpp"
#include "network/packets/SetHealthPacket.hpp"
#include "network/packets/TakeItemEntityPacket.hpp"
#include "network/packets/SendInventoryPacket.hpp"
#include "network/RakNetInstance.hpp"
#include "world/level/Level.hpp"

ServerPlayer::ServerPlayer(Level* pLevel, GameType playerGameType)
	: Player(pLevel, playerGameType)
{
	m_lastHealth = -999; // -99999999 on Java

	m_pInventoryMenu->addSlotListener(this);
}

void ServerPlayer::tick()
{
	Player::tick();

	if (m_health != m_lastHealth)
	{
		m_pLevel->m_pRakNetInstance->send(m_guid, new SetHealthPacket(m_health));
		m_lastHealth = m_health;
	}
}

void ServerPlayer::take(Entity* pEnt, int count)
{
	m_pLevel->m_pRakNetInstance->send(new TakeItemEntityPacket(pEnt->m_EntityID, m_EntityID));

	Player::take(pEnt, count);
}

void ServerPlayer::refreshContainer(ContainerMenu* menu, const std::vector<ItemStack>& items)
{
	/*m_pLevel->m_pRakNetInstance->send(new ContainerSetContentPacket(menu->m_containerId, items));
	m_pLevel->m_pRakNetInstance->send(new ContainerSetSlotPacket(-1, -1, m_pInventory->getCarried()));*/
}

void ServerPlayer::slotChanged(ContainerMenu* menu, int index, ItemStack& item, bool isResultSlot)
{
	/*if (!isResultSlot)
	{
		m_pLevel->m_pRakNetInstance->send(new ContainerSetSlotPacket(menu->m_containerId, index, item));
	}*/
}

void ServerPlayer::setContainerData(ContainerMenu* menu, int id, int value)
{
	//m_pLevel->m_pRakNetInstance->send(new ContainerSetDataPacket(menu->m_containerId, id, value));
}