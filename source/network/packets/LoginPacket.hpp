#pragma once

#include <string>
#include "../Packet.hpp"

class LoginPacket : public Packet
{
public:
	LoginPacket()
	{
		m_clientNetworkVersion = 0;
		m_clientNetworkVersionMin = 0;
	}
	LoginPacket(const std::string& uname, int protocolVersion)
	{
		m_userName = RakNet::RakString(uname.c_str());
		m_clientNetworkVersion = protocolVersion;
		m_clientNetworkVersionMin = protocolVersion;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	RakNet::RakString m_userName;
	int m_clientNetworkVersion;
	int m_clientNetworkVersionMin;
};