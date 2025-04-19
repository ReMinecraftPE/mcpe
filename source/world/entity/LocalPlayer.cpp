/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LocalPlayer.hpp"
#include "client/app/Minecraft.hpp"

int dword_250ADC, dword_250AE0;

LocalPlayer::LocalPlayer(Minecraft* pMinecraft, Level* pLevel, User* pUser, GameType playerGameType, int i) : Player(pLevel, playerGameType)
{
	field_BEC = 0;
	field_BF0 = Vec3::ZERO;
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
	field_C24 = Vec3::ZERO;
	field_C30 = Vec2::ZERO;
	// multiplayer related -- end
	field_C38 = 0;
	m_pMoveInput = nullptr;

	m_renderArmRot = Vec2::ZERO;
	m_lastRenderArmRot = Vec2::ZERO;

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
	if (m_pMoveInput->m_bSneaking && m_ySlideOffset < 0.2f)
		m_ySlideOffset = 0.2f;

	m_lastRenderArmRot = m_renderArmRot;
	m_renderArmRot.x = Mth::Lerp(m_renderArmRot.x, m_rot.x, 0.5f);
	m_renderArmRot.y = Mth::Lerp(m_renderArmRot.y, m_rot.y, 0.5f);

	Mob::aiStep();
	Player::aiStep();
}

void LocalPlayer::drop(const ItemInstance* pItemInstance, bool b)
{
	if (pItemInstance)
	{
		if (m_pMinecraft->isOnlineClient())
		{
			// @TODO: Replicate DropItemPacket to server
		}
		else
		{
			Player::drop(pItemInstance, b);
		}
	}
}

bool LocalPlayer::isImmobile() const
{
	// The player should never move if they aren't focused on the game.
	// i.e. they're in a menu or the window is unfocused.
	
	// @BUG: This causes the player's hand to pause mid-swing while in menus, only when on gamepad.
	// There does not seem to be a way to get around this without implementing another layer to handle input, which Mojang did later on.

	// This more or less signifies that the game has focus. No GUI Screens, no other windows, just gameplay.
	return Player::isImmobile() || (m_pMinecraft->useController() && m_pMinecraft->m_pScreen); //!m_pMinecraft->m_bGrabbedMouse; // this works if we still set this when not using a mouse
}

void LocalPlayer::take(Entity* pEnt, int orgCount)
{
	//m_pMinecraft->m_pParticleEngine->add(new TakeAnimationParticle(m_pLevel, pEnt, this, -0.5f));
}

void LocalPlayer::animateRespawn()
{

}

