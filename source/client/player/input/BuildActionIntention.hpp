/********************************************************************
	ReMinecraftPE
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

enum eBuildActionIntent
{
	INTENT_CLICKED = 1,        // touch screen was clicked
	INTENT_HELD = 2,		   // touch screen is being held down
	INTENT_FIRST_REMOVE = 10,  // after a small delay, starts breaking with this as a signal

	INTENT_MOUSE_LEFTCLICK   = 16,
	INTENT_MOUSE_RIGHTCLICK  = 17,
	INTENT_MOUSE_MIDDLECLICK = 18,
};

class BuildActionIntention
{
public:
	BuildActionIntention() : m_type(0) {}
	BuildActionIntention(int type) : m_type(type) {}

	bool isAttack() const {
		return m_type == INTENT_CLICKED || m_type == INTENT_MOUSE_LEFTCLICK;
	}

	bool isFirstRemove() const {
		return m_type == INTENT_FIRST_REMOVE || m_type == INTENT_MOUSE_LEFTCLICK;
	}

	bool isInteract() const {
		return m_type == INTENT_HELD || m_type == INTENT_MOUSE_RIGHTCLICK;
	}

	bool isBuild() {
		return m_type == INTENT_CLICKED || m_type == INTENT_MOUSE_RIGHTCLICK;
	}

	bool isRemove() const {
		return isFirstRemove() || isRemoveContinue();
	}

	bool isRemoveContinue() const {
		return m_type == INTENT_HELD || m_type == INTENT_MOUSE_LEFTCLICK;
	}

	bool isPick() const {
		return m_type == INTENT_MOUSE_MIDDLECLICK;
	}

private:
	int m_type;
};

