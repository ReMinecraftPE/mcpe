#include "common/Logger.hpp"

#include "client/app/App.hpp"
#include "client/app/NinecraftApp.hpp"

#include "AppPlatform_xdk360.hpp"

AppPlatform_xdk360 g_AppPlatform;
NinecraftApp* g_pApp;

void _setSize()
{
    XVIDEO_MODE VideoMode;
    XGetVideoMode(&VideoMode);
    Minecraft::width  = Mth::Max(VideoMode.dwDisplayWidth, 1280);
    Minecraft::height = Mth::Max(VideoMode.dwDisplayHeight, 720);
}

void __cdecl main()
{
	Logger::setSingleton(new Logger);

	_setSize();

	if (!g_AppPlatform.initGraphics(Minecraft::width, Minecraft::height))
		goto _cleanup;

	g_pApp = new NinecraftApp;
	g_pApp->m_pPlatform = &g_AppPlatform;
	g_AppPlatform.m_externalStorageDir = "savedrive:";

	// initialize the app
	g_pApp->init();
	g_pApp->sizeUpdate(Minecraft::width, Minecraft::height);

	while (!g_pApp->wantToQuit())
	{
		g_pApp->update();
		g_AppPlatform.swapBuffers();
	}

_cleanup:
	g_pApp->saveOptions();

	// Cleanup networking, renderer, sounds, textures, etc.
	delete g_pApp;
}