void LocalPlayer::calculateFlight(const Vec3& pos)
{
	float f1 = m_pMinecraft->getOptions()->field_244;
	float x1 = f1 * pos.x;
	float z1 = f1 * pos.z;

	float y1 = 0.0f;
	if (Keyboard::isKeyDown(m_pMinecraft->getOptions()->getKey(KM_FLY_UP)))
		y1 = f1 * 0.2f;
	if (Keyboard::isKeyDown(m_pMinecraft->getOptions()->getKey(KM_FLY_DOWN)))
		y1 = f1 * -0.2f;

	field_BFC += x1;
	float f2 = m_pMinecraft->getOptions()->m_fSensitivity * 0.35f;
	float f3 = f2 * (field_BFC - field_C00);
	float f4 = field_C04 + 0.5f * (f3 - field_C04);
	field_C04 = f4;
	if ((f3 <= 0.0f || f3 <= f4) && (f3 >= 0.0f || f3 >= f4))
		f4 = f3;
	field_C00 += f4;
	field_BF0.x = f4 * 10.0f;

	field_C08 += y1;
	float f5 = f2 * (field_C08 - field_C0C);
	float f6 = field_C10 + 0.5f * (f5 - field_C10);
	field_C10 = f6;
	if ((f5 <= 0.0f || f5 <= f6) && (f5 >= 0.0f || f5 >= f6))
		f6 = f5;
	field_C0C += f6;
	field_BF0.y = f6 * 10.0f;

	field_C14 += z1;
	float f7 = f2 * (field_C14 - field_C18);
	float f8 = field_C1C + 0.5f * (f7 - field_C1C);
	field_C1C = f8;
	if ((f7 <= 0.0f || f7 <= f8) && (f7 >= 0.0f || f7 >= f8))
		f8 = f7;
	field_C18 += f8;
	field_BF0.z = f8 * 10.0f;
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

bool LocalPlayer::isSneaking() const
{
	return m_pMoveInput->m_bSneaking;
}

int LocalPlayer::move(const Vec3& pos)
{
	int result = 0;

	LocalPlayer* pLP = m_pMinecraft->m_pLocalPlayer;
	if (Minecraft::DEADMAU5_CAMERA_CHEATS && pLP == this && m_pMinecraft->getOptions()->m_bFlyCheat)
	{
		//@HUH: Using m_pMinecraft->m_pLocalPlayer instead of this, even though they're the same
		pLP->m_bNoPhysics = true;

		float m_walkDist_old = m_walkDist;

		pLP->calculateFlight(pos);
		pLP->m_distanceFallen = 0.0f;
		pLP->m_vel.y = 0.0f;

		// This looks very funny.
		result = pLP->Entity::move(field_BF0);

		pLP->m_onGround = true;

		m_walkDist = m_walkDist_old;
	}
	else
	{
#ifndef ORIGINAL_CODE
		// @BUG: In the original Minecraft, you can't stop flying! If you do, you'll just fall through to the bottom of the world. :(
		pLP->m_bNoPhysics = false;
#endif

		// autojump stuff
		if (m_nAutoJumpFrames > 0)
		{
			m_nAutoJumpFrames--;
			m_pMoveInput->m_bJumping = true;
		}

		float posX = m_pos.x;
		float posY = m_pos.y;

		result = Entity::move(pos);

		//@BUG: backing up posZ too late
		float posZ = m_pos.z;

		if (m_nAutoJumpFrames <= 0)
		{
			if (Mth::floor(posX * 2) == Mth::floor(m_pos.x * 2) &&
				Mth::floor(posY * 2) == Mth::floor(m_pos.y * 2) &&
				Mth::floor(posZ * 2) == Mth::floor(m_pos.z * 2))
				return result;

			float dist = Mth::sqrt(pos.x * pos.x + pos.z * pos.z);
			int x1 = Mth::floor(pos.x / dist + m_pos.x);
			int z1 = Mth::floor(pos.z / dist + m_pos.z);

			TileID tileOnTop = m_pLevel->getTile(TilePos(x1, int(m_pos.y - 1.0f), z1));

			// not standing on top of a tile?
			if (!m_pLevel->isSolidTile(TilePos(x1, int(m_pos.y - 1.0f), z1)))
				return 0;

			// aren't inside of a tile right now
			if (m_pLevel->isSolidTile(TilePos(x1, int(m_pos.y), z1)))
				return 0;

			// don't have anything on top of us
			if (m_pLevel->isSolidTile(TilePos(x1, int(m_pos.y + 1.0f), z1)))
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
		if (fabsf(m_pos.x - field_C24.x) > 0.1f ||
			fabsf(m_pos.y - field_C24.y) > 0.01f ||
			fabsf(m_pos.z - field_C24.z) > 0.1f ||
			fabsf(field_C30.y - m_rot.y) > 1.0f ||
			fabsf(field_C30.x - m_rot.x) > 1.0f)
		{
			m_pMinecraft->m_pRakNetInstance->send(new MovePlayerPacket(m_EntityID, Vec3(m_pos.x, m_pos.y - m_heightOffset, m_pos.z), m_rot));
			field_C24 = m_pos;
			field_C30 = m_rot;
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

	field_B00.x = m_pMoveInput->m_horzInput;
	field_B00.y = m_pMoveInput->m_vertInput;

	m_bJumping = m_pMoveInput->m_bJumping || m_nAutoJumpFrames > 0;
}
