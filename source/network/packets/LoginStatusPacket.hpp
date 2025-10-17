#pragma once

#include "../Packet.hpp"

class LoginStatusPacket : public Packet
{
public:
	enum LoginStatus
	{
		STATUS_SUCCESS,
		STATUS_CLIENT_OUTDATED,
		STATUS_SERVER_OUTDATED
	};

public:
	LoginStatusPacket(LoginStatus loginStatus = STATUS_SUCCESS)
	{
		m_loginStatus = loginStatus;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	LoginStatus m_loginStatus;
};