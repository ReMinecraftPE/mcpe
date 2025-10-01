#include "Arrow.hpp"
#include "Mob.hpp"
#include "world/level/Level.hpp"
#include "nbt/CompoundTag.hpp"

const unsigned int Arrow::ARROW_BASE_DAMAGE = 4;

void Arrow::_init()
{
    m_pDescriptor = &EntityTypeDescriptor::arrow;
    field_C8 = RENDER_ARROW;
    setSize(0.5f, 0.5f);

    m_tilePos = Vec3(-1, -1, -1);
    m_lastTile = 0;
    m_lastTileData = 0;
    m_bInGround = false;
    m_bIsPlayerOwned = false;
    m_life = 0;
    m_flightTime = 0;
    m_shakeTime = 0;
    m_owner = nullptr;
}

Arrow::Arrow(Level* pLevel) : Entity(pLevel)
{
    _init();
}

Arrow::Arrow(Level* pLevel, const Vec3& pos) : Entity(pLevel)
{
    _init();

	setPos(pos);
}

Arrow::Arrow(Level* pLevel, Mob* pMob) : Entity(pLevel)
{
    _init();

    m_owner = pMob;
    m_bIsPlayerOwned = m_owner->isPlayer();
    moveTo(Vec3(pMob->m_pos.x, pMob->m_pos.y + pMob->getHeadHeight(), pMob->m_pos.z), Vec2(pMob->m_rot.y, pMob->m_rot.x));
    
    m_pos.x -= Mth::cos(m_rot.y / 180.0f * M_PI) * 0.16f;
    m_pos.y -= 0.1f;
    m_pos.z -= Mth::sin(m_rot.y / 180.0f * M_PI) * 0.16f;
    setPos(m_pos);

    m_vel.x = -Mth::sin(m_rot.y / 180.0f * M_PI) * Mth::cos(m_rot.x / 180.0f * M_PI);
    m_vel.z = Mth::cos(m_rot.y / 180.0f * M_PI) * Mth::cos(m_rot.x / 180.0f * M_PI);
    m_vel.y = -Mth::sin(m_rot.x / 180.0f * M_PI);
    shoot(m_vel, 1.5f, 1.0f);
}

void Arrow::shoot(Vec3 vel, float speed, float r)
{
    float len = vel.length();
    vel /= len;
    vel.x += sharedRandom.nextGaussian() * 0.0075f * r;
    vel.y += sharedRandom.nextGaussian() * 0.0075f * r;
    vel.z += sharedRandom.nextGaussian() * 0.0075f * r;
    vel *= speed;

    m_vel = vel;
    _lerpMotion(vel);

    m_life = 0;
}

void Arrow::_lerpMotion(const Vec3& vel)
{
    float len = vel.length();
    m_oRot.y = m_rot.y = Mth::atan2(vel.x, vel.z) * 180.0f / M_PI;
    m_oRot.x = m_rot.x = Mth::atan2(vel.y, len) * 180.0f / M_PI;
}

void Arrow::_lerpMotion2(const Vec3& vel)
{
    if (m_oRot.x == 0.0f && m_oRot.y == 0.0f)
    {
        return _lerpMotion(vel);
    }
}

void Arrow::lerpMotion(const Vec3& vel)
{
    m_vel = vel;

    _lerpMotion2(vel);
}

