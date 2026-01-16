/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Screen.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/hal/interface/RasterizerState.hpp"

Screen::Materials::Materials()
{
	MATERIAL_PTR(common, ui_cubemap);
	MATERIAL_PTR(common, ui_background);
}

bool Screen::_isPanoramaAvailable = false;

Screen::Screen()
{
	m_width = 1;
	m_height = 1;
	field_10 = false;
	m_tabButtonIndex = 0;
	m_pClickedButton = 0;
	m_yOffset = -1;
	m_pFont = nullptr;
	m_pMinecraft = nullptr;
}

Screen::~Screen()
{
	m_pClickedButton = nullptr;
	m_buttons.clear();
}

void Screen::init(Minecraft* pMinecraft, int a3, int a4)
{
	m_width  = a3;
	m_height = a4;
	m_pMinecraft = pMinecraft;
	m_pFont = pMinecraft->m_pFont;
	init();
	updateTabButtonSelection();
}

void Screen::keyPressed(int key)
{
	if (m_pMinecraft->getOptions()->isKey(KM_MENU_CANCEL, key))
	{
		m_pMinecraft->handleBack(false);
	}

	if (m_pMinecraft->useController() && m_buttonTabList.size() > 0)
	{
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_NEXT, key))
		{
			m_tabButtonIndex++;
			if (m_tabButtonIndex == int(m_buttonTabList.size()))
				m_tabButtonIndex = 0;
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_PREVIOUS, key))
		{
			m_tabButtonIndex--;
			if (m_tabButtonIndex == -1)
				m_tabButtonIndex = int(m_buttonTabList.size() - 1);
		}
		if (m_pMinecraft->getOptions()->isKey(KM_MENU_OK, key))
		{
			if (m_buttonTabList[m_tabButtonIndex]->m_bEnabled)
			{
				m_pMinecraft->m_pSoundEngine->playUI("random.click");
				buttonClicked(m_buttonTabList[m_tabButtonIndex]);
			}
		}

		updateTabButtonSelection();
	}
	
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->keyPressed(key);
	}
}

void Screen::keyboardNewChar(char chr)
{
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->charPressed(chr);
	}
}

void Screen::keyboardTextPaste(const std::string& text)
{
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->pasteText(text);
	}
}

void Screen::handleScroll(bool down)
{
}

static const char* g_panoramaList[] =
{
	"gui/background/panorama_0.png",
	"gui/background/panorama_1.png",
	"gui/background/panorama_2.png",
	"gui/background/panorama_3.png",
	"gui/background/panorama_4.png",
	"gui/background/panorama_5.png",
};

static float g_panoramaAngle = 0.0f;

