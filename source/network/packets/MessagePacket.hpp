#pragma once

#include <string>
#include "../Packet.hpp"

class MessagePacket : public Packet
{
public:
	MessagePacket() {}
	MessagePacket(const std::string& msg)
	{
		m_str = msg.c_str();
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	RakNet::RakString m_str;
};