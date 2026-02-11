#pragma once

#include <xtl.h>

#include "client/app/AppPlatform.hpp"
#include "platforms/input/xinput/GameControllerHandler_xinput.hpp"
#include "CustomSoundSystem.hpp"

class AppPlatform_xdk360 : public AppPlatform
{
public:
	AppPlatform_xdk360();
	~AppPlatform_xdk360();

protected:
	void _getXContentData(XCONTENT_DATA& out, LocalPlayerID playerId);
	const XCONTENTDEVICEID& _getSaveDeviceId(LocalPlayerID playerId);

public:
	void initSoundSystem() override;

	int checkLicense() override;
	int getScreenWidth() const override { return m_ScreenWidth; }
	int getScreenHeight() const override { return m_ScreenHeight; }
	std::string getDateString(int time) override;

	// From v0.1.1. Also add these to determine touch screen use within the game.
	bool isTouchscreen() const override;
	bool hasGamepad() const override;
	GameControllerHandler* getGameControllerHandler() override;

	void updateFocused(bool focused) override;

	void showKeyboard(LocalPlayerID playerId, const VirtualKeyboard& keyboard) override;
	void hideKeyboard(LocalPlayerID playerId) override;
	void onHideKeyboard() override;
	const std::string& getKeyboardText() const override;

	bool hasFileSystemAccess() override;

	std::string getAssetPath(const std::string& path) const override;
	void makeNativePath(std::string& path) const override;

	void beginProfileDataRead(LocalPlayerID playerId) override;
	void endProfileDataRead(LocalPlayerID playerId) override;
	void beginProfileDataWrite(LocalPlayerID playerId) override;
	void endProfileDataWrite(LocalPlayerID playerId) override;

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
	GameControllerHandler_xinput m_gameControllerHandler;

	XCONTENTDEVICEID m_saveDeviceId[C_MAX_LOCAL_PLAYERS];
	int m_currentSavingPlayerId;

public: // accessed from main.cpp, that way we can pass it to App
	bool m_bVirtualKeyboard;
	XOVERLAPPED m_vkOverlapped;
	std::wstring m_vkDefaultText;
	std::wstring m_vkTitleText;
	std::wstring m_vkDescriptionText;
	std::wstring m_vkResultText;
	std::string m_keyboardText;
};

