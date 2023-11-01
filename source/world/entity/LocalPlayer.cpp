/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LocalPlayer.hpp"
#include "client/app/Minecraft.hpp"

int dword_250ADC, dword_250AE0;

LocalPlayer::LocalPlayer(Minecraft* pMinecraft, Level* pLevel, User* pUser, int i) : Player(pLevel)
{
	field_BEC = 0;
	field_BF0 = 0.0f;
	field_BF8 = 0.0f;
	field_BFC = 0.0f;
	field_C00 = 0.0f;
	field_C04 = 0.0f;
	field_C08 = 0.0f;
	field_C0C = 0.0f;
	field_C10 = 0.0f;
	field_C14 = 0.0f;
	field_C18 = 0.0f;
	field_C1C = 0.0f;
	m_nAutoJumpFrames = 0;
	// multiplayer related
	field_C24 = 0.0f;
	field_C28 = 0.0f;
	field_C2C = 0.0f;
	field_C30 = 0.0f;
	field_C34 = 0.0f;
	// multiplayer related -- end
	field_C38 = 0;
	m_pMoveInput = nullptr;

	m_pMinecraft = pMinecraft;
	m_name = pUser->field_0;

	field_BC4 = i;
	field_C38 = m_pInventory->getSelectedItemId();
}

LocalPlayer::~LocalPlayer()
{
}

void LocalPlayer::aiStep()
{
	m_pMoveInput->tick(this);
	if (m_pMoveInput->m_bSneakButton && field_A4 < 0.2f)
		field_A4 = 0.2f;

	Mob::aiStep();
	Player::aiStep();
}

void LocalPlayer::animateRespawn()
{

}

void LocalPlayer::calculateFlight(float x, float y, float z)
{
	float f1 = m_pMinecraft->getOptions()->field_244;
	float x1 = f1 * x;
	float z1 = f1 * z;

	float y1 = 0.0f;
	if (Keyboard::isKeyDown(m_pMinecraft->getOptions()->getKey(KM_FLY_UP)))
		y1 = f1 * 0.2f;
	if (Keyboard::isKeyDown(m_pMinecraft->getOptions()->getKey(KM_FLY_DOWN)))
		y1 = f1 * -0.2f;

	field_BFC += x1;
	float f2 = m_pMinecraft->getOptions()->field_8 * 0.35f;
	float f3 = f2 * (field_BFC - field_C00);
	float f4 = field_C04 + 0.5f * (f3 - field_C04);
	field_C04 = f4;
	if ((f3 <= 0.0f || f3 <= f4) && (f3 >= 0.0f || f3 >= f4))
		f4 = f3;
	field_C00 += f4;
	field_BF0 = f4 * 10.0f;

	field_C08 += y1;
	float f5 = f2 * (field_C08 - field_C0C);
	float f6 = field_C10 + 0.5f * (f5 - field_C10);
	field_C10 = f6;
	if ((f5 <= 0.0f || f5 <= f6) && (f5 >= 0.0f || f5 >= f6))
		f6 = f5;
	field_C0C += f6;
	field_BF4 = f6 * 10.0f;

	field_C14 += z1;
	float f7 = f2 * (field_C14 - field_C18);
	float f8 = field_C1C + 0.5f * (f7 - field_C1C);
	field_C1C = f8;
	if ((f7 <= 0.0f || f7 <= f8) && (f7 >= 0.0f || f7 >= f8))
		f8 = f7;
	field_C18 += f8;
	field_BF8 = f8 * 10.0f;
}

void LocalPlayer::closeContainer()
{
	Player::closeContainer();
	m_pMinecraft->setScreen(nullptr);
}

void LocalPlayer::respawn()
{
	m_pMinecraft->respawnPlayer(this);
}

bool LocalPlayer::isSneaking()
{
	return m_pMoveInput->m_bSneakButton;
}

