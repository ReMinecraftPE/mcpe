#include "PacketUtil.hpp"
#include "nbt/NbtIo.hpp"

char PacketUtil::Rot_degreesToChar(float degrees)
{
	return (degrees / 360.0f) * 256.0f;
}

float PacketUtil::Rot_charToDegrees(char charValue)
{
	return (charValue * 0.0039062f) * 360.0f;
}

void PacketUtil::Rot_entityToChar(const Entity* entity, char& yawChar, char& pitchChar)
{
	yawChar = Rot_degreesToChar(entity->m_rot.x);
	pitchChar = Rot_degreesToChar(entity->m_rot.y);
}

void PacketUtil::Rot_charToEntity(Entity* entity, char yawChar, char pitchChar)
{
	float pitch = PacketUtil::Rot_charToDegrees(pitchChar);
	entity->m_oRot.y = pitch;
	entity->m_rot.y = pitch;

	float yaw = PacketUtil::Rot_charToDegrees(yawChar);
	entity->m_oRot.x = yaw;
	entity->m_rot.x = yaw;
}

void PacketUtil::WriteUserData(const ItemStack& item, RakNet::BitStream* bs, bool minData)
{
    if (item.getItem())
    {
        // Item::writeUserData (0.12.1)
        if (!item.getUserData())
        {
            int16_t size = 0;
            bs->Write(size);
            return;
        }

        std::string rawUserData;
        StringByteOutput dos(&rawUserData);
        const CompoundTag* userData;
        if (minData)
            userData = item.getNetworkUserData();
        else
            userData = item.getUserData();

        if (userData)
        {
            Tag::writeNamedTag("tag", *userData, dos);

            if (minData)
                delete userData;
        }

        bs->Write<int16_t>(rawUserData.size());
        bs->Write(rawUserData);
    }
    else if (item.getTile())
    {
        // Tile::writeUserData (0.12.1)
        if (item.getBaseRepairCost() == 0)
        {
            bs->Write(false);
            return;
        }

        bs->Write<int32_t>(item.getBaseRepairCost());

        if (item.hasCustomHoverName())
        {
            bs->Write(true);
            RakNet::RakString hoverName(item.getHoverName().c_str());
            bs->Write(hoverName);
        }
    }
}

void PacketUtil::ReadUserData(ItemStack& item, RakNet::BitStream* bs)
{
    if (item.getItem())
    {
        // Item::readUserData (0.12.1)
        int16_t size;
        bs->Read(size);
        if (size == 0)
            return;

        // Don't ask me why we're writing this as an std::string, this is from 0.12.1
        std::string rawUserData;
        bs->Read(rawUserData);

        StringByteInput dis(rawUserData);
        CompoundTag* userData = NbtIo::read(dis);
        if (userData)
            item.setUserData(userData);
        return;
    }
    else if (item.getTile())
    {
        // Tile::readUserData (0.12.1) (never called or called virtually)
        bool hasData = false;
        if (!bs->Read(hasData))
            return;
        if (!hasData)
            return;

        int32_t repairCost;
        bs->Read(repairCost);

        bool hasHoverName = false;
        bs->Read(hasHoverName);

        item.setRepairCost(repairCost);
        if (hasHoverName)
        {
            RakNet::RakString hoverName;
            bs->Read(hoverName);
            item.setHoverName(hoverName.C_String());
        }
    }

    assert(!"Attempted PacketUtil::ReadUserData() for invalid ItemStack!");
}

void PacketUtil::WriteItemStack(const ItemStack& item, RakNet::BitStream* bs, bool doUserData, bool minUserData)
{
    int16_t itemId = item.getId();
    int8_t count = item.m_count;
    int16_t auxValue = item.getAuxValue();
    if (itemId <= 0)
    {
        itemId = -1;
        bs->Write(itemId);
        return;
    }

    bs->Write(itemId);
    bs->Write(count);
    bs->Write(auxValue);
    if (doUserData)
        WriteUserData(item, bs, minUserData);
}

ItemStack PacketUtil::ReadItemStack(RakNet::BitStream* bs, bool doUserData)
{
    int16_t itemId;
    if (!bs->Read(itemId))
        return ItemStack();

    if (itemId == -1)
        return ItemStack();

    uint8_t count;
    int16_t auxValue;
    if (!bs->Read(count) || !bs->Read(auxValue))
    {
        return ItemStack(); // Return empty if stream fails
    }

    ItemStack itemStack(itemId, count, auxValue);
    if (!itemStack.isValid())
    {
        return ItemStack();
    }

    if (!itemStack.getItem())
    {
        /*Item air(TILE_AIR);
        // Reading user data for fun
        air.readUserData(itemStack, bs);*/
        return ItemStack(false);
    }

    if (doUserData)
        ReadUserData(itemStack, bs);

    return itemStack;
}
