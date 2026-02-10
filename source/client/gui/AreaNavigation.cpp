#include <cfloat>
#include "common/Utils.hpp"
#include "AreaNavigation.hpp"
#include "common/Mth.hpp"

AreaNavigation::ID AreaNavigation::navigate(Direction dir, int x, int y, bool cycle)
{
    m_index = -1;

    float bestScore = FLT_MAX;
    int nearest = -1;

    float dirX = 0, dirY = 0;
    switch (dir)
    {
    case UP:    dirY = -1; break;
    case DOWN:  dirY = 1; break;
    case LEFT:  dirX = -1; break;
    case RIGHT: dirX = 1; break;
    default: return -1;
    }

    if (cycle)
    {
        if (dirX) x = dirX < 0 ? m_area.w : m_area.x;
        else if (dirY) y = dirY < 0 ? m_area.h : m_area.y;
    }

    int elementX = 0;
    int elementY = 0;

    while (next(elementX, elementY, cycle))
    {
        constexpr float maxAngle = 75.0f;
        constexpr float penalty = 1.5f;

        int tx = elementX - x;
        int ty = elementY - y;

        float distance = Mth::sqrt(tx * tx + ty * ty);
        if (distance < 1e-4f) continue;

        float len = distance;
        float nx = tx / len;
        float ny = ty / len;

        float cosTheta = nx * dirX + ny * dirY;

        if (cosTheta < Mth::cos(maxAngle * M_PI / 180.0f))
            continue;

        float projectedDist = distance * cosTheta;

        float lateralPenalty = distance * Mth::sqrt(1.0f - cosTheta * cosTheta) * penalty;

        float score = projectedDist + lateralPenalty;

        if (score < bestScore)
        {
            bestScore = score;
            nearest = m_index;
        }
    }

    return nearest;
}

AreaNavigation::ID AreaNavigation::navigateCyclic(Direction dir, int x, int y)
{
    AreaNavigation::ID index = navigate(dir, x, y);

    if (index < 0)
        index = navigate(dir, x, y, true);

    return isValid(index) ? index : -1;
}

bool AreaNavigation::isValid(ID id)
{
    return true;
}
