#include "ServerPlayer.hpp"
#include "network/packets/SetHealthPacket.hpp"
#include "network/packets/TakeItemEntityPacket.hpp"
#include "network/RakNetInstance.hpp"
#include "world/level/Level.hpp"

ServerPlayer::ServerPlayer(Level* pLevel, GameType playerGameType)
	: Player(pLevel, playerGameType)
{
	m_lastHealth = -999; // -99999999 on Java
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