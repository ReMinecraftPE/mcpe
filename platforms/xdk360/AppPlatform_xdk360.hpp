/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/app/AppPlatform.hpp"
#include "platforms/sound/dummy/CustomSoundSystem.hpp"

class AppPlatform_xdk360 : public AppPlatform
{
public:
	AppPlatform_xdk360();
	~AppPlatform_xdk360();

public:
	void initSoundSystem() override;

	int checkLicense() override;
	int getScreenWidth() const override { return m_ScreenWidth; }
	int getScreenHeight() const override { return m_ScreenHeight; }
	std::string getDateString(int time) override;
	bool doesTextureExist(const std::string& path) const override;

	// From v0.1.1. Also add these to determine touch screen use within the game.
	bool isTouchscreen() const override;
	void updateFocused(bool focused) override;
	bool hasFileSystemAccess() override;

	// Also add this to allow dynamic texture patching.
	AssetFile readAssetFile(const std::string&, bool) const override;

	void setScreenSize(int width, int height);
	SoundSystem* const getSoundSystem() const override { return m_pSoundSystem; }
	
	bool initGraphics();
	void createWindowSizeDependentResources(const Vec2& logicalSize, const Vec2& compositionScale);
	void swapBuffers();

private:
	int m_ScreenWidth;
	int m_ScreenHeight;

	bool m_bHasGraphics;
	bool m_bIsFocused;
	bool m_bWasUnfocused;

	SOUND_SYSTEM* m_pSoundSystem;
};