void Arrow::tick()
{
    Entity::tick();

    _lerpMotion2(m_vel);

    if (m_shakeTime > 0)
        --m_shakeTime;

    if (m_bInGround)
    {
        if (m_pLevel->getTile(m_tilePos) == m_lastTile)
        {
            ++m_life;
            if (m_life == 1200)
            {
                remove();
            }

            return;
        }

        m_bInGround = false;
        m_vel.x *= sharedRandom.nextFloat() * 0.2f;
        m_vel.y *= sharedRandom.nextFloat() * 0.2f;
        m_vel.z *= sharedRandom.nextFloat() * 0.2f;
        m_life = 0;
        m_flightTime = 0;
    }
    else 
    {
        ++m_flightTime;
    }

    Vec3 future_pos = m_pos + m_vel;
    HitResult hit_result = m_pLevel->clip(m_pos, future_pos);
    if (hit_result.isHit()) 
    {
        future_pos = hit_result.m_hitPos;
    }

    Entity* hit_ent = nullptr;
    AABB hitbox = m_hitbox;
    hitbox.expand(m_vel.x, m_vel.y, m_vel.z).grow(1.0f);
    EntityVector entities = m_pLevel->getEntities(this, hitbox);
    
    float max_dist = 0.0f;
    constexpr float var10 = 0.3f;
    for (EntityVector::iterator it = entities.begin(); it != entities.end(); it++)
    {
        Entity* ent = *it;
        if (ent->isPickable() && (ent != m_owner || m_flightTime >= 5)) 
        {
            AABB aabb = ent->m_hitbox;
            aabb.grow(var10);
            // these Vec3's are copied in the TilePos::clip fn, so no need to create them over and over like in b1.2
            HitResult hit = aabb.clip(m_pos, future_pos);
            if (hit.isHit())
            {
                float distance = m_pos.distanceTo(hit.m_hitPos);
                if (distance < max_dist || max_dist == 0.0f)
                {
                    hit_ent = ent;
                    max_dist = distance;
                }
            }
        }
    }

    if (hit_ent != nullptr)
    {
        hit_result = HitResult(hit_ent);
    }

    if (hit_result.isHit())
    {
        if (hit_result.m_pEnt != nullptr)
        {
            if (hit_result.m_pEnt->hurt(m_owner, ARROW_BASE_DAMAGE))
            {
                m_pLevel->playSound(this, "random.drr", 1.0f, 1.2f / (sharedRandom.nextFloat() * 0.2f + 0.9f));
                remove();
            }
            else 
            {
                m_vel *= -0.1f;
                m_rot.y += 180.0f;
                m_oRot.y += 180.0f;
                m_flightTime = 0;
            }
        }
        else 
        {
            m_tilePos = hit_result.m_tilePos;
            m_lastTile = m_pLevel->getTile(m_tilePos);
            m_vel = hit_result.m_hitPos - m_pos;
            m_pos -= (m_vel / m_pos.length() * 0.05f);
            m_pLevel->playSound(this, "random.drr", 1.0f, 1.2f / (sharedRandom.nextFloat() * 0.2f + 0.9f));
            m_bInGround = true;
            m_shakeTime = 7;
        }
    }

    m_pos += m_vel;
    float var17 = m_vel.length();
    m_rot.y = Mth::atan2(m_vel.x, m_vel.z) * 180.0f / M_PI;

    for (m_rot.x = Mth::atan2(m_vel.y, var17) * 180.0f / M_PI; m_rot.x - m_oRot.x < -180.0f; m_oRot.x -= 360.0f);

    while (m_rot.x - m_oRot.x >= 180.0f)
    {
        m_oRot.x += 360.0f;
    }

    while (m_rot.y - m_oRot.y < -180.0f)
    {
        m_oRot.y -= 360.0f;
    }

    while (m_rot.y - m_oRot.y >= 180.0f)
    {
        m_oRot.y += 360.0f;
    }

    m_rot.x = m_oRot.x + (m_rot.x - m_oRot.x) * 0.2f;
    m_rot.y = m_oRot.y + (m_rot.y - m_oRot.y) * 0.2f;
    float dampening = 0.99f;
    if (isInWater())
    {
        for (int var19 = 0; var19 < 4; ++var19)
        {
            constexpr float var20 = 0.25f;
            m_pLevel->addParticle("bubble", m_pos - m_vel * var20, m_pos * 1); // passed as reference so *1; although addParticle doesn't exist yet
        }

        dampening = 0.8f;
    }

    m_vel *= dampening;
    m_vel.y -= 0.03f;
    setPos(m_pos);
}

void Arrow::playerTouch(Player* pPlayer)
{
    if (!m_pLevel->m_bIsClientSide)
    {
        // had m_owner == pPlayer, but this logic breaks when loaded from a save, and m_owner is null
        if (m_bInGround && m_bIsPlayerOwned && m_shakeTime <= 0)
        {
            ItemInstance arrow(Item::arrow, 1);
            // they use ->add here in b1.2_02
            if (pPlayer->m_pInventory->addItem(arrow))
            {
                m_pLevel->playSound(this, "random.pop", 0.2f, ((sharedRandom.nextFloat() - sharedRandom.nextFloat()) * 0.7f + 1.0f) * 2.0f);
                pPlayer->take(this, 1);
                remove();
            }
        }
    }
}

void Arrow::addAdditionalSaveData(CompoundTag& tag) const
{
    tag.putInt16("xTile", m_tilePos.x);
    tag.putInt16("yTile", m_tilePos.y);
    tag.putInt16("zTile", m_tilePos.z);
    tag.putInt8("inTile", m_lastTile);
    tag.putInt8("inData", m_lastTileData);
    tag.putInt8("shake", m_shakeTime);
    tag.putBoolean("inGround", m_bInGround);
    tag.putBoolean("player", m_bIsPlayerOwned);
}

void Arrow::readAdditionalSaveData(const CompoundTag& tag)
{
    m_tilePos.x = tag.getInt16("xTile");
    m_tilePos.y = tag.getInt16("yTile");
    m_tilePos.z = tag.getInt16("zTile");
    m_lastTile = tag.getInt8("inTile") & 255;
    m_lastTileData = tag.getInt8("inData") & 255;
    m_shakeTime = tag.getInt8("shake") & 255;
    m_bInGround = tag.getBoolean("inGround");
    m_bIsPlayerOwned = tag.getBoolean("player");
}
