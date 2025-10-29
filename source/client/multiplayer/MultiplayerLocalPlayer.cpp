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

bool MultiplayerLocalPlayer::hurt(Entity* pAttacker, int damage)
{
    // Java returns false
	return false;

    // Pulled from Mob::hurt(), modified to remove impact on health.
    // @BUG: Will never work, because EntityEventPacket sets m_invulnerableTime
    // before InteractPacket is received and calls this.
    // If we remove the m_invulnerableTime check, the player can then be
    // knocked back despite being invulnerable

    /*if (isCreative())
        return false;

    bool var3 = true;
    if (float(m_invulnerableTime) > float(m_invulnerableDuration) / 2.0f)
    {
        var3 = false;
    }
    else
    {
        m_invulnerableTime = m_invulnerableDuration;
        m_hurtTime = m_hurtDuration = 10;
    }

    m_hurtDir = 0.0f;
    if (var3)
    {
        markHurt();

        if (pAttacker)
        {
            float xd = pAttacker->m_pos.x - m_pos.x;
            float zd = pAttacker->m_pos.z - m_pos.z;

            while (zd * zd + xd * xd < 0.0001f)
            {
                xd = 0.01f * (Mth::random() - Mth::random());
                zd = 0.01f * (Mth::random() - Mth::random());
            }

            float ang = atan2f(zd, xd);
            v020_field_104 = ang * (180.0f / float(M_PI)) - m_rot.x;

            knockback(pAttacker, damage, xd, zd);
        }
    }

    return true;*/
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