//
//  AppPlatform_iOS.h
//  Minecraft
//
//  Created by Brent on 10/30/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
//

#pragma once

#import "minecraftpeViewController.h"

#include <string>

#include "client/app/AppPlatform.hpp"

#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "common/Logger.hpp"

class AppPlatform_iOS : public AppPlatform
{
public:
    AppPlatform_iOS();
    ~AppPlatform_iOS();
    
	void initSoundSystem() override;
    
	int checkLicense() override;
	int getScreenWidth() const override;
	int getScreenHeight() const override;
	Texture loadTexture(const std::string& path, bool b = false) override;
	int getUserInputStatus() override;
    bool isTouchscreen() override;
    bool hasFileSystemAccess() override;
    std::string getAssetPath(const std::string &path) const override;
    std::string getPatchData() override;
	SoundSystem* const getSoundSystem() const override { return m_pSoundSystem; }
    
	// Also add these to allow proper turning within the game.
	void setMouseGrabbed(bool b) override;
	void setMouseDiff(int x, int y);
	void getMouseDiff(int& x, int& y) override;
	void clearDiff() override;
    
	// Also add these to allow proper text input within the game.
	bool shiftPressed() override;
	void setShiftPressed(bool b, bool isLeft);
private:
	Logger* m_pLogger;
	SoundSystem* m_pSoundSystem;
    
    minecraftpeViewController* m_pViewController;
    
	bool m_bShiftPressed[2];
    
	int xrel;
	int yrel;
};