void Screen::renderMenuBackground(float f)
{
	if (!m_pMinecraft->getOptions()->m_bMenuPanorama)
	{
		renderDirtBackground(0);
		return;
	}

	Textures* pTextures = m_pMinecraft->m_pTextures;

	g_panoramaAngle += float(30.0 * m_pMinecraft->m_fDeltaTime);

	float aspectRatio = 1.0f;
	//aspectRatio = float(m_width) / float(m_height);

	// @HAL: this should be using ui_cubemap, but for whatever reason we need to disable culling
	mce::MaterialPtr& materialPtr = m_screenMaterials.ui_background;

	{
		MatrixStack::Ref projMtx = MatrixStack::Projection.pushIdentity();
		projMtx->setPerspective(120.0f, aspectRatio, 0.05f, 10.0f);

		MatrixStack::Ref viewMtx = MatrixStack::View.pushIdentity();
		MatrixStack::Ref worldMtx = MatrixStack::World.push();
		currentShaderColor = Color::WHITE;
		worldMtx->rotate(180.0f, Vec3::UNIT_X);
		worldMtx->rotate(Mth::sin((f + g_panoramaAngle) / 400.0f) * 25.0f + 20.0f, Vec3::UNIT_X);
		worldMtx->rotate(-0.1f * (f + g_panoramaAngle), Vec3::UNIT_Y);

		for (int i = 0; i < 6; i++)
		{
			MatrixStack::Ref mtx = MatrixStack::World.push();

			float ang = 0.0f;
			Vec2 axis;
			switch (i)
			{
			case 1:
				ang = 90.0f;
				axis = Vec2::UNIT_Y;
				break;
			case 2:
				ang = 180.0f;
				axis = Vec2::UNIT_Y;
				break;
			case 3:
				ang = -90.0f;
				axis = Vec2::UNIT_Y;
				break;
			case 4:
				ang = 90.0f;
				axis = Vec2::UNIT_X;
				break;
			case 5:
				ang = -90.0f;
				axis = Vec2::UNIT_X;
				break;
			default:
				goto skip_rotate;
			}

			mtx->rotate(ang, Vec3(axis.x, axis.y, 0.0f));

		skip_rotate:
			pTextures->setSmoothing(true);
			pTextures->setClampToEdge(true);
			pTextures->loadAndBindTexture(std::string(g_panoramaList[i]));
			pTextures->setSmoothing(false);
			pTextures->setClampToEdge(false);

			Tesselator& t = Tesselator::instance;
			t.begin(4);
			t.vertexUV(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f);
			t.vertexUV(+1.0f, -1.0f, 1.0f, 1.0f, 0.0f);
			t.vertexUV(+1.0f, +1.0f, 1.0f, 1.0f, 1.0f);
			t.vertexUV(-1.0f, +1.0f, 1.0f, 0.0f, 1.0f);
			t.draw(materialPtr);
		}
	}

	fillGradient(0, 0, m_width, m_height, Color(0, 0, 0, 137), Color(255, 255, 255, 137));
}

void Screen::mouseClicked(int xPos, int yPos, int d) // d = clicked?
{
	if (!d) return;
	
	for (int i = 0; i < int(m_buttons.size()); i++)
	{
		Button* button = m_buttons[i];
		if (button->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pClickedButton = button;

			if (!m_pMinecraft->isTouchscreen())
			{
				m_pMinecraft->m_pSoundEngine->playUI("random.click");
				buttonClicked(button);
			}
		}
	}

#ifndef ORIGINAL_CODE
	// Iterate over focused text inputs first. This is because if changing
	// focus, the previous focus must hide the OSK, before the new focus
	// shows it.
	for (int phase = 0; phase < 2; phase++)
	{
		bool handleFocused = phase == 0;
		for (int i = 0; i < int(m_textInputs.size()); i++)
		{
			TextInputBox* textInput = m_textInputs[i];
			if (textInput->m_bFocused == handleFocused)
			{
				textInput->onClick(xPos, yPos);
			}
		}
	}

#ifdef USE_NATIVE_ANDROID
	// if the keyboard is shown:
	if (m_pMinecraft->platform()->getKeyboardUpOffset())
	{
		// if there are none focused at the moment:
		bool areAnyFocused = false;
		for (int i = 0; i < int(m_textInputs.size()); i++)
		{
			TextInputBox* textInput = m_textInputs[i];
			if (textInput->m_bFocused)
			{
				areAnyFocused = true;
				break;
			}
		}

		if (!areAnyFocused)
			m_pMinecraft->platform()->hideKeyboard();
	}
#endif
#endif
}

void Screen::mouseReleased(int xPos, int yPos, int d)
{
	if (!d) return;

	if (m_pClickedButton)
	{
		if (m_pMinecraft->isTouchscreen() && m_pClickedButton->clicked(m_pMinecraft, xPos, yPos))
		{
			m_pMinecraft->m_pSoundEngine->playUI("random.click");
			buttonClicked(m_pClickedButton);
		}
		m_pClickedButton->released(xPos, yPos);
		m_pClickedButton = nullptr;
	}
}

void Screen::render(int xPos, int yPos, float unused)
{
	for (int i = 0; i < int(m_buttons.size()); i++)
	{
		Button* button = m_buttons[i];
		button->render(m_pMinecraft, xPos, yPos);
	}

#ifndef ORIGINAL_CODE
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* textInput = m_textInputs[i];
		textInput->tick();
		textInput->render();
	}
#endif
}

void Screen::tick()
{
	g_panoramaAngle++;
}

