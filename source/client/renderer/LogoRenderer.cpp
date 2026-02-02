#include "LogoRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/ShaderConstants.hpp"
#include "client/renderer/Textures.hpp"

// special mode so that we can crop out the title:
//#define TITLE_CROP_MODE

#define C_TITLE_PATH_DEFAULT  "title/mclogo.png"
#define C_TITLE_PATH_FALLBACK "gui/title.png" // everyone should have this
#define C_TITLE_PATH_POCKET   "gui/title_pe.png"
#define C_TITLE_PATH_XBOX360  "gui/title_xbox360.png"
#define C_TITLE_PATH_CONSOLE   "gui/title_console.png"

const char gLogoLine1[] = "??? ??? #   # # #   # ### ### ### ### ### ### $$$ $$$";
const char gLogoLine2[] = "? ? ?   ## ## # ##  # #   #   # # # # #    #  $ $ $  ";
const char gLogoLine3[] = "??  ??  # # # # # # # ##  #   ##  ### ##   #  $$  $$ ";
const char gLogoLine4[] = "? ? ?   #   # # #  ## #   #   # # # # #    #  $   $  ";
const char gLogoLine5[] = "? ? ??? #   # # #   # ### ### # # # # #    #  $   $$$";

const char* gLogoLines[] = {
	gLogoLine1,
	gLogoLine2,
	gLogoLine3,
	gLogoLine4,
	gLogoLine5,
};

LogoRenderer* LogoRenderer::instance = nullptr;

void LogoRenderer::init(Minecraft* mc)
{
	m_pMinecraft = mc;
	_initTextures();

	TitleTile::regenerate();
}

LogoRenderer::LogoRenderer()
{
	m_pTiles = nullptr;
	m_pMinecraft = nullptr;
}

LogoRenderer::~LogoRenderer()
{
	SAFE_DELETE_ARRAY(m_pTiles);
}

void LogoRenderer::build(int width)
{
	m_width = width;
	_build2dTitleMesh();
}

void LogoRenderer::tick()
{
	if (m_pTiles)
	{
		int Width = int(sizeof gLogoLine1 - 1);
		int Height = int(sizeof gLogoLines / sizeof * gLogoLines);
		for (int i = 0; i < Width * Height; i++)
			m_pTiles[i]->tick();
	}
}

void LogoRenderer::render(float f)
{
	if (m_pMinecraft->getOptions()->m_b2dTitleLogo)
		render2D();
	else
		render3D(f);
}

void LogoRenderer::_initTextures()
{
	if (!m_p2dTitleTexPath.empty())
		return;

	Textures* tx = m_pMinecraft->m_pTextures;
	std::string path;

	switch (m_pMinecraft->getOptions()->getLogoTheme())
	{
	case LOGO_POCKET:
		path = C_TITLE_PATH_POCKET;
		break;
	case LOGO_JAVA:
		path = C_TITLE_PATH_DEFAULT;
		break;
	case LOGO_CONSOLE:
		path = C_TITLE_PATH_CONSOLE;
		break;
	case LOGO_XBOX360:
		path = C_TITLE_PATH_XBOX360;
		break;
	default:
		break;
	}

	if (!tx->getTextureData(path, false))
	{
		path = C_TITLE_PATH_FALLBACK;
		// "preload" texture data
		tx->getTextureData(path, true);
	}

	m_p2dTitleTexPath = path;
}

