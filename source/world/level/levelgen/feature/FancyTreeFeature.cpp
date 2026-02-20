#include "Feature.hpp"
#include "world/level/Level.hpp"
#include <vector>

struct FoliageCoord
{
    int x, y, z, branchY;
};

FancyTreeFeature::FancyTreeFeature() :
    m_pLevel(NULL),
    m_origin(),
    m_height(0),
    m_trunkHeight(0),
    m_trunkHeightScale(0.618f),
    m_branchDensity(1.0f),
    m_branchSlope(0.381f),
    m_widthScale(1.0f),
    m_foliageDensity(1.0f),
    m_trunkWidth(1),
    m_heightVariance(12),
    m_foliageHeight(4)
{
}

FancyTreeFeature::~FancyTreeFeature()
{
}

int& FancyTreeFeature::getAxisCoord(TilePos& pos, uint8_t axis)
{
    switch (axis)
    {
        case 0:  return pos.x;
        case 1:  return pos.y;
        case 2:  return pos.z;
        default: return pos.x;
    }
}

int FancyTreeFeature::getAxisCoord(const TilePos& pos, uint8_t axis)
{
    switch (axis)
    {
        case 0:  return pos.x;
        case 1:  return pos.y;
        case 2:  return pos.z;
        default: return pos.x;
    }
}

void FancyTreeFeature::generateBranchesAndTrunk()
{
    m_trunkHeight = Mth::Min(static_cast<int>(m_height * m_trunkHeightScale), m_height - 1);

    int branchCount = Mth::Max(static_cast<int>(1.382f + ((m_foliageDensity * m_height / 13.0f) * (m_foliageDensity * m_height / 13.0f))), 1);
    
    std::vector<FoliageCoord> foliageCoords;

    int topY = m_origin.y + m_height - m_foliageHeight;
    int trunkTopY = m_origin.y + m_trunkHeight;
    int offset = topY - m_origin.y;

    FoliageCoord initialBranch = {m_origin.x, topY, m_origin.z, trunkTopY};
    foliageCoords.push_back(initialBranch);
    --topY;

    while (offset >= 0)
    {
        float spread = treeShape(offset);
        if (spread < 0.0f)
        {
            --topY;
            --offset;
            continue;
        }

        for (int i = 0; i < branchCount; ++i)
        {
            float length = m_widthScale * spread * (m_rnd.nextFloat() + 0.328f);
            float angle = m_rnd.nextFloat() * 2.0f * M_PI;

            int dx = static_cast<int>(Mth::floor((length * Mth::sin(angle) + m_origin.x) + 0.5f));
            int dz = static_cast<int>(Mth::floor((length * Mth::cos(angle) + m_origin.z) + 0.5f));
            
            TilePos branchPos(dx, topY, dz);
            TilePos branchTop = branchPos.above(m_foliageHeight);

            if (checkLine(branchPos, branchTop) == -1)
            {
                float dxDiff = static_cast<float>(m_origin.x - dx);
                float dzDiff = static_cast<float>(m_origin.z - dz);
                
                float dist = Mth::sqrt(dxDiff * dxDiff + dzDiff * dzDiff);
                float dy = dist * m_branchSlope;
                
                TilePos base(m_origin.x, (branchPos.y - dy > trunkTopY) ? trunkTopY : static_cast<int>(branchPos.y - dy), m_origin.z);

                if (checkLine(base, branchPos) == -1)
                {
                    FoliageCoord branchCoord = {dx, topY, dz, base.y};
                    foliageCoords.push_back(branchCoord);
                }
            }
        }
        --topY;
        --offset;
    }

    for (size_t i = 0; i < foliageCoords.size(); ++i)
    {
        const FoliageCoord& branch = foliageCoords[i];
        foliageCluster(branch.x, branch.y, branch.z);
    }

    makeTrunk();

    for (size_t i = 0; i < foliageCoords.size(); ++i)
    {
        const FoliageCoord& branch = foliageCoords[i];
        TilePos base(m_origin.x, branch.branchY, m_origin.z);
        if (trimBranches(base.y - m_origin.y))
        {
            limb(base, TilePos(branch.x, branch.y, branch.z), Tile::treeTrunk->m_ID);
        }
    }
}

