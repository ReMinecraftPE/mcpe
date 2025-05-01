/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ITouchScreenModel.hpp"
#include "IArea.hpp"

class TouchAreaModel : public ITouchScreenModel
{
public:
	struct Area
	{
		IArea* m_pArea;
		int m_id;

		Area() : m_pArea(0), m_id(0) {}
	};

public:
	~TouchAreaModel();

	int getPointerId(const MouseAction&) const override;
	int getPointerId(int x, int y, int pointerId) const override;

	void clear();
	void addArea(int id, IArea* m_pArea);

private:
	std::vector<Area*> m_areas;
};