void LogoRenderer::_build2dTitleMesh()
{
	// bool crampedMode = false;

	int yPos, width, height, left;

	TextureData* pTex = m_pMinecraft->m_pTextures->getTextureData(m_p2dTitleTexPath, true);
	if (!pTex)
		return;

	bool isConsole = m_pMinecraft->m_pScreen && m_pMinecraft->m_pScreen->m_uiTheme == UI_CONSOLE;

	switch (m_pMinecraft->getOptions()->getLogoTheme())
	{
	case LOGO_POCKET:
	{
		yPos = 15;
		width = pTex->m_imageData.m_width;
		height = pTex->m_imageData.m_height;

		if (isConsole)
		{
			yPos = 56;
			width *= 2;
			height *= 2;
		}

		left = (m_width - width) / 2;

		if (m_width * 3 / 4 < m_2dTitleBounds.w)
		{
			// crampedMode = true;
			yPos = 4;
		}

		m_2dTitleBounds.x = left;
		m_2dTitleBounds.y = yPos;
		m_2dTitleBounds.w = width;
		m_2dTitleBounds.h = height;

		blit(m_2dTitleMesh, m_2dTitleBounds);
		break;
	}
	case LOGO_JAVA:
	{
		yPos = 30;
		width = 274;
		int halfWidth = 155;
		height = 44;

		if (isConsole)
		{
			yPos = 56;
			width *= 2;
			halfWidth *= 2;
			height *= 2;
		}

		left = m_width / 2 - width / 2;

		if (m_width * 3 / 4 < m_2dTitleBounds.w)
		{
			// crampedMode = true;
			yPos = 4;
		}

		Tesselator& t = Tesselator::instance;
		t.begin(8);
		t.vertexUV(left, yPos + height, 0, 0.0f, 44.0f / 256.0f);
		t.vertexUV(left + halfWidth, yPos + height, 0, 155.0f / 256.0f, 44.0f / 256.0f);
		t.vertexUV(left + halfWidth, yPos, 0, 155.0f / 256.0f, 0.0f);
		t.vertexUV(left, yPos, 0, 0.0f, 0.0f);
		t.vertexUV(left + halfWidth, yPos + height, 0, 0.0f, (45.0f + 44.0f) / 256.0f);
		t.vertexUV(left + halfWidth * 2, yPos + height, 0, 155.0f / 256.0f, (45.0f + 44.0f) / 256.0f);
		t.vertexUV(left + halfWidth * 2, yPos, 0, 155.0f / 256.0f, 45.0f / 256.0f);
		t.vertexUV(left + halfWidth, yPos, 0, 0.0f, 45.0f / 256.0f);
		m_2dTitleMesh = t.end();
		break;
	}
	case LOGO_XBOX360:
	case LOGO_CONSOLE:
	{
		yPos = 56;
		width = 571;
		height = 138;

		if (!isConsole)
		{
			yPos = 15;
			width /= 2;
			height /= 2;
		}

		left = (m_width - width) / 2;


		m_2dTitleBounds.x = left;
		m_2dTitleBounds.y = yPos;
		m_2dTitleBounds.w = width;
		m_2dTitleBounds.h = height;

		blit(m_2dTitleMesh, m_2dTitleBounds);
		break;
	}
	default:
		break;
	}
}

void LogoRenderer::render2D()
{
	currentShaderColor = Color::WHITE;
	m_pMinecraft->m_pTextures->loadAndBindTexture(m_p2dTitleTexPath);
	m_2dTitleMesh.render(m_materials.ui_textured);
}

void LogoRenderer::render3D(float f)
{
	bool isConsole = m_pMinecraft->m_pScreen && m_pMinecraft->m_pScreen->m_uiTheme == UI_CONSOLE;

	int Width = int(sizeof gLogoLine1 - 1);
	int Height = int(sizeof gLogoLines / sizeof * gLogoLines);

	if (!m_pTiles)
	{
		m_pTiles = new TitleTile * [Width * Height];

		for (int y = 0; y < Height; y++)
			for (int x = 0; x < Width; x++)
				m_pTiles[y * Width + x] = new TitleTile(m_random, x, y);
	}

	int titleHeight = int(120 / Gui::InvGuiScale);

	if (m_width * 3 / 4 < 256) // cramped mode
		titleHeight = int(80 / Gui::InvGuiScale);

	if (isConsole)
		titleHeight *= 2;

	MatrixStack::Ref projMtx = MatrixStack::Projection.pushIdentity();
	projMtx->setPerspective(70.0f, float(Minecraft::width) / titleHeight, 0.05f, 100.0f);

	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();

	mce::ViewportOrigin viewportOrigin;
	{
		viewportOrigin.leftX = 0;
		viewportOrigin.bottomLeftY = Minecraft::height - titleHeight;
		viewportOrigin.topLeftY = 0;
	}
	renderContext.setViewport(Minecraft::width, titleHeight, 0.0f, 0.7f, viewportOrigin);

	MatrixStack::Ref viewMtx = MatrixStack::View.pushIdentity();

	mce::MaterialPtr* pMaterial;

	for (int i = 0; i < 2; i++)
	{
		MatrixStack::Ref matrix = MatrixStack::World.push();
		matrix->translate(Vec3(0.4f, 0.6f, -12.0f));
		switch (i)
		{
		case 0: // shadow
			//glClear(GL_DEPTH_BUFFER_BIT);
			renderContext.clearDepthStencilBuffer();
			matrix->translate(Vec3(0.0f, -0.5f, -0.5f));
			//glEnable(GL_BLEND);
			//force set alpha
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default
			break;

		case 1: // tiles
			//glDisable(GL_BLEND);
			//glClear(GL_DEPTH_BUFFER_BIT);
			renderContext.clearDepthStencilBuffer();
			//revert
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default
			break;

		case 2: // tiles again
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_COLOR, GL_ONE);
			break;
		}

		matrix->scale(Vec3(1.0f, -1.0f, 1.0f));
		matrix->rotate(8.0f, Vec3::UNIT_X);
		//matrix->rotate(15.0f, Vec3::UNIT_X);
		matrix->scale(Vec3(0.89f, 1.0f, 0.4f));
		matrix->translate(Vec3(-Width * 0.5f, -Height * 0.5f, 0.0f));

		m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		if (i == 0)
		{
			m_pMinecraft->m_pTextures->loadAndBindTexture("gui/black.png");
			pMaterial = &m_screenMaterials.ui_title_tile_shadow;
			//currentShaderColor = Color(0, 0, 0, 100);
		}
		else
		{
			pMaterial = &m_screenMaterials.ui_title_tile;
			currentShaderColor = Color::WHITE;
		}

		for (int y = 0; y < Height; y++)
		{
			for (int x = 0; x < Width; x++)
			{
				if (gLogoLines[y][x] == ' ')
					continue;

				Tile* pTile = TitleTile::getTileFromChar(gLogoLines[y][x]);

				MatrixStack::Ref matrix = MatrixStack::World.push();

				TitleTile* pTTile = m_pTiles[y * Width + x];
				float z = Mth::Lerp(pTTile->lastHeight, pTTile->height, f);
				float scale = 1.0f;
				float bright = 1.0f;
				float rotation = 180.0f;

				if (i == 0)
				{
					scale = z * 0.04f + 1.0f;
					bright = 1.0f / scale;
					z = 0.0f;
				}

				matrix->translate(Vec3(x, y, z));
				matrix->scale(scale);
				matrix->scale(Vec3(-1.0f, 1.0f, 1.0f));
				matrix->rotate(rotation, Vec3::UNIT_Z);

				// rotate 90 deg on the X axis to correct lighting
				matrix->rotate(90.0f, Vec3::UNIT_X);

				m_tileRenderer.renderTile(FullTile(pTile, i == 0 ? 255 : 0), *pMaterial, bright, true);
			}
		}
	}

	renderContext.setViewport(Minecraft::width, Minecraft::height, 0.0f, 0.7f);
}

