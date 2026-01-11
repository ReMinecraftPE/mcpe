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

	std::string getAssetPath(const std::string& path) const override;

	void setScreenSize(int width, int height);
	SoundSystem* getSoundSystem() const override { return m_pSoundSystem; }
	
	bool initGraphics(unsigned int width, unsigned int height);
	void createWindowSizeDependentResources(unsigned int width, unsigned int height);
	void swapBuffers();

private:
	int m_ScreenWidth;
	int m_ScreenHeight;

	bool m_bHasGraphics;
	bool m_bIsFocused;
	bool m_bWasUnfocused;

	SOUND_SYSTEM* m_pSoundSystem;
};

