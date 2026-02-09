/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LocalPlayer.hpp"
#include "client/app/Minecraft.hpp"
#include "nbt/CompoundTag.hpp"
#include "network/packets/MovePlayerPacket.hpp"
#include "network/packets/PlayerEquipmentPacket.hpp"
#include "client/gui/screens/inventory/CraftingScreen.hpp"
#include "client/gui/screens/inventory/ChestScreen.hpp"

int dword_250ADC, dword_250AE0;

void LocalPlayer::_init()
{
	m_nAutoJumpFrames = 0;
	// multiplayer related
	m_lastSentPos = Vec3::ZERO;
	m_lastSentRot = Vec2::ZERO;
	// multiplayer related -- end

	m_renderArmRot = Vec2::ZERO;
	m_lastRenderArmRot = Vec2::ZERO;
	m_lastSelectedSlot = m_pInventory->getSelectedItemId();
}

LocalPlayer::LocalPlayer(Minecraft* pMinecraft, Level* pLevel, User* pUser, GameType playerGameType, int dimensionId) : Player(pLevel, playerGameType)
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
	m_lastSelectedSlot = 0;
	m_pMoveInput = nullptr;

	m_pMinecraft = pMinecraft;
	m_name = pUser->field_0;

	m_dimension = dimensionId;
	_init();
}

LocalPlayer::~LocalPlayer()
{
}

void LocalPlayer::die(Entity* pCulprit)
{
#if NETWORK_PROTOCOL_VERSION >= 4
	m_pInventory->dropAll();
#endif

	Player::die(pCulprit);
}

void LocalPlayer::aiStep()
{
	bool wasJumping = m_pMoveInput->m_bJumping;
	m_pMoveInput->tick(this);

	if (m_pMoveInput->m_bSneaking && m_ySlideOffset < 0.2f)
		m_ySlideOffset = 0.2f;

	m_lastRenderArmRot = m_renderArmRot;
	m_renderArmRot.x = Mth::Lerp(m_renderArmRot.x, m_rot.x, 0.5f);
	m_renderArmRot.y = Mth::Lerp(m_renderArmRot.y, m_rot.y, 0.5f);

	// timer for switching flight states
	if (m_abilities.bCanFly && m_pMoveInput->m_bJumping && !wasJumping)
	{
		if (m_jumpTriggerTime == 0)
			m_jumpTriggerTime = 7;
		else
		{
			m_bFlying = !m_bFlying;
			m_jumpTriggerTime = 0;
		}
	}

	// up/down movement while flying
	if (m_bFlying)
	{
		int yChange = 0;
		
		if (m_pMoveInput->m_bSneaking)
			--yChange;
		if (m_pMoveInput->m_bFlyUp)
			++yChange;

		if (yChange != 0)
			m_vel.y += yChange * 0.15;
	}

	Mob::aiStep();
	Player::aiStep();

	if ((m_bFlying && m_bOnGround) || !m_abilities.bCanFly)
		m_bFlying = false;

	if (interpolateOnly())
		updateAi();
}

void LocalPlayer::setPlayerGameType(GameType gameType)
{
	// @HACK: This info should be updated / stored in one place, and one place only.
	// Don't know what crack Notch was smoking.
	m_pMinecraft->setGameMode(gameType);

	Player::setPlayerGameType(gameType);
}

void LocalPlayer::swing()
{
	Player::swing();

	m_pMinecraft->m_pRakNetInstance->send(new AnimatePacket(m_EntityID, AnimatePacket::SWING));
}

void LocalPlayer::startCrafting(const TilePos& pos)
{
	// PE 0.3.2 doesn't let you craft in creative mode
	m_pMinecraft->setScreen(new CraftingScreen(m_pInventory, pos, m_pLevel));
}

/*void LocalPlayer::openFurnace(FurnaceTileEntity* furnace)
{
	// PE 0.3.2 doesn't let you cook in creative mode
	m_pMinecraft->setScreen(new FurnaceScreen(m_pInventory, furnace));
}*/

void LocalPlayer::openContainer(Container* container)
{
	// PE 0.3.2 doesn't let you open chests in creative mode
	m_pMinecraft->setScreen(new ChestScreen(m_pInventory, container));
}

void LocalPlayer::closeContainer()
{
	Player::closeContainer();
	m_pMinecraft->m_pGameMode->handleCloseInventory(m_pContainerMenu->m_containerId, this);
	m_pMinecraft->setScreen(nullptr);
}

/*void LocalPlayer::openTrap(DispenserTileEntity* tileEntity)
{
	m_pMinecraft->setScreen(new TrapScreen(m_pInventory, tileEntity));
}*/

/*void LocalPlayer::openTextEdit(SignTileEntity* tileEntity)
{
	m_pMinecraft->setScreen(new TextEditScreen(tileEntity));
}*/

void LocalPlayer::reset()
{
	Player::reset();
	_init();
}

void LocalPlayer::animateRespawn()
{

}

