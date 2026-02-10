/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StartMenuScreen.hpp"

#include "client/renderer/ScreenRenderer.hpp"
#include "client/renderer/LogoRenderer.hpp"

#include "InvalidLicenseScreen.hpp"
#include "OptionsScreen.hpp"
#include "ProgressScreen.hpp"
#include "SelectWorldScreen.hpp"
#include "JoinGameScreen.hpp"
#include "CreditsScreen.hpp"
#include "ConsoleSettingsScreen.hpp"

// actual name
const char* gSplashes[] =
{
	"One star! Deal with it notch!",
	"100% more yellow text!",
	"Glowing creepy eyes!",
	"Toilet friendly!",
	"Annoying touch buttons!",
	"Astronomically accurate!",
	"0xffff-1 chunks!",
	"Cubism!",
	"Pocket!",
	"Mostly harmless!",
	"!!!1!",
	"Dramatic lighting!",
	"As seen on TV!",
	"Awesome!",
	"100% pure!",
	"May contain nuts!",
	"Better than Prey!",
	"Less polygons!",
	"Sexy!",
	"Limited edition!",
	"Flashing letters!",
	"Made by Mojang!",
	"It's here!",
	"Best in class!",
	"It's alpha!",
	"100% dragon free!",
	"Excitement!",
	"More than 500 sold!",
	"One of a kind!",
	"Heaps of hits on YouTube!",
	"Indev!",
	"Spiders everywhere!",
	"Check it out!",
	"Holy cow, man!",
	"It's a game!",
	"Made in Sweden!",
	"Uses C++!",
	"Reticulating splines!",
	"Minecraft!",
	"Yaaay!",
	"Multiplayer!",
	"Touch compatible!",
	"Undocumented!",
	"Ingots!",
	"Exploding creepers!",
	"That's no moon!",
	"l33t!",
	"Create!",
	"Survive!",
	"Dungeon!",
	"Exclusive!",
	"The bee's knees!",
	"Down with O.P.P.!",
	"Open source!", //"Closed source!",
	"Classy!",
	"Wow!",
	"Not on steam!",
	"Oh man!",
	"Awesome community!",
	"Pixels!",
	"Teetsuuuuoooo!",
	"Kaaneeeedaaaa!",
	"Enhanced!",
	"90% bug free!",
	"Pretty!",
	"12 herbs and spices!",
	"Fat free!",
	"Absolutely no memes!",
	"Free dental!",
	"Ask your doctor!",
	"Minors welcome!",
	"Cloud computing!",
	"Legal in Finland!",
	"Hard to label!",
	"Technically good!",
	"Bringing home the bacon!",
	"Quite Indie!",
	"GOTY!",
	"Euclidian!",
	"Now in 3D!",
	"Inspirational!",
	"Herregud!",
	"Complex cellular automata!",
	"Yes, sir!",
	"Played by cowboys!",
#if MCE_GFX_API_OGL
#ifdef USE_GLES
	"OpenGL ES 1.1!",
#else
	"OpenGL 1.5!",
#endif
#elif MCE_GFX_API_D3D11
	"Direct3D 11.1!",
#elif MCE_GFX_API_D3D9
	"Direct3D 9!",
#endif
	"Thousands of colors!",
	"Try it!",
	"Age of Wonders is better!",
	"Try the mushroom stew!",
	"Sensational!",
	"Hot tamale, hot hot tamale!",
	"Play him off, keyboard cat!",
	"Guaranteed!",
	"Macroscopic!",
	"Bring it on!",
	"Random splash!",
	"Call your mother!",
	"Monster infighting!",
	"Loved by millions!",
	"Ultimate edition!",
	"Freaky!",
	"You've got a brand new key!",
	"Water proof!",
	"Uninflammable!",
	"Whoa, dude!",
	"All inclusive!",
	"Tell your friends!",
	"NP is not in P!",
	"Notch <3 ez!",
	"Livestreamed!",
	"Haunted!",
	"Polynomial!",
	"Terrestrial!",
	"All is full of love!",
	"Full of stars!",
	"Scientific!",
	"Cooler than Spock!",
	"Collaborate and listen!",
	"Never dig down!",
	"Take frequent breaks!",
	"Not linear!",
	"Han shot first!",
	"Nice to meet you!",
	"Buckets of lava!",
	"Ride the pig!",
	"Larger than Earth!",
	"sqrt(-1) love you!",
	"Phobos anomaly!",
	"Punching wood!",
	"Falling off cliffs!",
	"0% sugar!",
	"150% hyperbole!",
	"Synecdoche!",
	"Let's danec!",
	"Seecret Friday update!",
	"Ported implementation!",
	"Lewd with two dudes with food!",
	"Kiss the sky!",
	"20 GOTO 10!",
	"Verlet intregration!", // [sic]
	"Peter Griffin!",
	"Redistributable!", // Do not distribute!
	"Cogito ergo sum!",
	"4815162342 lines of code!",
	"A skeleton popped out!",
	"The Work of Notch!",
	"The sum of its parts!",
	"BTAF used to be good!",
	"I miss ADOM!",
	"umop-apisdn!",
	"OICU812!",
	"Bring me Ray Cokes!",
	"Finger-licking!",
	"Thematic!",
	"Pneumatic!",
	"Sublime!",
	"Octagonal!",
	"Une baguette!",
	"Gargamel plays it!",
	"Rita is the new top dog!",
	"SWM forever!",
	"Representing Edsbyn!",
	"Matt Damon!",
	"Supercalifragilisticexpialidocious!",
	"Consummate V's!",
	"Cow Tools!",
	"Double buffered!",
	"V-synched!",
	"Fan fiction!",
	"Flaxkikare!",
	"Jason! Jason! Jason!",
	"Hotter than the sun!",
	"Internet enabled!",
	"Autonomous!",
	"Engage!",
	"Fantasy!",
	"DRR! DRR! DRR!",
	"Kick it root down!",
	"Regional resources!",
	"Woo, facepunch!",
	"Woo, somethingawful!",
	"Woo, /v/!",
	"Woo, tigsource!",
	"Woo, minecraftforum!",
	"Woo, worldofminecraft!",
	"Woo, reddit!",
	"Woo, 2pp!",
	"Not Google anlyticsed!",
	"Give us Gordon!",
	"Tip your waiter!",
	"Very fun!",
	"12345 is a bad password!",
	"Vote for net neutrality!",
	"Lives in a pineapple under the sea!",
	"Omnipotent!",
	"Gasp!",
	"...!",
	"Bees, bees, bees, bees!",
	"Haha, LOL!",
	"Hampsterdance!",
	"Switches and ores!",
	"Menger sponge!",
	"idspispopd!",
	"Eple (original edit)!",
	"So fresh, so clean!",
	"Don't look directly at the bugs!",
	"Oh, ok, Pigmen!",
	"Scary!",
	"Play Minecraft, Watch Topgear, Get Pig!",
	"Twittered about!",
	"Jump up, jump up, and get down!",
	"Joel is neat!",
	"A riddle, wrapped in a mystery!",
	"Huge tracts of land!",
	"Welcome to your Doom!",
	"Stay a while, stay forever!",
	"Stay a while and listen!",
	"Treatment for your rash!",
	"\"Autological\" is!",
	"Information wants to be free!",
	"\"Almost never\" is an interesting concept!",
	"Lots of truthiness!",
	"The creeper is a spy!",
	"It's groundbreaking!",
	"Let our battle's begin!",
	"The sky is the limit!",
	"Jeb has amazing hair!",
	"Casual gaming!",
	"Undefeated!",
	"Kinda like Lemmings!",
	"Follow the train, CJ!",
	"Leveraging synergy!",
	"DungeonQuest is unfair!",
	"110813!",
	"90210!",
	"Tyrion would love it!",
	"Also try VVVVVV!",
	"Also try Super Meat Boy!",
	"Also try Terraria!",
	"Also try Mount And Blade!",
	"Also try Project Zomboid!",
	"Also try World of Goo!",
	"Also try Limbo!",
	"Also try Pixeljunk Shooter!",
	"Also try Braid!",
	"That's super!",
	"Bread is pain!",
	"Read more books!",
	"Khaaaaaaaaan!",
	"Less addictive than TV Tropes!",
	"More addictive than lemonade!",
	"Bigger than a bread box!",
	"Millions of peaches!",
	"Fnord!",
	"This is my true form!",
	"Totally forgot about Dre!",
	"Don't bother with the clones!",
	"Pumpkinhead!",
	"Hobo humping slobo babe!",
	"Endless!",
	"Feature packed!",
	"Boots with the fur!",
	"Stop, hammertime!",
	"Conventional!",
	"Homeomorphic to a 3-sphere!",
	"Doesn't avoid double negatives!",
	"Place ALL the blocks!",
	"Does barrel rolls!",
	"Meeting expectations!",
	"PC gaming since 1873!",
	"Ghoughpteighbteau tchoghs!",
	"Got your nose!",
	"Haley loves Elan!",
	"Afraid of the big, black bat!",
	"Doesn't use the U-word!",
	"Child's play!",
	"See you next Friday or so!",
	"150 bpm for 400000 minutes!",
	"Technologic!",
	"Funk soul brother!",
	"Pumpa kungen!",
	"Helo Cymru!",
	"My life for Aiur!",
	"Lennart lennart = new Lennart();",
	"I see your vocabulary has improved!",
	"Who put it there?",
	"You can't explain that!",
	"if not ok then return end",
	"SOPA means LOSER in Swedish!",
	"Big Pointy Teeth!",
	"Bekarton guards the gate!",
	"Mmmph, mmph!",
	"Don't feed avocados to parrots!",
	"Swords for everyone!",
	"Plz reply to my tweet!",
	".party()!",
	"Take her pillow!",
	"Put that cookie down!",
	"Pretty scary!",
	"I have a suggestion.",
	"Now with extra hugs!",
	"Almost C++98!", // "Almost C++11!",
	"Woah.",
	"HURNERJSGER?",
	"What's up, Doc?",

	// custom:
	"https://github.com/ReMinecraftPE/mcpe",
	"100% (render)dragon free!",
	"Also try Minecraft!", // Notch
	"Also try Noita!",
	"Also try Castle Crashers!",
	"Also try Unturned!",
	"Also try Minecraft Pi Reborn!", // TheBrokenRail
	"Also try RePocket!", // atipls
	"Also try PEtoLE!", // Wilyicaro
	"Controller support!",
	"Check out our GitHub!",
	"Flint and steel!",
	"I ... am Steve!",
	"Nostalgic!",
	"Now with graphics abstraction!",
	"Now with HAL!",
	"Supports PowerPC!",
	// These guys carried
	"The Work of Aron Nieminen!",      // https://minecraft.wiki/w/Aron_Nieminen
	"The Work of Johan Bernhardsson!", // https://minecraft.wiki/w/Johan_Bernhardsson
	"The Work of Tommaso Checchi!",     // https://minecraft.wiki/w/Tommaso_Checchi
	"Woo, forge!",
	"Woo, fabric!",
	"Woo, newgrounds!",
};