void Screen::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	// not original code. Will need to re-init again
	m_buttons.clear();
	m_buttonTabList.clear();
	m_textInputs.clear();
	init();
}

void Screen::onRender(int mouseX, int mouseY, float f)
{
	m_yOffset = getYOffset();

	MatrixStack::Ref matrix;

	if (m_yOffset != 0)
	{
		// push the entire screen up
		matrix = MatrixStack::World.push();
		matrix->translate(Vec3(0.0f, -m_yOffset, 0.0f));
	}

	render(mouseX, mouseY, f);
}

int Screen::getYOffset()
{
#ifdef USE_NATIVE_ANDROID
	int keybOffset = m_pMinecraft->platform()->getKeyboardUpOffset();
	if (!keybOffset)
		return 0;

	int offset = 0;

	// look through every text box, see if one's open
	// and determine its offset from there
	for (int i = 0; i < int(m_textInputs.size()); i++)
	{
		TextInputBox* pBox = m_textInputs[i];

		if (!pBox->m_bFocused)
			continue;
		
		int heightLeft = m_height - int(float(keybOffset) * Gui::InvGuiScale);

		// we want to keep the center of the text box in the center of the screen
		int textCenterY = pBox->m_yPos + pBox->m_height / 2;
		int scrnCenterY = heightLeft / 2;

		int diff = textCenterY - scrnCenterY;
		
		// Prevent the difference from revealing the outside of the screen.
		if (diff > m_height - heightLeft)
			diff = m_height - heightLeft;
		if (diff < 0)
			diff = 0;

		offset = diff;
		break;
	}

	return offset;
#else
	return 0;
#endif
}

void Screen::updateEvents()
{
	if (field_10) return;

	while (Mouse::next())
		mouseEvent();

	while (Keyboard::next())
		keyboardEvent();
}

void Screen::keyboardEvent()
{
	if (Keyboard::getEventKeyState())
		keyPressed(Keyboard::getEventKey());
}

void Screen::mouseEvent()
{
	MouseAction* pAction = Mouse::getEvent();
	if (pAction->isButton())
	{
		if (Mouse::getEventButtonState())
			mouseClicked (m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset(), Mouse::getEventButton());
		else
			mouseReleased(m_width * pAction->_posX / Minecraft::width, m_height * pAction->_posY / Minecraft::height - 1 + getYOffset(), Mouse::getEventButton());
	}
	if (pAction->_buttonType == BUTTON_SCROLLWHEEL)
		handleScroll(Mouse::getEventButtonState());
}

void Screen::renderBackground(int vo)
{
	if (m_pMinecraft->isLevelGenerated())
	{
		// draw the background offset by the Y offset so that the smaller virtual
		// keyboards don't reveal undrawn areas
		fillGradient(0, m_yOffset, m_width, m_height, Color(16, 16, 16, 192), Color(16, 16, 16, 208)); // 0xC0101010, 0xD0101010
	}
	else
	{
		renderDirtBackground(vo);
	}
}

void Screen::renderBackground()
{
	renderBackground(0);
}

void Screen::renderDirtBackground(int vo)
{
	m_pMinecraft->m_pTextures->loadAndBindTexture("gui/background.png");
	currentShaderColor = Color::WHITE;

	Tesselator& t = Tesselator::instance;
	t.begin(4);
	//t.setOffset(0, m_yOffset, 0);
	t.color(0x404040);
	t.vertexUV(0.0f,           float(m_height), 0, 0,                      float(vo) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), float(m_height), 0, float(m_width) / 32.0f, float(vo) + float(m_height) / 32.0f);
	t.vertexUV(float(m_width), 0,               0, float(m_width) / 32.0f, float(vo) + 0.0f);
	t.vertexUV(0.0f,           0,               0, 0,                      float(vo) + 0.0f);
	//t.setOffset(0, 0, 0);
	t.draw(m_materials.ui_texture_and_color);
}


void Screen::updateTabButtonSelection()
{
	if (m_pMinecraft->useController())
	{
		for (int i = 0; i < int(m_buttonTabList.size()); i++)
		{
			m_buttonTabList[i]->m_bHovered = m_tabButtonIndex == i;
		}
	}
}
