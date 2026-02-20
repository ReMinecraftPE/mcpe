#include "Feature.hpp"
#include "world/level/Level.hpp"

using std::vector;
using std::array;

// Static array definition for axis conversion
const uint8_t FancyTreeFeature::axisConversionArray[] = {2, 0, 0, 1, 2, 1};

void FancyTreeFeature::generateBranchesAndTrunk()
{
    m_trunkHeight = Mth::Min((int)(m_height * m_trunkHeightScale), m_height - 1);

    int branchCount = Mth::Max((int)(1.382 + ((m_foliageDensity * m_height / 13.0) * (m_foliageDensity * m_height / 13.0))), 1);
    int maxBranches = branchCount * m_height;

    vector<array<int, 4> > foliageCoords;

    int topY = m_origin.y + m_height - m_foliageHeight;
    int trunkTopY = m_origin.y + m_trunkHeight;
    int offset = topY - m_origin.y;

    array<int, 4> initialBranch;
    initialBranch[0] = m_origin.x;
    initialBranch[1] = topY;
    initialBranch[2] = m_origin.z;
    initialBranch[3] = trunkTopY;
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
            double length = m_widthScale * spread * (m_rnd.nextFloat() + 0.328);
            double angle = m_rnd.nextFloat() * 2.0 * M_PI;

            int dx = (int)Mth::floor((float)(length * Mth::sin(angle) + m_origin.x) + 0.5f);
            int dz = (int)Mth::floor((float)(length * Mth::cos(angle) + m_origin.z) + 0.5f);
            TilePos branchPos(dx, topY, dz);
            TilePos branchTop = branchPos.above(m_foliageHeight);

            if (checkLine(branchPos, branchTop) == -1)
            {
                double dxDiff = (double)(m_origin.x - dx);
                double dzDiff = (double)(m_origin.z - dz);
                double dist = Mth::sqrt((dxDiff > 0.0 ? dxDiff : -dxDiff) * (dxDiff > 0.0 ? dxDiff : -dxDiff) + (dzDiff > 0.0 ? dzDiff : -dzDiff) * (dzDiff > 0.0 ? dzDiff : -dzDiff));
                double dy = dist * m_branchSlope;
                TilePos base(m_origin.x, (branchPos.y - dy > trunkTopY) ? trunkTopY : (int)(branchPos.y - dy), m_origin.z);

                if (checkLine(base, branchPos) == -1)
                {
                    array<int, 4> branchCoord;
                    branchCoord[0] = dx;
                    branchCoord[1] = topY;
                    branchCoord[2] = dz;
                    branchCoord[3] = base.y;
                    foliageCoords.push_back(branchCoord);
                }
            }
        }
        --topY;
        --offset;
    }

    for (size_t branchIdx = 0; branchIdx < foliageCoords.size(); ++branchIdx)
    {
        const array<int, 4>& branch = foliageCoords[branchIdx];
        foliageCluster(branch[0], branch[1], branch[2]);
    }

    makeTrunk();

    for (size_t branchIdx = 0; branchIdx < foliageCoords.size(); ++branchIdx)
    {
        const array<int, 4>& branch = foliageCoords[branchIdx];
        TilePos base(m_origin.x, branch[3], m_origin.z);
        if (trimBranches(base.y - m_origin.y))
        {
            limb(base, TilePos(branch[0], branch[1], branch[2]), Tile::treeTrunk->m_ID);
        }
    }
    foliageCoords.clear();
}

void FancyTreeFeature::crossection(int x, int y, int z, float radius, uint8_t majorAxis, int blockId)
{
    int radiusRounded = (int)((double)radius + 0.618);
    uint8_t axis2 = axisConversionArray[majorAxis];
    uint8_t axis3 = axisConversionArray[majorAxis + 3];
    TilePos center = TilePos(x, y, z);
    TilePos currentPos = center;
    int dx = -radiusRounded;
    int dz = -radiusRounded;

    for (int i = 0; i <= 2 * radiusRounded; ++i)
    {
        dx = -radiusRounded + i;
        getAxisCoord(currentPos, axis2) = getAxisCoord(center, axis2) + dx;
        dz = -radiusRounded;

        while (dz <= radiusRounded)
        {
            double distance = Mth::sqrt(((double)Mth::abs(dx) + 0.5) * ((double)Mth::abs(dx) + 0.5) + ((double)Mth::abs(dz) + 0.5) * ((double)Mth::abs(dz) + 0.5));
            if (distance > (double)radius)
            {
                ++dz;
            }
            else
            {
                getAxisCoord(currentPos, axis3) = getAxisCoord(center, axis3) + dz;
                int tileId = m_pLevel->getTile(currentPos);
                if (tileId != TILE_AIR && tileId != TILE_LEAVES)
                {
                    ++dz;
                }
                else
                {
                    m_pLevel->setTileNoUpdate(currentPos, blockId);
                    ++dz;
                }
            }
        }
    }
}

float FancyTreeFeature::treeShape(int offset)
{
    if ((double)offset < (double)((float)m_height) * 0.3) 
    {
        return -1.618f;
    }
    else 
    {
        float halfHeight = (float)m_height / 2.0;
        float offsetFromCenter = (float)m_height / 2.0 - (float)offset;
        float width;
        if (offsetFromCenter == 0.0f)
        {
            width = halfHeight;
        }
        else if (Mth::abs(offsetFromCenter) >= halfHeight)
        {
            width = 0.0f;
        }
        else
        {
            width = (float)Mth::sqrt((double)Mth::abs(halfHeight) * (double)Mth::abs(halfHeight) - (double)Mth::abs(offsetFromCenter) * (double)Mth::abs(offsetFromCenter));
        }
        return width / 2;
    }
}