StartMenuScreen::StartMenuScreen() :
	m_startButton  (0, 0, 160, 24, "Start Game"),
	m_joinButton   (0, 0, 160, 24, "Join Game"),
	m_optionsButton(0, 0,  78, 22, "Options"),
	m_buyButton    (0, 0,  78, 22, "Buy"),
	m_creditsButton(0, 0,  78, 22, "")
{
	m_chosenSplash = -1;

	// note: do it here because we don't want the title to
	// show up differently when you resize
}

StartMenuScreen::~StartMenuScreen()
{
}

void StartMenuScreen::_updateLicense()
{
	int licenseID = m_pMinecraft->getLicenseId();
	if (licenseID < 0)
	{
		m_optionsButton.setEnabled(false);
		m_startButton.setEnabled(false);
		m_joinButton.setEnabled(false);
	}
	else if (licenseID <= 1)
	{
		m_optionsButton.setEnabled(true);
		m_startButton.setEnabled(true);
#ifdef FEATURE_NETWORKING
		m_joinButton.setEnabled(true);
#else
		m_joinButton.setEnabled(false);
#endif
	}
	else
	{
		m_pMinecraft->setScreen(new InvalidLicenseScreen(licenseID, m_pMinecraft->platform()->hasBuyButtonWhenInvalidLicense()));
	}
}

void StartMenuScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_startButton.getId())
	{
#if defined(DEMO)
		m_pMinecraft->selectLevel("_DemoLevel", "_DemoLevel", int(getEpochTimeS()));
#else
		m_pMinecraft->setScreen(new SelectWorldScreen);
#endif
	}
	else if (pButton->getId() == m_joinButton.getId())
	{
		m_pMinecraft->locateMultiplayer();
		m_pMinecraft->setScreen(new JoinGameScreen);
	}
	else if (pButton->getId() == m_buyButton.getId())
	{
#ifdef TITLE_CROP_MODE
		TitleTile::regenerate();
		return;
#endif
        
#ifdef DEMO
        m_pMinecraft->platform()->buyGame();
#else
		m_pMinecraft->quit();
#endif
	}
	else if (pButton->getId() == m_optionsButton.getId())
	{
		switch (m_uiTheme)
		{
		case UI_CONSOLE:
			m_pMinecraft->setScreen(new ConsoleSettingsScreen(this));
			break;
		default:
			m_pMinecraft->setScreen(new OptionsScreen(this));
			break;
		}
	}
	else if (pButton->getId() == m_creditsButton.getId())
	{
		m_pMinecraft->setScreen(new CreditsScreen(this));
	}
}

void StartMenuScreen::init()
{
	m_uiTheme = m_pMinecraft->getOptions()->getUITheme() ;

	bool consoleUI = m_uiTheme == UI_CONSOLE;

	if (consoleUI)
	{
		m_startButton.m_width = m_joinButton.m_width = m_optionsButton.m_width = m_buyButton.m_width = 450;
		m_startButton.m_height = m_joinButton.m_height = m_optionsButton.m_height = m_buyButton.m_height = 40;

		m_startButton.m_yPos = m_height / 3 + 10;
		m_joinButton.m_yPos = m_startButton.m_yPos + 50;
		m_optionsButton.m_yPos = m_joinButton.m_yPos + 50;
		m_buyButton.m_yPos = m_optionsButton.m_yPos + 50;

		int x1 = (m_width - m_startButton.m_width) / 2;

		m_startButton.m_xPos = x1;
		m_joinButton.m_xPos = x1;
		m_optionsButton.m_xPos = x1;
		m_buyButton.m_xPos = x1;
	}
	else
	{
		int yPos = m_height / 2;

		m_startButton.m_width = m_joinButton.m_width = 160;
		m_optionsButton.m_width = m_buyButton.m_width = 78;
		m_startButton.m_height = m_joinButton.m_height = m_optionsButton.m_height = m_buyButton.m_height = 25;

		m_joinButton.m_yPos = yPos + 25;
		m_startButton.m_yPos = yPos - 3;

		yPos += 55;

		m_optionsButton.m_yPos = yPos;
		m_buyButton.m_yPos = yPos;

		m_startButton.m_xPos = (m_width - m_startButton.m_width) / 2;

		int x1 = m_width - m_joinButton.m_width;

		m_joinButton.m_xPos = x1 / 2;
		m_optionsButton.m_xPos = x1 / 2;

		m_buyButton.m_xPos = x1 / 2 + m_optionsButton.m_width + 4;
	}

	m_creditsButton.m_xPos = 0;
	m_creditsButton.m_yPos = 0;
	m_creditsButton.m_width = m_width;
	m_creditsButton.m_height = 75;
	m_creditsButton.m_color.a = 0.0f;

	// fill in empty space where quit/buy button would be
	if (m_pMinecraft->isTouchscreen())
	{
		m_optionsButton.m_xPos = m_startButton.m_xPos;
		m_optionsButton.m_width = m_startButton.m_width;
	}

	// add the buttons to the screen:
	_addElement(m_startButton);
	_addElement(m_joinButton);
	_addElement(m_optionsButton);

    bool canQuit = false;

#if defined(DEMO) || !MC_PLATFORM_MOBILE
	canQuit = true;
#endif

	if (canQuit)
    {
        _addElement(m_buyButton);
    }

	_addElement(m_creditsButton, false);

	m_watermarkText = "\xFFMojang AB";
	m_watermarkX = m_width - 1 - m_pFont->width(m_watermarkText);

	m_versionText = m_pMinecraft->getVersionString();
#ifdef DEMO
		" (Demo)"
#endif
		;

	m_versionTextX = (m_width - m_pFont->width(m_versionText)) / 2;

#ifndef DEMO
	m_buyButton.setMessage("Quit");
#endif

	_updateLicense();
}

