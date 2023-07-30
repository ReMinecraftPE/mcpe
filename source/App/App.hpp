/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "AppPlatform.hpp"

class App
{

public:
	void destroy();
	void draw();
	virtual bool handleBack(bool);
	virtual void init();
	void loadState(void*, int);
	AppPlatform* platform();
	void quit();
	void saveState(void**, int);
	virtual void update();
	bool wantToQuit();

public:
	bool m_bWantToQuit = false;

	// don't know what these are ...
	int field_8;
	int field_C;
	int field_10;

	AppPlatform* m_pPlatform = nullptr;
};