int LocalPlayer::move(float x, float y, float z)
{
	int result = 0;

	LocalPlayer* pLP = m_pMinecraft->m_pLocalPlayer;
	if (Minecraft::DEADMAU5_CAMERA_CHEATS && pLP == this && m_pMinecraft->getOptions()->m_bFlyCheat)
	{
		//@HUH: Using m_pMinecraft->m_pLocalPlayer instead of this, even though they're the same
		pLP->m_bNoCollision = true;

		float field_94_old = field_94;

		pLP->calculateFlight(x, y, z);
		pLP->m_distanceFallen = 0.0f;
		pLP->m_vel.y = 0.0f;

		// This looks very funny.
		result = pLP->Entity::move(field_BF0, field_BF4, field_BF8);

		pLP->field_7C = true;

		field_94 = field_94_old;
	}
	else
	{
#ifndef ORIGINAL_CODE
		// @BUG: In the original Minecraft, you can't stop flying! If you do, you'll just fall through to the bottom of the world. :(
		pLP->m_bNoCollision = false;
#endif

		// autojump stuff
		if (m_nAutoJumpFrames > 0)
		{
			m_nAutoJumpFrames--;
			m_pMoveInput->m_bJumpButton = true;
		}

		float posX = m_pos.x;
		float posY = m_pos.y;

		result = Entity::move(x, y, z);

		//@BUG: backing up posZ too late
		float posZ = m_pos.z;

		if (m_nAutoJumpFrames <= 0)
		{
			if (Mth::floor(posX * 2) == Mth::floor(m_pos.x * 2) &&
				Mth::floor(posY * 2) == Mth::floor(m_pos.y * 2) &&
				Mth::floor(posZ * 2) == Mth::floor(m_pos.z * 2))
				return result;

			float dist = Mth::sqrt(x * x + z * z);
			int x1 = Mth::floor(x / dist + m_pos.x);
			int z1 = Mth::floor(z / dist + m_pos.z);

			TileID tileOnTop = m_pLevel->getTile(x1, int(m_pos.y - 1.0f), z1);

			// not standing on top of a tile?
			if (!m_pLevel->isSolidTile(x1, int(m_pos.y - 1.0f), z1))
				return 0;

			// aren't inside of a tile right now
			if (m_pLevel->isSolidTile(x1, int(m_pos.y), z1))
				return 0;

			// don't have anything on top of us
			if (m_pLevel->isSolidTile(x1, int(m_pos.y + 1.0f), z1))
				return 1;

			// are we trying to walk into stairs or a slab?
			if (tileOnTop != Tile::stairs_stone->m_ID && tileOnTop != Tile::stairs_wood->m_ID && tileOnTop != Tile::stoneSlabHalf->m_ID && m_pMinecraft->getOptions()->m_bAutoJump)
				// Nope, we're walking towards a full block. Trigger an auto jump.
				m_nAutoJumpFrames = 1;
		}
	}

	return result;
}

void LocalPlayer::tick()
{
	Player::tick();

	if (m_pMinecraft->isOnline())
	{
		if (fabsf(m_pos.x - field_C24) > 0.1f ||
			fabsf(m_pos.y - field_C28) > 0.01f ||
			fabsf(m_pos.z - field_C2C) > 0.1f ||
			fabsf(field_C30 - m_pitch) > 1.0f ||
			fabsf(field_C34 - m_yaw) > 1.0f)
		{
			m_pMinecraft->m_pRakNetInstance->send(new MovePlayerPacket(m_EntityID, m_pos.x, m_pos.y - field_84, m_pos.z, m_pitch, m_yaw));
			field_C24 = m_pos.x;
			field_C28 = m_pos.y;
			field_C2C = m_pos.z;
			field_C30 = m_pitch;
			field_C34 = m_yaw;
		}

		if (field_C38 != m_pInventory->getSelectedItemId())
		{
			field_C38 = m_pInventory->getSelectedItemId();
			m_pMinecraft->m_pRakNetInstance->send(new PlayerEquipmentPacket(m_EntityID, field_C38));
		}
	}
}

void LocalPlayer::updateAi()
{
	Player::updateAi();

	field_B00 = m_pMoveInput->m_horzInput;
	field_B04 = m_pMoveInput->m_vertInput;

	field_B0C = m_pMoveInput->m_bJumpButton || m_nAutoJumpFrames > 0;
}

bool LocalPlayer::isLocalPlayer()
{
	return true;
}
