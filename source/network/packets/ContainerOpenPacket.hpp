#pragma once

#include <string>
#include "../Packet.hpp"
#include "world/inventory/Container.hpp"

class ContainerOpenPacket : public Packet
{
public:
	ContainerOpenPacket()
	{
		m_containerId = 0;
		m_type = Container::CONTAINER;
		m_size = 0;
	}
	ContainerOpenPacket(int8_t containerId, Container::Type type, const std::string& title, int8_t size);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	int8_t m_containerId;
	Container::Type m_type;
	RakNet::RakString m_title;
	int8_t m_size;
};
