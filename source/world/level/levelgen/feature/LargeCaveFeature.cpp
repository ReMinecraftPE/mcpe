#include "LargeCaveFeature.hpp"

void LargeCaveFeature::addFeature(Level* level, int ax, int az, int x, int z, TileID* tiles, int unk)
{
    int x1 = m_random.nextInt(m_random.nextInt(m_random.nextInt(40) + 1) + 1);

    if (m_random.nextInt(15) != 0)
        x1 = 0;

    for (int i = 0; i < x1; i++)
    {
        float rx = float(16 * ax + m_random.nextInt(16));
        float ry = float(m_random.nextInt(m_random.nextInt(120) + 8));
        float rz = float(16 * ax + m_random.nextInt(16));

        int nTunnels = 1;
        if (m_random.nextInt(4) == 0)
        {
            addRoom(x, z, tiles, rx, ry, rz);
            nTunnels = 1 + m_random.nextInt(4);
        }

        for (int j = 0; j < nTunnels; j++)
        {
            float ang1 = 2.0f * float(M_PI) * m_random.nextFloat();
            float x2 = (2.0f * (m_random.nextFloat() - 0.5f)) / 8.0f;
            float step = (2.0f * m_random.nextFloat()) + m_random.nextFloat();
            addTunnel(x, z, tiles, rx, ry, rz, step, ang1, x2, 0, 0, 1.0f);
        }
    }
}

void LargeCaveFeature::addRoom(int x, int z, TileID* tiles, float rx, float ry, float rz)
{
    addTunnel(x, z, tiles, rx, ry, rz, 1.0f + 6.0f * m_random.nextFloat(), 0.0f, 0.0f, -1, -1, 0.5f);
}

