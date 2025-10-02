#pragma once

#include <string>
#include "../Packet.hpp"

class LoginPacket : public Packet
{
public:
	LoginPacket()
	{
		m_clientNetworkVersion = 0;
		m_clientNetworkVersion2 = 0;
	}
	LoginPacket(const std::string& uname)
	{
		m_str = RakNet::RakString(uname.c_str());
		m_clientNetworkVersion = NETWORK_PROTOCOL_VERSION;
		m_clientNetworkVersion2 = NETWORK_PROTOCOL_VERSION;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	RakNet::RakString m_str;
	int m_clientNetworkVersion;
	int m_clientNetworkVersion2;
};