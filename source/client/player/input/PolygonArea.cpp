#include "PolygonArea.hpp"
#include "common/Utils.hpp"

PolygonArea::PolygonArea(int count, const float* x, const float* y)
{
	m_count = count;
	m_xPos = new float[count];
	m_yPos = new float[count];

	for (int i = 0; i < count; i++)
	{
		m_xPos[i] = x[i];
		m_yPos[i] = y[i];
	}
}

PolygonArea::~PolygonArea()
{
	SAFE_DELETE_ARRAY(m_xPos);
	SAFE_DELETE_ARRAY(m_yPos);
}

bool PolygonArea::isInside(float x, float y)
{
	bool result = false;
	
	int idx = 0, idx2 = m_count - 1;
	while (idx < m_count)
	{
		// horrible ass condition
		if (((m_yPos[idx] <= y && m_yPos[idx2] > y) || (m_yPos[idx2] <= y && m_yPos[idx] > y))
			&& ((((m_xPos[idx2] - m_xPos[idx]) * (y - m_yPos[idx]))
				/ (m_yPos[idx2] - m_yPos[idx]))
				+ m_xPos[idx]) > x)
		{
			result = !result;
		}

		idx2 = idx++;
	}

	return result;
}