float FancyTreeFeature::foliageShape(int layerOffset)
{
    return layerOffset >= 0 && layerOffset < m_foliageHeight ? (layerOffset != 0 && layerOffset != m_foliageHeight - 1 ? 3.0f : 2.0f) : -1.0f;
}

void FancyTreeFeature::foliageCluster(int x, int y, int z)
{
    int currentY = y;

    for (int endY = y + m_foliageHeight; currentY < endY; ++currentY)
    {
        float layerWidth = foliageShape(currentY - y);
        crossection(x, currentY, z, layerWidth, 1, Tile::leaves->m_ID);
    }
}

void FancyTreeFeature::limb(const TilePos& start, const TilePos& end, int blockId)
{
    TilePos delta;
    delta.x = end.x - start.x;
    delta.y = end.y - start.y;
    delta.z = end.z - start.z;
    
    // Find the axis with the largest absolute difference
    uint8_t majorAxis = 0;
    if (Mth::abs(delta.y) > Mth::abs(delta.x)) majorAxis = 1;
    if (Mth::abs(getAxisCoord(delta, 2)) > Mth::abs(getAxisCoord(delta, majorAxis))) majorAxis = 2;

    if (getAxisCoord(delta, majorAxis) != 0) {
        uint8_t axis2 = axisConversionArray[majorAxis];
        uint8_t axis3 = axisConversionArray[majorAxis + 3];
        int step = (getAxisCoord(delta, majorAxis) > 0) ? 1 : -1;

        double ratio1 = (double)getAxisCoord(delta, axis2) / (double)getAxisCoord(delta, majorAxis);
        double ratio2 = (double)getAxisCoord(delta, axis3) / (double)getAxisCoord(delta, majorAxis);
        TilePos currentPos;
        int counter = 0;
        int endCounter = getAxisCoord(delta, majorAxis) + step;

        while (counter != endCounter) {
            getAxisCoord(currentPos, majorAxis) = (int)Mth::floor((float)(getAxisCoord(start, majorAxis) + counter) + 0.5f);
            getAxisCoord(currentPos, axis2) = (int)Mth::floor((float)getAxisCoord(start, axis2) + (float)counter * (float)ratio1 + 0.5f);
            getAxisCoord(currentPos, axis3) = (int)Mth::floor((float)getAxisCoord(start, axis3) + (float)counter * (float)ratio2 + 0.5f);
            m_pLevel->setTileNoUpdate(currentPos, blockId);
            counter += step;
        }
    }
}

bool FancyTreeFeature::trimBranches(int heightOffset)
{
    return (double)heightOffset >= (double)m_height * 0.2;
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
    TilePos delta;
    delta.x = endPos.x - startPos.x;
    delta.y = endPos.y - startPos.y;
    delta.z = endPos.z - startPos.z;

    // Find the axis with the largest absolute difference
    uint8_t majorAxisIdx = 0;
    if (Mth::abs(delta.y) > Mth::abs(delta.x)) majorAxisIdx = 1;
    if (Mth::abs(getAxisCoord(delta, 2)) > Mth::abs(getAxisCoord(delta, majorAxisIdx))) majorAxisIdx = 2;

    if (getAxisCoord(delta, majorAxisIdx) == 0)
    {
        return -1;
    }
    else
    {
        uint8_t axis2 = axisConversionArray[majorAxisIdx];
        uint8_t axis3 = axisConversionArray[majorAxisIdx + 3];
        int step = (getAxisCoord(delta, majorAxisIdx) > 0) ? 1 : -1;

        double ratio1 = (double)getAxisCoord(delta, axis2) / (double)getAxisCoord(delta, majorAxisIdx);
        double ratio2 = (double)getAxisCoord(delta, axis3) / (double)getAxisCoord(delta, majorAxisIdx);
        TilePos currentPos;
        int counter = 0;
        int endCounter = getAxisCoord(delta, majorAxisIdx) + step;

        while (counter != endCounter)
        {
            getAxisCoord(currentPos, majorAxisIdx) = getAxisCoord(startPos, majorAxisIdx) + counter;
            getAxisCoord(currentPos, axis2) = Mth::floor((double)getAxisCoord(startPos, axis2) + (double)counter * ratio1);
            getAxisCoord(currentPos, axis3) = Mth::floor((double)getAxisCoord(startPos, axis3) + (double)counter * ratio2);
            int tileId = m_pLevel->getTile(currentPos);
            if (tileId != TILE_AIR && tileId != TILE_LEAVES)
            {
                return Mth::abs(counter);
            }
            counter += step;
        }
        return -1;
    }
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
    else
    {
        int obstacleDistance = checkLine(basePos, topPos);
        if (obstacleDistance == -1)
        {
            return true;
        }
        else if (obstacleDistance < 6)
        {
            return false;
        }
        else
        {
            m_height = obstacleDistance;
            return true;
        }
    }
}

void FancyTreeFeature::init(float density, float widthScale, float foliageDensity)
{
    m_heightVariance = (int)(density * 12.0);
    if (density > 0.5)
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
    else
    {
        generateBranchesAndTrunk();
        return true;
    }
}
