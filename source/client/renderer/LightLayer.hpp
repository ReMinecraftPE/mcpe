/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include "world/level/Brightness.hpp"

struct LightLayer
{
public:
	static LightLayer Sky;
	static LightLayer Block;

private:
	Brightness_t m_surrounding;

public:
	LightLayer(Brightness_t surrounding)
		: m_surrounding(surrounding)
	{
	}

public:
	bool operator==(const LightLayer& other) const
	{
		return this == &other;
	}

	bool operator!=(const LightLayer& other) const
	{
		return this != &other;
	}

public:
	Brightness_t getSurrounding() const
	{
		return m_surrounding;
	}
};

