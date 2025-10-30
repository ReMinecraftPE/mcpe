#pragma once

#include "platforms/sdl/base/AppPlatform_sdl.hpp"

class AppPlatform_sdl2 : public AppPlatform_sdl
{
private:
	void _init(SDL_Window* window);

public:
	AppPlatform_sdl2(std::string storageDir, SDL_Window* window)
		: AppPlatform_sdl(storageDir)
	{
		_init(window);
	}
	~AppPlatform_sdl2() override;

private:
	SDL_GameController* _findGameController();

protected:
	void _updateWindowIcon() override;
	void _setMouseGrabbed(bool b) override;
	void _handleKeyEvent(int key, uint8_t state) override;
	void* _getHWND() const override;

public:
	const char* getWindowTitle() const override;

	int getScreenWidth() const override;
	int getScreenHeight() const override;

	// On-screen keyboard
	void showKeyboard(int x, int y, int w, int h) override;
	void hideKeyboard() override;

	// Game controller
	bool hasGamepad() const override;
	SDL_GameController* getPrimaryGameController() const { return m_pController; }
	void setPrimaryGameController(SDL_GameController* controller) { m_pController = controller; }
	void gameControllerAdded(int32_t index);
	void gameControllerRemoved(int32_t index);

public:
	static bool GetMouseButtonState(const SDL_Event& event);

private:
	SDL_GameController* m_pController;

protected:
	SDL_Window* m_pWindow;
};
