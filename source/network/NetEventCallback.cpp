#include "NetEventCallback.hpp"
#include "common/Logger.hpp"
#include "world/level/Level.hpp"

Player* NetEventCallback::_findPlayer(Level& level, Entity::ID entityId, const RakNet::RakNetGUID* guid)
{
    if (entityId != -1)
    {
        Entity* pEntity = level.getEntity(entityId);
        if (pEntity)
        {
            if (pEntity->isPlayer())
                return (Player*)pEntity;
        }
    }

    if (!guid || level.m_players.empty())
        return nullptr;

    for (int i = 0; i < level.m_players.size(); i++)
    {
        Player* pPlayer = level.m_players[i];
        if (pPlayer && pPlayer->m_guid == *guid)
            return pPlayer;
    }
    
    return nullptr;
}

void NetEventCallback::handle(Level& level, const RakNet::RakNetGUID& guid, RespawnPacket* pkt)
{
    Player* pPlayer = _findPlayer(level, pkt->m_entityId);
    if (!pPlayer)
    {
        LOG_W("NetEventCallback failed to find player to respawn with ID: %d", pkt->m_entityId);
        return;
    }

    // @TODO: on server, ignore client's requested coords, and teleport them to their server-determined spawn
    pPlayer->moveTo(pkt->m_pos);
    pPlayer->reset();
    pPlayer->resetPos(true);
}