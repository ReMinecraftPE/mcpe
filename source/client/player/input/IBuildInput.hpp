#pragma once

#include "BuildActionIntention.hpp"

class Player;

class IBuildInput
{
public:
	virtual ~IBuildInput();
	virtual void setScreenSize(int width, int height);
	virtual void tickBuild(Player*, BuildActionIntention*);
};

