#pragma once

#include "BuildActionIntention.hpp"

class Player;

class IBuildInput
{
public:
	virtual ~IBuildInput();
	virtual void setScreenSize(int width, int height);
	virtual bool tickBuild(Player*, BuildActionIntention*);
};

