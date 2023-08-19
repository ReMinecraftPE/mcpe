/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "RakNetTypes.h"
#include "RakString.h"


struct PingedCompatibleServer
{
	RakNet::RakString m_name;
	RakNet::SystemAddress m_address;
	int m_lastPinged;
};
