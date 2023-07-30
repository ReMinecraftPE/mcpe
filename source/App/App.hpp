/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	App.hpp

	The following code is licensed under the following license:
	< no license yet :( >
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

