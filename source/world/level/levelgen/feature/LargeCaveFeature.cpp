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
			float x3 = (2.0f * m_random.nextFloat()) + m_random.nextFloat();
			addTunnel(x, z, tiles, rx, ry, rz, x3, ang1, x2, 0, 0, 1.0f);
		}
	}
}

void LargeCaveFeature::addRoom(int x, int z, TileID* tiles, float rx, float ry, float rz)
{
	addTunnel(x, z, tiles, rx, ry, rz, 1.0f + 6.0f * m_random.nextFloat(), 0.0f, 0.0f, -1, -1, 0.5f);
}

void LargeCaveFeature::addTunnel(int x, int z, TileID* tiles, float rx, float ry, float rz, float x1, float ang, float x2, int x3, int x4, float x5)
{
    float v65 = float(8 * (2 * x + 1));
    float v64 = float(8 * (2 * z + 1));
    float v63 = 0.0;
    float v62 = 0.0;
    int v13 = m_random.nextLong();
    Random v33(v13);

    if (x4 <= 0)
    {
        int v61 = (16 * (m_radius - 1));
        x4 = v61 - v33.nextInt(v61 / 4);
    }
    
    bool v69 = false;
    if (x3 == -1)
    {
        x3 = x4 / 2;
        v69 = true;
    }

    int v60 = x4 / 4 + v33.nextInt(x4 / 2);
    bool v68 = v33.nextInt(6) == 0;
    while (x3 < x4)
    {
        float v59 = sin((x3 * float(M_PI)) / (float)x4) * x1 + 1.5f;
        float v58 = v59 * x5;
        float cos_pitch = cos(x2);
        rx += (float)(cos(ang) * cos_pitch);
        ry += sin(x2);
        rz = rz + (float)(sin(ang) * cos_pitch);
        float a10a;
        if (v68)
            a10a = x2 * 0.92f;
        else
            a10a = x2 * 0.7f;

        x2 = a10a + (float)(v62 * 0.1f);
        ang = ang + (float)(v63 * 0.1f);
        v62 = v62 * 0.9f;
        v63 = v63 * 0.75f;
        float v18 = v33.nextFloat() - v33.nextFloat();
        float v19 = v33.nextFloat();
        v62 += 2 * v18 * v19;
        float v21 = v33.nextFloat() - v33.nextFloat();
        float v22 = v33.nextFloat();
        v63 += (v21 * v22) * 4.0f;
        if (v69 != 1 && x3 == v60 && x1 > 1.0f)
        {
            addTunnel(
                x,
                z,
                tiles,
                rx,
                ry,
                rz,
                (v33.nextFloat() * 0.5f) + 0.5f,
                (float(M_PI) / -2.0f) + ang,
                x2 / 3.0f,
                x3,
                x4,
                1.0f);
            addTunnel(
                x,
                z,
                tiles,
                rx,
                ry,
                rz,
                (v33.nextFloat() * 0.5f) + 0.5f,
                (float(M_PI) / 2.0f) + ang,
                x2 / 3.0f,
                x3,
                x4,
                1.0f);
            return;
        }
        if (v69 == 1 || v33.nextInt(4) != 0)
        {
            float v55 = rx - v65;
            float v54 = rz - v64;
            float v53 = (x4 - x3);
            float v52 = (x1 + 2.0f) + 16.0f;
            if (((((rx - v65) * (rx - v65))
                + ((rz - v64) * (rz - v64)))
                - (v53 * v53)) > (v52 * v52))
                return;
            if (((v65 - 16.0f) + (v59 * -2.0f)) <= rx &&
                ((v64 - 16.0f) + (v59 * -2.0f)) <= rz &&
                ((v65 + 16.0f) + (v59 * +2.0f)) >= rx &&
                ((v64 + 16.0f) + (v59 * +2.0f)) >= rz)
            {
                int v51 = -16 * x + (int)floor(rx - v59) - 1;
                int v50 = -16 * x + (int)floor(rx + v59) + 1;
                int v49 = (int)floor(ry - v58) - 1;
                int v48 = (int)floor(ry + v58) + 1;
                int v47 = -16 * z + (int)floor(rz - v59) - 1;
                int v46 = -16 * z + (int)floor(rz + v59) + 1;
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
                        float v40 = ((float(l + 16 * x) + 0.5f) - rx) / v59;
                        for (int m = v47; m < v46; ++m)
                        {
                            float v38 = ((float(m + 16 * z) + 0.5f) - rz) / v59;
                            int v37 = v48 + ((m + 16 * l) << 7);
                            bool v66 = 0;
                            if ((float)((float)(v40 * v40) + (float)(v38 * v38)) < 1.0f)
                            {
                                for (int n = v48 - 1; n >= v49; --n)
                                {
                                    float v35 = ((float(n) + 0.5) - ry) / v58;
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
                    if (v69)
                        return;
                }
            }
        }
        ++x3;
    }
}