void FancyTreeFeature::crossection(int x, int y, int z, float radius, uint8_t majorAxis, int blockId)
{
    int radiusRounded = static_cast<int>(radius + 0.618f);
    uint8_t axis2 = axisConversionArray[majorAxis];
    uint8_t axis3 = axisConversionArray[majorAxis + 3];
    
    TilePos center(x, y, z);
    TilePos currentPos = center;

    for (int dx = -radiusRounded; dx <= radiusRounded; ++dx)
    {
        getAxisCoord(currentPos, axis2) = getAxisCoord(center, axis2) + dx;

        for (int dz = -radiusRounded; dz <= radiusRounded; ++dz)
        {
            float absDx = Mth::abs(static_cast<float>(dx)) + 0.5f;
            float absDz = Mth::abs(static_cast<float>(dz)) + 0.5f;
            float distance = Mth::sqrt(absDx * absDx + absDz * absDz);

            if (distance <= radius)
            {
                getAxisCoord(currentPos, axis3) = getAxisCoord(center, axis3) + dz;
                int tileId = m_pLevel->getTile(currentPos);
                
                if (tileId == TILE_AIR || tileId == TILE_LEAVES)
                {
                    m_pLevel->setTileNoUpdate(currentPos, blockId);
                }
            }
        }
    }
}

float FancyTreeFeature::treeShape(int offset)
{
    if (static_cast<float>(offset) < static_cast<float>(m_height) * 0.3f) 
    {
        return -1.618f;
    }

    float halfHeight = static_cast<float>(m_height) / 2.0f;
    float offsetFromCenter = halfHeight - static_cast<float>(offset);
    
    if (offsetFromCenter == 0.0f)
    {
        return halfHeight / 2.0f;
    }
    
    if (Mth::abs(offsetFromCenter) >= halfHeight)
    {
        return 0.0f;
    }

    float width = Mth::sqrt(halfHeight * halfHeight - offsetFromCenter * offsetFromCenter);
    return width / 2.0f;
}

float FancyTreeFeature::foliageShape(int layerOffset)
{
    if (layerOffset >= 0 && layerOffset < m_foliageHeight)
    {
        return (layerOffset != 0 && layerOffset != m_foliageHeight - 1) ? 3.0f : 2.0f;
    }
    return -1.0f;
}

void FancyTreeFeature::foliageCluster(int x, int y, int z)
{
    int endY = y + m_foliageHeight;
    for (int currentY = y; currentY < endY; ++currentY)
    {
        float layerWidth = foliageShape(currentY - y);
        crossection(x, currentY, z, layerWidth, 1, Tile::leaves->m_ID);
    }
}

void FancyTreeFeature::limb(const TilePos& start, const TilePos& end, int blockId)
{
    TilePos delta(end.x - start.x, end.y - start.y, end.z - start.z);
    
    uint8_t majorAxis = 0;
    if (Mth::abs(delta.y) > Mth::abs(delta.x)) majorAxis = 1;
    if (Mth::abs(delta.z) > Mth::abs(getAxisCoord(delta, majorAxis))) majorAxis = 2;

    int majorDelta = getAxisCoord(delta, majorAxis);

    if (majorDelta != 0)
    {
        uint8_t axis2 = axisConversionArray[majorAxis];
        uint8_t axis3 = axisConversionArray[majorAxis + 3];
        int step = (majorDelta > 0) ? 1 : -1;

        float ratio1 = static_cast<float>(getAxisCoord(delta, axis2)) / static_cast<float>(majorDelta);
        float ratio2 = static_cast<float>(getAxisCoord(delta, axis3)) / static_cast<float>(majorDelta);
        
        TilePos currentPos;
        int endCounter = majorDelta + step;

        for (int counter = 0; counter != endCounter; counter += step)
        {
            getAxisCoord(currentPos, majorAxis) = static_cast<int>(Mth::floor(static_cast<float>(getAxisCoord(start, majorAxis) + counter) + 0.5f));
            getAxisCoord(currentPos, axis2) = static_cast<int>(Mth::floor(static_cast<float>(getAxisCoord(start, axis2)) + static_cast<float>(counter) * ratio1 + 0.5f));
            getAxisCoord(currentPos, axis3) = static_cast<int>(Mth::floor(static_cast<float>(getAxisCoord(start, axis3)) + static_cast<float>(counter) * ratio2 + 0.5f));
            
            m_pLevel->setTileNoUpdate(currentPos, blockId);
        }
    }
}

