/********************************************************************
	ReMinecraftPE
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

enum eBuildActionIntent
{
	INTENT_ATTACK = 1,
	INTENT_INTERACT = 2,
	INTENT_REMOVE_CONTINUE = 2,
	INTENT_FIRST_REMOVE = 10,
};

class BuildActionIntention
{
public:
	BuildActionIntention() : m_type(0) {}
	BuildActionIntention(int type) : m_type(type) {}

	bool isAttack() const {
		return m_type == INTENT_ATTACK;
	}

	bool isFirstRemove() const {
		return m_type == INTENT_FIRST_REMOVE;
	}

	bool isInteract() const {
		return m_type == INTENT_INTERACT;
	}

	bool isRemove() const {
		return isFirstRemove() || isRemoveContinue();
	}

	bool isRemoveContinue() const {
		return m_type == INTENT_REMOVE_CONTINUE;
	}

private:
	int m_type;
};