bool StartMenuScreen::isInGameScreen()
{
	return false;
}

void StartMenuScreen::render(float f)
{
#ifdef TITLE_CROP_MODE
	fill(0, 0, width, height, 0xFF00FF00);
#else
	//renderBackground();
	renderMenuBackground(f);
#endif

	//int titleYPos = 4;
	//int titleYPos = 30; // -- MC Java position.
	int titleYPos = 15;
	bool crampedMode = false;

	if (m_width * 3 / 4 < 256)
	{
		crampedMode = true;
		titleYPos = 4;
	}

	LogoRenderer::singleton().render(f);

	if (m_uiTheme != UI_CONSOLE)
	{
		drawString(*m_pFont, m_versionText, m_versionTextX, 58 + titleYPos, Color(204, 204, 204));
		drawString(*m_pFont, m_watermarkText, m_watermarkX, m_height - 10, Color::WHITE);
	}

	// Draw the splash text, if we have enough room.
#ifndef TITLE_CROP_MODE
	if (!crampedMode)
		drawSplash();
#endif

	Screen::render(f);
}

void StartMenuScreen::tick()
{
	Screen::tick();
	_updateLicense();
}

void StartMenuScreen::drawSplash()
{
	MatrixStack::Ref mtx = MatrixStack::World.push();

	std::string splashText = getSplashString();
	int textWidth = m_pFont->width(splashText);
	//int textHeight = m_pFont->height(splashText);
	if (m_uiTheme == UI_CONSOLE)
	{
		mtx->translate(Vec3(float(m_width) / 2.0f + 230.0f, 170.0f, 0.0f));
		mtx->rotate(-15.0f, Vec3::UNIT_Z);
	}
	else
	{
		mtx->translate(Vec3(float(m_width) / 2.0f + 90.0f, 70.0f, 0.0f));
		mtx->rotate(-20.0f, Vec3::UNIT_Z);
	}

	float timeMS = float(getTimeMs() % 1000) / 1000.0f;
	float scale = 1.8f - Mth::abs(0.1f * Mth::sin(2.0f * float(M_PI) * timeMS));
	scale = (scale * 100.0f) / (32.0f + textWidth);
	if (m_uiTheme == UI_CONSOLE)
		scale *= 3;
	mtx->scale(scale);

	drawCenteredString(*m_pFont, splashText, 0, -8, Color::YELLOW);
}

std::string StartMenuScreen::getSplashString()
{
	if (m_chosenSplash == -1)
	{
		Random random;
		m_chosenSplash = random.nextInt(int(sizeof(gSplashes) / sizeof(*gSplashes)));
	}

	return std::string(gSplashes[m_chosenSplash]);
}

bool StartMenuScreen::handleBackEvent(bool b)
{
	if (!b)
	{
		m_pMinecraft->quit();
	}
	return true;
}