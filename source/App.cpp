/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "App.hpp"

void App::destroy()
{

}

void App::draw()
{

}

bool App::handleBack(bool b)
{
	return false;
}

void App::init()
{

}

void App::loadState(void* a2, int a3)
{

}

AppPlatform* App::platform()
{
	return m_pPlatform;
}

void App::quit()
{
	m_bWantToQuit = true;
}

bool App::wantToQuit()
{
	return m_bWantToQuit;
}

void App::saveState(void**, int)
{

}

void App::update()
{

}
