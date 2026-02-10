#pragma once

#include "client/options/Options.hpp"
#include "client/gui/IntRectangle.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "client/gui/GuiComponent.hpp"

class LogoRenderer;

class TitleTile
{
public:
    TitleTile(Random&, int x, int y);
    void tick();

    static Tile* getTileFromChar(char c);
    static void regenerate();

protected:
    float height;
    float lastHeight;
    float dropVel;

private:
    friend class LogoRenderer;
    static Tile* _tiles[3];
    static bool _firstTimeInit;
    static Random _random;
    static Tile* getRandomTile(Tile* except1, Tile* except2);
};

class LogoRenderer : public GuiComponent
{
public:
    LogoRenderer();
    ~LogoRenderer();

    void init(Minecraft*);

    void build(int width);
    void build();

    void tick();

    void render(float f);

    void render2D();
    void render3D(float f);

    static LogoRenderer& singleton()
    {
        if (!instance)
        {
            instance = new LogoRenderer;
        }

        return *instance;
    }

private:
    void _initTextures();
    void _build2dTitleMesh();

private:
    class Materials
    {
    public:
        mce::MaterialPtr ui_title_tile;
        mce::MaterialPtr ui_title_tile_shadow;

        Materials();
    };

    Minecraft* m_pMinecraft;
 
    int m_width;
    std::string m_p2dTitleTexPath;
    mce::Mesh m_2dTitleMesh;
    IntRectangle m_2dTitleBounds;
    Random m_random;
    TileRenderer m_tileRenderer;
    TitleTile** m_pTiles;
    Materials m_screenMaterials;
    static LogoRenderer* instance;
};