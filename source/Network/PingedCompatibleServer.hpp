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
