#include "MultiplayerLocalPlayer.hpp"
#include "network/RakNetInstance.hpp"
#include "world/level/Level.hpp"

MultiplayerLocalPlayer::MultiplayerLocalPlayer(Minecraft* pMinecraft, Level* pLevel, User* pUser, GameType gameType, int dimensionId)
	: LocalPlayer(pMinecraft, pLevel, pUser, gameType, dimensionId)
{
	m_flashOnSetHealth = false;
}

void MultiplayerLocalPlayer::reallyDrop(ItemEntity* itemEntity)
{
}

bool MultiplayerLocalPlayer::hurt(Entity* pEnt, int damage)
{
	return false;
}

void MultiplayerLocalPlayer::heal(int health)
{
}

void MultiplayerLocalPlayer::drop()
{
	//m_pLevel->m_pRakNetInstance->send(new PlayerActionPacket(PlayerActionPacket::DROP_ITEM))
}

void MultiplayerLocalPlayer::hurtTo(int newHealth)
{
    if (m_flashOnSetHealth)
    {
        LocalPlayer::hurtTo(newHealth);
    }
    else
    {
        m_health = newHealth;
        m_flashOnSetHealth = true;
    }
}