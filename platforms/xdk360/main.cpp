#include "common/Logger.hpp"

#include "client/app/App.hpp"
#include "client/app/NinecraftApp.hpp"

#include "AppPlatform_xdk360.hpp"

AppPlatform_xdk360 g_AppPlatform;
NinecraftApp* g_pApp;

void __cdecl main()
{
	Logger::setSingleton(new Logger);

	g_pApp = new NinecraftApp;
	g_pApp->m_pPlatform = &g_AppPlatform;
	g_pApp->m_externalStorageDir = ".";

	// initialize the app
	g_pApp->init();
	g_pApp->sizeUpdate(Minecraft::width, Minecraft::height);
	g_AppPlatform.setScreenSize(Minecraft::width, Minecraft::height);

	while (!g_pApp->wantToQuit())
	{
		g_pApp->update();
		g_AppPlatform.swapBuffers();
	}

	g_pApp->saveOptions();

	// Cleanup networking, renderer, sounds, textures, etc.
	delete g_pApp;
}
