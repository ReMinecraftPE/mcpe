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
#include "compat/PlatformDefinitions.h"
#include "common/Util.hpp"

struct PingedCompatibleServer
{
	RakNet::RakString m_name;
	RakNet::SystemAddress m_address;
	int m_lastPinged;

	std::string getAddressStr() const
	{
#if MC_PLATFORM_XBOX
		return Util::format("%d.%d.%d.%d|%hu", m_address.address.addr4.sin_addr.S_un.S_un_b.s_b1
		                                     , m_address.address.addr4.sin_addr.S_un.S_un_b.s_b2
		                                     , m_address.address.addr4.sin_addr.S_un.S_un_b.s_b3
		                                     , m_address.address.addr4.sin_addr.S_un.S_un_b.s_b4
			                                 , m_address.GetPort()
		);
#else
		return std::string(m_address.ToString());
#endif
	}
};