void LocalPlayer::hurtTo(int newHealth)
{
    // only called by client network handler
	int dmg = m_health - newHealth;
	if (dmg <= 0)
	{
		m_health = newHealth;
	}
	else
	{
		m_lastHurt = dmg;
		m_lastHealth = m_health;
        // makes EntityEventPacket the authority for client-side invulnerability
		//m_invulnerableTime = m_invulnerableDuration;
		actuallyHurt(dmg);
		m_hurtTime = m_hurtDuration = 10;
	}
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
	float f2 = m_pMinecraft->getOptions()->m_sensitivity.get() * 0.35f;
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

void LocalPlayer::respawn()
{
	m_pMinecraft->respawnPlayer();
}

bool LocalPlayer::isSneaking() const
{
	return m_pMoveInput->m_bSneaking && !m_bFlying;
}

void LocalPlayer::move(const Vec3& pos)
{
	LocalPlayer* pLP = m_pMinecraft->m_pLocalPlayer;
	if (Minecraft::DEADMAU5_CAMERA_CHEATS && pLP == this && m_pMinecraft->getOptions()->m_flightHax.get())
	{
		//@HUH: Using m_pMinecraft->m_pLocalPlayer instead of this, even though they're the same
		pLP->m_bNoPhysics = true;

		float m_walkDist_old = m_walkDist;

		pLP->calculateFlight(pos);
		pLP->m_distanceFallen = 0.0f;
		pLP->m_vel.y = 0.0f;

		// This looks very funny.
		pLP->Entity::move(field_BF0);

		pLP->m_bOnGround = true;

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
		float posZ = m_pos.z;

		Entity::move(pos);

		if (m_nAutoJumpFrames <= 0)
		{
			if (Mth::floor(posX * 2) == Mth::floor(m_pos.x * 2) &&
				Mth::floor(posY * 2) == Mth::floor(m_pos.y * 2) &&
				Mth::floor(posZ * 2) == Mth::floor(m_pos.z * 2))
				return;

			float dist = Mth::sqrt(pos.x * pos.x + pos.z * pos.z);
			int x1 = Mth::floor(pos.x / dist + m_pos.x);
			int z1 = Mth::floor(pos.z / dist + m_pos.z);

			TileID tileOnTop = m_pLevel->getTile(TilePos(x1, int(m_pos.y - 1.0f), z1));

			// not standing on top of a tile?
			if (!m_pLevel->isSolidTile(TilePos(x1, int(m_pos.y - 1.0f), z1)))
				return;

			// aren't inside of a tile right now
			if (m_pLevel->isSolidTile(TilePos(x1, int(m_pos.y), z1)))
				return;

			// don't have anything on top of us
			if (m_pLevel->isSolidTile(TilePos(x1, int(m_pos.y + 1.0f), z1)))
				return;

			// are we trying to walk into stairs or a slab?
			if (tileOnTop != Tile::stairs_stone->m_ID &&
				tileOnTop != Tile::stairs_wood->m_ID &&
				tileOnTop != Tile::stoneSlabHalf->m_ID &&
				m_pMinecraft->getOptions()->m_autoJump.get())
				// Nope, we're walking towards a full block. Trigger an auto jump.
				m_nAutoJumpFrames = 1;
		}
	}
}

void LocalPlayer::tick()
{
	Player::tick();

	if (m_pMinecraft->isOnline())
	{
		sendPosition();

		if (m_lastSelectedSlot != m_pInventory->m_selectedSlot)
		{
			m_lastSelectedSlot = m_pInventory->m_selectedSlot;
			const ItemStack& item = m_pInventory->getSelectedItem();
			m_pMinecraft->m_pRakNetInstance->send(new PlayerEquipmentPacket(m_EntityID, item.getId(), item.getAuxValue()));
		}
	}
}

void LocalPlayer::updateAi()
{
	Player::updateAi();

	if (m_pMinecraft->m_pScreen)
	{
		m_moveVelocity = Vec2::ZERO;
		m_bJumping = false;
	}
	else
	{
		m_moveVelocity.x = m_pMoveInput->m_horzInput;
		m_moveVelocity.y = m_pMoveInput->m_vertInput;

		m_bJumping = m_pMoveInput->m_bJumping || m_nAutoJumpFrames > 0;
	}
}

void LocalPlayer::addAdditionalSaveData(CompoundTag& tag) const
{
	Player::addAdditionalSaveData(tag);

	tag.putInt32("Score", getScore());
}

void LocalPlayer::readAdditionalSaveData(const CompoundTag& tag)
{
	Player::readAdditionalSaveData(tag);

	m_score = tag.getInt32("Score");
}

void LocalPlayer::sendPosition()
{
	if (fabsf(m_pos.x - m_lastSentPos.x) > 0.1f ||
		fabsf(m_pos.y - m_lastSentPos.y) > 0.01f ||
		fabsf(m_pos.z - m_lastSentPos.z) > 0.1f ||
		fabsf(m_lastSentRot.y - m_rot.y) > 1.0f ||
		fabsf(m_lastSentRot.x - m_rot.x) > 1.0f)
	{
		m_pMinecraft->m_pRakNetInstance->send(new MovePlayerPacket(m_EntityID, Vec3(m_pos.x, m_pos.y - m_heightOffset, m_pos.z), m_rot));
		m_lastSentPos = m_pos;
		m_lastSentRot = m_rot;
	}
}
