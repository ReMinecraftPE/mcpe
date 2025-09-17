/********************************************************************
	ReMinecraftPE
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/LegacyCPP.hpp"

class BuildActionIntention
{
public:
	enum BuildActionIntent
	{
		NONE              = 0<<0,    		 //  0 0 0 0 0 0 0  //  

		PLACE             = 1<<0,			 //  0 0 0 0 0 0 1  //  indicates the desire to place a block
		DESTROY_START     = 1<<1,			 //  0 0 0 0 0 1 0  //  causes the player to start destroying the block in front of them, used to cause Minecraft to not automatically handle the BuildActionIntention
		DESTROY_CONTINUE  = 1<<2,			 //  0 0 0 0 1 0 0  //  causes the player to continue destroying the block in front of them
		ATTACK            = 1<<3,			 //  0 0 0 1 0 0 0  //  causes the player to take a swing in the air
		INTERACT          = 1<<4,			 //  0 0 1 0 0 0 0  //  causes the player to use the item in their hand
		// Custom bits
		PICK              = 1<<5,            //  1 0 0 0 0 0 0  //  causes the player to equip the block they're currently looking at

		TOUCH_TAP               = ATTACK   | PLACE,             // touch screen was tapped and released quickly
		KEY_DESTROY             = ATTACK   | DESTROY_START,     // the destory key was pressed
		KEY_USE                 = INTERACT | PLACE,             // the use key was pressed
		TOUCH_HOLD_START        = INTERACT | DESTROY_START,     // touch screen started being held down
		TOUCH_HOLD_CONTINUE     = INTERACT | DESTROY_CONTINUE   // touch screen is being held down
	};

public:
	BuildActionIntention() : m_type(NONE) {}
	BuildActionIntention(BuildActionIntent type) : m_type(type) {}

	bool isAttack() const {
		return (m_type & ATTACK) != NONE;
	}

	bool isInteract() const {
		return (m_type & INTERACT) != NONE;
	}

	bool isDestroy() const {
		return (m_type & (DESTROY_START + DESTROY_CONTINUE)) != NONE;
	}

	bool isDestroyStart() const {
		return (m_type & DESTROY_START) != NONE;
	}

	bool isDestroyContinue() const {
		return (m_type & DESTROY_CONTINUE) != NONE;
	}

	bool isPlace() const {
		return (m_type & PLACE) != NONE;
	}

	bool isPick() const {
		return (m_type & PICK) != NONE;
	}

private:
	BuildActionIntent m_type;
};