bool FancyTreeFeature::trimBranches(int heightOffset)
{
    return static_cast<float>(heightOffset) >= static_cast<float>(m_height) * 0.2f;
}

void FancyTreeFeature::makeTrunk()
{
    TilePos top = m_origin.above(m_trunkHeight);
    limb(m_origin, top, Tile::treeTrunk->m_ID);

    if (m_trunkWidth == 2)
    {
        TilePos x1(m_origin.x + 1, m_origin.y, m_origin.z);
        TilePos x2(m_origin.x + 1, m_origin.y, m_origin.z + 1);
        TilePos x3(m_origin.x, m_origin.y, m_origin.z + 1);
        
        limb(x1, x1.above(m_trunkHeight), Tile::treeTrunk->m_ID);
        limb(x2, x2.above(m_trunkHeight), Tile::treeTrunk->m_ID);
        limb(x3, x3.above(m_trunkHeight), Tile::treeTrunk->m_ID);
    }
}

int FancyTreeFeature::checkLine(TilePos& startPos, TilePos& endPos)
{
    TilePos delta(endPos.x - startPos.x, endPos.y - startPos.y, endPos.z - startPos.z);

    uint8_t majorAxisIdx = 0;
    if (Mth::abs(delta.y) > Mth::abs(delta.x)) majorAxisIdx = 1;
    if (Mth::abs(delta.z) > Mth::abs(getAxisCoord(delta, majorAxisIdx))) majorAxisIdx = 2;

    int majorDelta = getAxisCoord(delta, majorAxisIdx);

    if (majorDelta == 0)
    {
        return -1;
    }

    uint8_t axis2 = axisConversionArray[majorAxisIdx];
    uint8_t axis3 = axisConversionArray[majorAxisIdx + 3];
    int step = (majorDelta > 0) ? 1 : -1;

    float ratio1 = static_cast<float>(getAxisCoord(delta, axis2)) / static_cast<float>(majorDelta);
    float ratio2 = static_cast<float>(getAxisCoord(delta, axis3)) / static_cast<float>(majorDelta);
    
    TilePos currentPos;
    int endCounter = majorDelta + step;

    for (int counter = 0; counter != endCounter; counter += step)
    {
        getAxisCoord(currentPos, majorAxisIdx) = getAxisCoord(startPos, majorAxisIdx) + counter;
        getAxisCoord(currentPos, axis2) = static_cast<int>(Mth::floor(static_cast<float>(getAxisCoord(startPos, axis2)) + static_cast<float>(counter) * ratio1));
        getAxisCoord(currentPos, axis3) = static_cast<int>(Mth::floor(static_cast<float>(getAxisCoord(startPos, axis3)) + static_cast<float>(counter) * ratio2));
        
        int tileId = m_pLevel->getTile(currentPos);
        if (tileId != TILE_AIR && tileId != TILE_LEAVES)
        {
            return Mth::abs(counter);
        }
    }
    
    return -1;
}

bool FancyTreeFeature::checkLocation()
{
    TilePos basePos(m_origin);
    TilePos topPos = m_origin.above(m_height - 1);
    int groundTileId = m_pLevel->getTile(m_origin.below());
    
    if (groundTileId != 2 && groundTileId != 3)
    {
        return false;
    }

    int obstacleDistance = checkLine(basePos, topPos);
    if (obstacleDistance == -1)
    {
        return true;
    }
    
    if (obstacleDistance < 6)
    {
        return false;
    }

    m_height = obstacleDistance;
    return true;
}

void FancyTreeFeature::init(float density, float widthScale, float foliageDensity)
{
    m_heightVariance = static_cast<int>(density * 12.0f);
    if (density > 0.5f)
    {
        m_foliageHeight = 5;
    }

    m_widthScale = widthScale;
    m_foliageDensity = foliageDensity;
}

bool FancyTreeFeature::place(Level* level, Random* random, const TilePos& pos)
{
    m_pLevel = level;
    m_rnd.setSeed(random->nextLong());
    m_origin = pos;
    
    if (!m_height)
    {
        m_height = 5 + m_rnd.nextInt(m_heightVariance);
    }

    if (!checkLocation())
    {
        return false;
    }

    generateBranchesAndTrunk();
    return true;
}