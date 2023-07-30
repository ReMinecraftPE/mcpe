#pragma once

#include "NetEventCallback.hpp"
#include "Minecraft.hpp"
#include "RakNetInstance.hpp"
#include "LevelListener.hpp"
class Minecraft;

class ServerSideNetworkHandler : public NetEventCallback, public LevelListener
{
public:
	ServerSideNetworkHandler(Minecraft*, RakNetInstance*);
	~ServerSideNetworkHandler();

	// Overridden from NetEventCallback
	void levelGenerated(Level*) override;
	void onNewClient(const RakNet::RakNetGUID&) override;
	void onDisconnect(const RakNet::RakNetGUID&) override;
	void handle(const RakNet::RakNetGUID&, LoginPacket*) override;
	void handle(const RakNet::RakNetGUID&, MovePlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlaceBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, RemoveBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlayerEquipmentPacket*) override;
	void handle(const RakNet::RakNetGUID&, RequestChunkPacket*) override;

	// Overridden from LevelListener
	void tileBrightnessChanged(int x, int y, int z) override;
	void tileChanged(int x, int y, int z) override;

	void allowIncomingConnections(bool b);
	void displayGameMessage(const std::string&);
	void redistributePacket(Packet* packet, const RakNet::RakNetGUID& source);

public:
	Minecraft* m_pMinecraft = nullptr;
	Level* m_pLevel = nullptr;
	RakNetInstance* m_pRakNetInstance = nullptr;
	RakNet::RakPeerInterface* m_pRakNetPeer = nullptr;
	bool m_bAllowIncoming = false;	
};

