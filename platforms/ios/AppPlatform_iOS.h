//
//  AppPlatform_iOS.h
//  Minecraft
//
//  Created by Brent on 10/30/23.
//  Copyright (c) 2023 ReMinecraftPE. All rights reserved.
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
private:
    NSString* _getAssetPath(const std::string &path) const;
    
public:
	AppPlatform_iOS(minecraftpeViewController *viewController);
	~AppPlatform_iOS();
	
	void initSoundSystem() override;
	
	int checkLicense() override;
	int getScreenWidth() const override;
	int getScreenHeight() const override;
	Texture loadTexture(const std::string& path, bool b = false) override;
    bool doesTextureExist(const std::string& path) const override;
	int getUserInputStatus() override;
	bool isTouchscreen() const override;
	std::string getAssetPath(const std::string& path) const override;
	std::string getPatchData() override;
	SoundSystem* const getSoundSystem() const override { return m_pSoundSystem; }
	
	// Also add these to allow proper text input within the game.
	bool shiftPressed() override;
	void setShiftPressed(bool b, bool isLeft);
	void showKeyboard() override;
	void hideKeyboard() override;
	int getKeyboardUpOffset() override;
	
	// Also add these to allow saving options.
	bool hasFileSystemAccess() override;
private:
	Logger* m_pLogger;
	SoundSystem* m_pSoundSystem;
	
	minecraftpeViewController* m_pViewController;
	
	bool m_bShiftPressed[2];
	
	bool m_bIsKeyboardShown;
};

