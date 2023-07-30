/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	AppPlatform.hpp

	The following code is licensed under the following license:
	< no license yet :( >
 ********************************************************************/

#pragma once

#include <string>
#include <vector>

#include "Texture.hpp"

class AppPlatform
{
public:
	enum eDialogType
	{
		DLG_CREATE_WORLD = 1,
		DLG_CHAT,
		DLG_OPTIONS,
		DLG_RENAME_MP_WORLD,
	};

public:
	virtual void buyGame();
	virtual int checkLicense();
	virtual void createUserInput();
	virtual void finish();
	virtual std::string getDateString(int);
	virtual int getScreenWidth() const;
	virtual int getScreenHeight() const;
	virtual std::vector<std::string> getUserInput();
	virtual int getUserInputStatus();
	virtual bool hasBuyButtonWhenInvalidLicense();
	virtual void saveScreenshot(const std::string&, int, int);
	virtual void showDialog(eDialogType);
	virtual void uploadPlatformDependentData(int, void*);
	virtual Texture loadTexture(const std::string&, bool);
	virtual std::vector<std::string>  getOptionStrings();

#ifndef ORIGINAL_CODE
	// Also add these to allow proper turning within the game.
	virtual void recenterMouse();
	virtual void setMouseGrabbed(bool b);
	virtual void getMouseDiff(int& x, int& y);
	virtual void clearDiff();
	// Also add this to allow proper text input within the game.
	virtual bool shiftPressed();
#endif


private:
	virtual void _tick();
};