Tile* TitleTile::_tiles[3];
Random TitleTile::_random;
bool TitleTile::_firstTimeInit = true;

TitleTile::TitleTile(Random& random, int x, int y)
{
	height = float(10 + y) + 32.0f * random.nextFloat() + float(y);
	lastHeight = height;
	dropVel = 0;
}

void TitleTile::tick()
{
	lastHeight = height;

	if (height > 0.0f)
		dropVel -= 0.6f;

	height += dropVel;
	dropVel *= 0.9f;

	if (height < 0.0f)
	{
		height = 0.0f;
		dropVel = 0.0f;
	}
}

Tile* TitleTile::getTileFromChar(char c)
{
	switch (c)
	{
	case '?': return _tiles[1];
	case '$': return _tiles[2];
	default:  return _tiles[0];
	}
}

// NOTE: Using the tile enum instead of Tile::tileName->id, may want to.. not?
static const int _tileBlockList[] = {
	TILE_BOOKSHELF,
	TILE_TOPSNOW,
	TILE_GRASS,
	TILE_INFO_UPDATEGAME1,
	TILE_INFO_UPDATEGAME2,
	TILE_LEAVES_CARRIED
};
static const int _tileBlockListSize = sizeof _tileBlockList / sizeof(int);

Tile* TitleTile::getRandomTile(Tile* except1, Tile* except2)
{
	TileID id;
	for (;;)
	{
		id = _random.nextInt(256);
		for (int i = 0; i < _tileBlockListSize; i++) {
			if (_tileBlockList[i] == id) {
				// N.B. Air does not have a tile
				id = TILE_AIR;
				break;
			}
		}

		if (!Tile::tiles[id])
			continue;

		// If found a tile, check if it can be rendered
		Tile* pTile = Tile::tiles[id];
		eRenderShape renderShape = pTile->getRenderShape();
		if (!TileRenderer::canRender(renderShape) || renderShape != SHAPE_SOLID)
			continue;

		if (pTile == except1 || pTile == except2)
			continue;

		return pTile;
	}
}

void TitleTile::regenerate()
{
	if (_firstTimeInit)
	{
		_firstTimeInit = false;
		_random.setSeed(getTimeMs());
	}

	_tiles[0] = getRandomTile(nullptr, nullptr);
	_tiles[1] = getRandomTile(_tiles[0], nullptr);
	_tiles[2] = getRandomTile(_tiles[0], _tiles[1]);
}

LogoRenderer::Materials::Materials()
{
	MATERIAL_PTR(common, ui_title_tile);
	MATERIAL_PTR(common, ui_title_tile_shadow);
}