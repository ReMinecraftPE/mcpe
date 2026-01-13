#include "ServerPlayer.hpp"
#include "network/packets/SetHealthPacket.hpp"
#include "network/packets/TakeItemEntityPacket.hpp"
#include "network/packets/AnimatePacket.hpp"
#include "network/packets/MovePlayerPacket.hpp"
#include "network/RakNetInstance.hpp"
#include "world/level/Level.hpp"
#include "world/tile/BedTile.hpp"

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

Player::BedSleepingProblem ServerPlayer::startSleepInBed(const TilePos& pos)
{
	Player::BedSleepingProblem result = Player::startSleepInBed(pos);
	
	// Broadcast position and sleep state to all clients if successful
	if (result == BED_SLEEPING_OK && m_pLevel && m_pLevel->m_pRakNetInstance)
	{
		// Send position update to ALL clients including self so they can find the bed
		// The position is the bed tile center
		m_pLevel->m_pRakNetInstance->send(new MovePlayerPacket(m_EntityID, Vec3(m_pos.x, m_pos.y - m_heightOffset, m_pos.z), m_rot));
		
		// Then send sleep animation to everyone
		m_pLevel->m_pRakNetInstance->send(new AnimatePacket(m_EntityID, AnimatePacket::SLEEP));
	}
	
	return result;
}

void ServerPlayer::stopSleepInBed(bool resetCounter, bool update, bool setSpawn)
{
	// Clear sleeping state FIRST so isImmobile() returns false
	m_bSleeping = false;
	if (resetCounter) {
		m_sleepTimer = 0;
	}
	
	// Reset size and height
	setSize(0.6f, 1.8f);
	setDefaultHeadHeight();
	
	// Clear bed occupancy
	if (m_bHasBedSleepPos && m_pLevel && m_pLevel->getTile(m_bedSleepPos) == Tile::bed->m_ID) {
		BedTile::setBedOccupied(m_pLevel, m_bedSleepPos, false);
	}
	
	if (setSpawn && m_bHasBedSleepPos) {
		setRespawnPos(m_bedSleepPos);
	}
	
	// Reset lerp steps so position updates work immediately
	m_lSteps = 0;
	
	// Broadcast wake animation to all clients
	if (m_pLevel && m_pLevel->m_pRakNetInstance)
	{
		m_pLevel->m_pRakNetInstance->send(new AnimatePacket(m_EntityID, AnimatePacket::WAKE));
	}
}
