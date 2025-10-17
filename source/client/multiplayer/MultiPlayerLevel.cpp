#include "MultiPlayerLevel.hpp"

MultiPlayerLevel::MultiPlayerLevel(LevelStorage* pStor, const std::string& name, const LevelSettings& settings, int storageVersion, Dimension* pDimension)
	: Level(pStor, name, settings, storageVersion, pDimension)
{
}

void MultiPlayerLevel::tick()
{
    _setTime(getTime() + 1); // Bypasses the normally-required update to LevelListeners
    updateSkyDarken();

    for (int i = 0; i < 10 && i < m_reEntries.size(); i++)
    {
        Entity* pEntity = m_reEntries[i];
        if (std::find(m_entities.begin(), m_entities.end(), pEntity) != m_entities.end())
        {
            addEntity(pEntity);
        }
    }

    for (std::vector<ResetInfo>::iterator it = m_updatesToReset.begin(); it != m_updatesToReset.end(); it++)
    {
        if (--it->ticks == 0)
        {
            setTileAndDataNoUpdate(it->pos, it->tile, it->data);
            sendTileUpdated(it->pos);
            m_updatesToReset.erase(it);
        }
    }
}

ChunkSource* MultiPlayerLevel::createChunkSource()
{
    return nullptr;
}

void MultiPlayerLevel::putEntity(int id, Entity* e)
{
    Entity* old = getEntity(id);
    if (old != nullptr)
    {
        removeEntity(old);
    }

    m_forced.push_back(e);
    e->m_EntityID = id;
    if (!addEntity(e))
    {
        m_reEntries.push_back(e);
    }

    m_entitiesById[id] = e;
}