void LargeCaveFeature::addTunnel(int x, int z, TileID* tiles, float rx, float ry, float rz, float x1, float ang, float x2, int step, int dist, float x5)
{
    float xMid = float(8 * (2 * x + 1));
    float zMid = float(8 * (2 * z + 1));

    float yRota = 0.0;
    float xRota = 0.0;
    Random random(m_random.nextLong());

    if (dist <= 0)
    {
        int max = m_radius * 16 - 16;
        dist = max - random.nextInt(max / 4);
    }

    bool singleStep = false;
    if (step == -1)
    {
        step = dist / 2;
        singleStep = true;
    }

    int splitPoint = random.nextInt(dist / 2) + dist / 4;
    bool steep = random.nextInt(6) == 0;
    for (; step < dist; step++)
    {
        float rad = sin((step * float(M_PI)) / (float)dist) * x1 + 1.5f;
        float yRad = rad * x5;
        float cos_pitch = cos(x2);
        rx += (float)(cos(ang) * cos_pitch);
        ry += sin(x2);
        rz = rz + (float)(sin(ang) * cos_pitch);
        float a10a;
        if (steep)
            a10a = x2 * 0.92f;
        else
            a10a = x2 * 0.7f;

        x2 = a10a + (float)(xRota * 0.1f);
        ang = ang + (float)(yRota * 0.1f);
        xRota = xRota * 0.9f;
        yRota = yRota * 0.75f;
        float v18 = random.nextFloat() - random.nextFloat();
        float v19 = random.nextFloat();
        xRota += 2 * v18 * v19;
        float v21 = random.nextFloat() - random.nextFloat();
        float v22 = random.nextFloat();
        yRota += (v21 * v22) * 4.0f;
        if (singleStep != 1 && step == splitPoint && x1 > 1.0f)
        {
            addTunnel(
                x,
                z,
                tiles,
                rx,
                ry,
                rz,
                (random.nextFloat() * 0.5f) + 0.5f,
                (float(M_PI) / -2.0f) + ang,
                x2 / 3.0f,
                step,
                dist,
                1.0f);
            addTunnel(
                x,
                z,
                tiles,
                rx,
                ry,
                rz,
                (random.nextFloat() * 0.5f) + 0.5f,
                (float(M_PI) / 2.0f) + ang,
                x2 / 3.0f,
                step,
                dist,
                1.0f);
            return;
        }
        if (singleStep == 1 || random.nextInt(4) != 0)
        {
            float v55 = rx - xMid;
            float v54 = rz - zMid;
            float v53 = (dist - step);
            float v52 = (x1 + 2.0f) + 16.0f;
            if (((((rx - xMid) * (rx - xMid))
                + ((rz - zMid) * (rz - zMid)))
                - (v53 * v53)) > (v52 * v52))
                return;
            if (((xMid - 16.0f) + (rad * -2.0f)) <= rx &&
                ((zMid - 16.0f) + (rad * -2.0f)) <= rz &&
                ((xMid + 16.0f) + (rad * +2.0f)) >= rx &&
                ((zMid + 16.0f) + (rad * +2.0f)) >= rz)
            {
                int v51 = -16 * x + (int)floor(rx - rad) - 1;
                int v50 = -16 * x + (int)floor(rx + rad) + 1;
                int v49 = (int)floor(ry - yRad) - 1;
                int v48 = (int)floor(ry + yRad) + 1;
                int v47 = -16 * z + (int)floor(rz - rad) - 1;
                int v46 = -16 * z + (int)floor(rz + rad) + 1;
                if (v51 < 0)   v51 = 0;
                if (v50 > 16)  v50 = 16;
                if (v49 <= 0)  v49 = 1;
                if (v48 > 120) v48 = 120;
                if (v47 < 0)   v47 = 0;
                if (v46 > 16)  v46 = 16;
                bool v67 = 0;
                for (int i = v51; v67 != 1 && i < v50; ++i)
                {
                    for (int j = v47; v67 != 1 && j < v46; ++j)
                    {
                        for (int k = v48 + 1; v67 != 1 && v49 - 1 <= k; --k)
                        {
                            int v42 = k + ((j + 16 * i) << 7);
                            if (k >= 0 && k <= 127)
                            {
                                if (tiles[v42] == Tile::water->m_ID || tiles[v42] == Tile::calmWater->m_ID)
                                    v67 = 1;
                                if (v49 - 1 != k && i != v51 && v50 - 1 != i && j != v47 && v46 - 1 != j)
                                    k = v49;
                            }
                        }
                    }
                }
                if (!v67)
                {
                    for (int l = v51; l < v50; ++l)
                    {
                        float v40 = ((float(l + 16 * x) + 0.5f) - rx) / rad;
                        for (int m = v47; m < v46; ++m)
                        {
                            float v38 = ((float(m + 16 * z) + 0.5f) - rz) / rad;
                            int v37 = v48 + ((m + 16 * l) << 7);
                            bool v66 = 0;
                            if ((float)((float)(v40 * v40) + (float)(v38 * v38)) < 1.0f)
                            {
                                for (int n = v48 - 1; n >= v49; --n)
                                {
                                    float v35 = ((float(n) + 0.5) - ry) / yRad;
                                    if (v35 > -0.7f
                                        && v40 * v40 + v35 * v35 + v38 * v38 < 1.0f)
                                    {
                                        TileID v34 = tiles[v37];
                                        if (Tile::grass->m_ID == v34)
                                            v66 = 1;
                                        if (Tile::rock->m_ID == v34 ||
                                            Tile::dirt->m_ID == v34 ||
                                            Tile::grass->m_ID == v34)
                                        {
                                            if (n > 9)
                                            {
                                                tiles[v37] = 0;
                                                if (v66)
                                                {
                                                    if (tiles[v37 - 1] == Tile::dirt->m_ID)
                                                        tiles[v37 - 1] = Tile::grass->m_ID;
                                                }
                                            }
                                            else
                                            {
                                                tiles[v37] = Tile::lava->m_ID;
                                            }
                                        }
                                    }
                                    --v37;
                                }
                            }
                        }
                    }
                    if (singleStep)
                        return;
                }
            }
        }
    }
}
