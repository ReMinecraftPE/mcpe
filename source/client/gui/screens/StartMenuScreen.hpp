/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "../IntRectangle.hpp"
#include "client/renderer/TileRenderer.hpp"

class StartMenuScreen;

class TitleTile
{
public:
	TitleTile(StartMenuScreen*, int x, int y);
	void tick();

	static Tile* getTileFromChar(char c);
	static void regenerate();

protected:
	friend class StartMenuScreen;
	float height;
	float lastHeight;
	float dropVel;

private:
	static Tile* _tiles[3];
	static bool _firstTimeInit;
	static Random _random;
	static Tile* getRandomTile(Tile* except1, Tile* except2);
};

class StartMenuScreen : public Screen
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr ui_title_tile;
		mce::MaterialPtr ui_title_tile_shadow;

		Materials();
	};

public:
	StartMenuScreen();
	~StartMenuScreen();

protected:
	void _initTextures();
	void _initResources();
	void _build2dTitleMesh();
	void _updateLicense();

public:
	void init() override;
	void buttonClicked(Button*) override;
	bool isInGameScreen() override;
	void render(int, int, float) override;
	void tick() override;

	void drawSplash();
	void draw2dTitle();
	void draw3dTitle(float f);

	std::string getSplashString();

	bool handleBackEvent(bool b) override;

protected:
	friend class TitleTile;

	Button m_startButton;
	Button m_joinButton;
	Button m_optionsButton;
	Button m_testButton;
	Button m_buyButton;
	Button m_creditsButton;
	std::string field_154;
	int field_16C;
	std::string field_170;
	int field_188;

	int m_chosenSplash;

	TextureData* m_p2dTitleTex;
	bool m_bUsingJavaLogo;
	mce::Mesh m_2dTitleMesh;
	IntRectangle m_2dTitleBounds;

	TileRenderer m_tileRenderer;
	Random m_random;
	TitleTile** m_pTiles;
	Materials m_screenMaterials;
};

