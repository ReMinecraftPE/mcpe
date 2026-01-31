#include "NetEventCallback.hpp"
#include "common/Logger.hpp"
#include "world/level/Level.hpp"
#include "world/tile/BedTile.hpp"

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

    for (size_t i = 0; i < level.m_players.size(); i++)
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

    TilePos spawnPos = level.getSharedSpawnPos();
    
    pPlayer->moveTo(Vec3(spawnPos.x + 0.5f, float(spawnPos.y), spawnPos.z + 0.5f));
    pPlayer->reset();
    pPlayer->resetPos(true);
}
