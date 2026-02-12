/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "client/app/Minecraft.hpp"
#include "client/gui/screens/IngameBlockSelectionScreen.hpp"
#include "client/gui/screens/ChatScreen.hpp"
#include "client/gui/screens/inventory/InventoryScreen.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "renderer/ShaderConstants.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

Gui::Materials::Materials()
{
	MATERIAL_PTR(common, ui_vignette);
	MATERIAL_PTR(common, ui_overlay);
	MATERIAL_PTR(common, ui_invert_overlay);
	MATERIAL_PTR(common, ui_overlay_textured);
	MATERIAL_PTR(common, ui_invert_overlay_textured);
	MATERIAL_PTR(common, ui_crosshair);
}

#ifdef ENH_USE_GUI_SCALE_2
float Gui::GuiScale = 1.0f / 2.0f;
#else
float Gui::GuiScale = 1.0f / 3.0f;
#endif
int Gui::GuiWidth = Minecraft::width;
int Gui::GuiHeight = Minecraft::height;

bool Gui::_isVignetteAvailable = false; // false because PE never seemed to have it

Gui::Gui(Minecraft* pMinecraft)
{
	m_progress = 0;
	field_C = "";
	field_24 = 0;
	field_28 = 0;
	field_2C = 0;
	m_ticks = 0;
	field_A00 = "";
	field_A18 = 0;
	field_A1C = false;
	m_tbr = 1.0f;
	field_A3C = true;
	m_bRenderMessages = true;
    m_bRenderHunger = false;

	m_pMinecraft = pMinecraft;
}

void Gui::addMessage(const std::string& s)
{
	// if the message contains a new line, add each line separately:
	if (s.find("\n") != std::string::npos)
	{
		std::stringstream ss(s);
		std::string line;
		while (std::getline(ss, line))
			addMessage(line);

		return;
	}

	std::string str = s;

	while (m_pMinecraft->m_pFont->width(str) > 320)
	{
		size_t i = 2;
		for (; i < str.size(); i++)
		{
			std::string sstr = str.substr(0, i);

			// this sucks
			if (m_pMinecraft->m_pFont->width(sstr) > 320)
				break;
		}

		std::string sstr = str.substr(0, i - 1);
		addMessage(sstr);
		str = str.substr(i - 1);
	}

	if (m_guiMessages.size() > 50)
	{
		m_guiMessages.erase(m_guiMessages.end() - 1);
	}

	m_guiMessages.insert(m_guiMessages.begin(), GuiMessage(str, 0));
}

void Gui::setNowPlaying(const std::string& str)
{
	field_A00 = "Now playing: " + str;
	field_A18 = 60;
	field_A1C = true;
}

void Gui::renderPumpkin(int var1, int var2)
{
	currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color::WHITE;

	m_pMinecraft->m_pTextures->setSmoothing(true);
	m_pMinecraft->m_pTextures->loadAndBindTexture("/misc/pumpkinblur.png");
	m_pMinecraft->m_pTextures->setSmoothing(false);

	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.vertexUV(0.0f, var2, -90.0f, 0.0f, 1.0f);
	t.vertexUV(var1, var2, -90.0f, 1.0f, 1.0f);
	t.vertexUV(var1, 0.0f, -90.0f, 1.0f, 0.0f);
	t.vertexUV(0.0f, 0.0f, -90.0f, 0.0f, 0.0f);
	t.draw(m_materials.ui_textured);
}

void Gui::renderVignette(float brightness, int width, int height)
{
	brightness = 1.0f - brightness;
	brightness = Mth::clamp(brightness, 0.0f, 1.0f);

	m_tbr += ((brightness - m_tbr) * 0.01f);

	m_pMinecraft->m_pTextures->setSmoothing(true);
	m_pMinecraft->m_pTextures->loadAndBindTexture("misc/vignette.png");
	m_pMinecraft->m_pTextures->setSmoothing(false);

	// @TODO: bake this mesh and use currentShaderColor for recoloring

	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.color(m_tbr, m_tbr, m_tbr);
	t.vertexUV(0.0f,  height, -90.0f, 0.0f, 1.0f);
	t.vertexUV(width, height, -90.0f, 1.0f, 1.0f);
	t.vertexUV(width, 0.0f,   -90.0f, 1.0f, 0.0f);
	t.vertexUV(0.0f,  0.0f,   -90.0f, 0.0f, 0.0f);
	t.draw(m_guiMaterials.ui_vignette);
}

void Gui::inventoryUpdated()
{
	field_A3C = true;
}

void Gui::render(float f, bool bHaveScreen, int mouseX, int mouseY)
{
	Minecraft& mc = *m_pMinecraft;
	GameRenderer& renderer = *mc.m_pGameRenderer;
	Textures& textures = *mc.m_pTextures;
    bool isPocket = mc.getOptions()->getUiTheme() == UI_POCKET;
	bool isConsole = mc.getOptions()->getUiTheme() == UI_CONSOLE;

	renderer.setupGuiScreen();

	if (bHaveScreen && isConsole)
		return;

	if (!mc.m_pLevel || !mc.m_pLocalPlayer)
		return;

	if (mc.getOptions()->m_fancyGraphics.get() && isVignetteAvailable() && !isConsole)
	{
		renderVignette(mc.m_pLocalPlayer->getBrightness(f), GuiWidth, GuiHeight);
	}

	ItemStack& headGear = mc.m_pLocalPlayer->m_pInventory->getArmor(Item::SLOT_HEAD);

	if (!mc.getOptions()->m_thirdPerson.get() && !headGear.isEmpty() && headGear.getId() == Tile::pumpkin->m_ID)
		renderPumpkin(GuiWidth, GuiHeight);

	renderProgressIndicator(GuiWidth, GuiHeight);

	currentShaderColor = Color::WHITE;
	currentShaderDarkColor = Color::WHITE;

	MatrixStack::Ref matrix = MatrixStack::World.push();
	matrix->translate(Vec3(GuiWidth / 2, GuiHeight, 0));
	if (isConsole)
	{
		matrix->translate(Vec3(0, -35, 0));
		matrix->scale(mc.getOptions()->m_hudSize.get());
	}
	if (mc.m_pGameMode->canHurtPlayer())
	{
		textures.loadAndBindTexture("gui/icons.png");

		Tesselator& t = Tesselator::instance;
		t.begin(0);
		t.voidBeginAndEndCalls(true);

		renderHearts(isPocket);
		renderArmor(isPocket);
		renderBubbles(isPocket);
        if (m_bRenderHunger)
            renderHunger(isPocket);

		t.voidBeginAndEndCalls(false);
		t.draw(m_materials.ui_textured);

		renderExperience();
	}

	float alpha = 1.0f;
#ifndef ENH_TRANSPARENT_HOTBAR
	alpha = 0.50f; // 0.65f on 0.12.1
#endif
	renderToolBar(f, alpha);
	matrix.release();

	if (m_bRenderMessages)
	{
		renderMessages(false);
	}
}

void Gui::tick()
{
	if (field_A18 > 0)
		field_A18--;

	m_ticks++;

	for (size_t i = 0; i < m_guiMessages.size(); i++)
	{
		GuiMessage& msg = m_guiMessages[i];
		msg.field_18++;
	}
}

void Gui::renderSlot(int slot, int x, int y, float f)
{
	Inventory* pInv = m_pMinecraft->m_pLocalPlayer->m_pInventory;

	ItemStack& item = pInv->getItem(slot);
	if (item.isEmpty())
		return;

	{
		MatrixStack::Ref matrix;

		float var6 = ((float)item.m_popTime) - f;
		if (var6 > 0.0f)
		{
			float var7 = 1.0f + var6 / 5.0f;
			matrix = MatrixStack::World.push();
			matrix->translate(Vec3(x + 8, y + 12, 0.0f));
			matrix->scale(Vec3(1.0f / var7, (var7 + 1.0f) / 2.0f, 1.0f));
			matrix->translate(Vec3(-(x + 8), -(y + 12), 0.0f));
		}

		ItemRenderer::singleton().renderGuiItem(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, item, x, y, true);
	}

    //ItemRenderer::renderGuiItemDecorations(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, item, x, y);
}

void Gui::renderSlotOverlay(int slot, int x, int y, float f)
{
	Inventory* pInv = m_pMinecraft->m_pLocalPlayer->m_pInventory;

	ItemStack& item = pInv->getItem(slot);
	if (item.isEmpty())
		return;

	ItemRenderer::singleton().renderGuiItemOverlay(m_pMinecraft->m_pFont, m_pMinecraft->m_pTextures, item, x, y);
}

int Gui::getSlotIdAt(int mouseX, int mouseY)
{
	int scaledY = int(GuiScale * mouseY);
	int scaledHeight = int(GuiScale * Minecraft::height);

	if (scaledY >= scaledHeight)
		return -1;
	if (scaledY < scaledHeight - 19)
		return -1;

	int hotbarOffset = getNumSlots() * 20 / 2 - 2;

	int slotX = (int(GuiScale * mouseX) - int(GuiScale * Minecraft::width) / 2 + hotbarOffset + 20) / 20;

	if (slotX >= 0)
		slotX--;

	if (slotX >= getNumSlots())
		slotX = -1;

	return slotX;
}

bool Gui::isInside(int mouseX, int mouseY)
{
	return getSlotIdAt(mouseX, mouseY) != -1;
}

void Gui::handleClick(int clickID, int mouseX, int mouseY)
{
	if (clickID != 1)
		return;

	int slot = getSlotIdAt(mouseX, mouseY);
	if (slot == -1)
		return;

	if (m_pMinecraft->isTouchscreen() && slot == getNumSlots() - 1)
		m_pMinecraft->setScreen(new IngameBlockSelectionScreen);
	else
		m_pMinecraft->m_pLocalPlayer->m_pInventory->selectSlot(slot);
}

void Gui::handleScrollWheel(bool down)
{
	SlotID slot = m_pMinecraft->m_pLocalPlayer->m_pInventory->m_selectedSlot;

	int maxItems = getNumUsableSlots() - 1;

	if (down)
	{
		if (slot++ == maxItems)
			slot = 0;
	}
	else
	{
		if (slot-- == 0)
			slot = maxItems;
	}

	m_pMinecraft->m_pLocalPlayer->m_pInventory->selectSlot(slot);
}

void Gui::handleKeyPressed(int keyCode)
{
	Options* options = m_pMinecraft->getOptions();

	if (options->isKey(KM_INVENTORY, keyCode))
	{
		if (m_pMinecraft->m_pGameMode->isSurvivalType())
			m_pMinecraft->setScreen(new InventoryScreen(m_pMinecraft->m_pLocalPlayer));
		else
			m_pMinecraft->setScreen(new IngameBlockSelectionScreen);
		return;
	}

	bool slotL = options->isKey(KM_SLOT_L, keyCode);
	bool slotR = options->isKey(KM_SLOT_R, keyCode);
	if (slotL || slotR)
	{
		int maxItems = getNumSlots() - 1;
		if (m_pMinecraft->isTouchscreen())
			maxItems--;
		SlotID* slot = &m_pMinecraft->m_pLocalPlayer->m_pInventory->m_selectedSlot;

		if (slotR)
		{
			if (*slot < maxItems)
				(*slot)++;
			else
				*slot = 0;
		}
		else if (slotL)
		{
			if (*slot > 0)
				(*slot)--;
			else
				*slot = maxItems;
		}
		return;
	}

	if (options->isKey(KM_CHAT, keyCode) || options->isKey(KM_CHAT_CMD, keyCode))
	{
		if (!m_pMinecraft->m_pScreen)
			m_pMinecraft->setScreen(new ChatScreen(m_pMinecraft->getOptions()->isKey(KM_CHAT_CMD, keyCode)));
	}
}

void Gui::renderMessages(bool bShowAll)
{
	int topEdge = GuiHeight - 49;
	if (m_pMinecraft->isTouchscreen())
		topEdge = 49;

	for (size_t i = 0; i < m_guiMessages.size(); i++)
	{
		GuiMessage& msg = m_guiMessages[i];
		if (!bShowAll && msg.field_18 > 199)
			continue;

		int bkgdColor = 0x7F000000, textColor = 0xFFFFFFFF;

		float fade = 1.0f;

		if (!bShowAll)
		{
			fade = 10.0f * (1.0f - (float(msg.field_18) / 200.0f));
			if (fade <= 0.0f)
				continue;

			if (fade < 1.0f)
			{
				int x = int(fade * fade * 255.0f);
				if (x == 0)
					continue;

				bkgdColor = (x / 2) << 24;
				textColor = (x << 24) + 0xFFFFFF;
			}
		}

		fill(2, topEdge, 322, topEdge + 9, bkgdColor);
		m_pMinecraft->m_pFont->drawShadow(msg.msg, 2, topEdge + 1, textColor);

		topEdge -= 9;
	}
}

void Gui::renderHearts(bool topLeft)
{
	m_random.setSeed(m_ticks * 312871);

	LocalPlayer* player = m_pMinecraft->m_pLocalPlayer;

	bool b1 = player->m_invulnerableTime >= 10 && player->m_invulnerableTime / 3 % 2;

	int heartX;
	int heartYStart;

	if (topLeft)
	{
		heartX = -GuiWidth / 2 + 2;
		heartYStart = -GuiHeight + 2;
	}
	else
	{
		// @NOTE: At the default scale, this would go off screen.
		// Renders to the left of the hotbar, why?
		/*heartX = cenX - 191; // why?
		heartYStart = height - 10;*/

		heartX = -91;
		heartYStart = -32;
	}

	int playerHealth = player->m_health;
	int maxHealth = player->getMaxHealth();

	for (int healthNo = 1; healthNo <= maxHealth; healthNo += 2)
	{
		int heartY = heartYStart;

		if (playerHealth <= 4 && m_random.genrand_int32() % 2)
			heartY++;

		blit(heartX, heartY, 16 + b1 * 9, 0, 9, 9, 0, 0);

		if (b1)
		{
			if (healthNo < player->m_lastHealth)
				blit(heartX, heartY, 70, 0, 9, 9, 0, 0);
			else if (healthNo == player->m_lastHealth)
				blit(heartX, heartY, 79, 0, 9, 9, 0, 0);
		}

		if (healthNo < playerHealth)
			blit(heartX, heartY, 52, 0, 9, 9, 0, 0);
		else if (healthNo == playerHealth)
			blit(heartX, heartY, 61, 0, 9, 9, 0, 0);

		heartX += 8;
	}
}

void Gui::renderArmor(bool topLeft)
{
	int armor = m_pMinecraft->m_pLocalPlayer->m_pInventory->getArmorValue();
	if (armor <= 0)
		return;
	
	int hotbarWidth = (topLeft) ? 0 : (2 + getNumSlots() * 20);
	int armorX = (topLeft) ? (GuiWidth / 2 - 11) : - 91 + (hotbarWidth - 9);
	int armorY = (topLeft) ? 2 - GuiHeight : -32;

	if (armor > 0)
	{
		int maxHealth = m_pMinecraft->m_pLocalPlayer->getMaxHealth();
		for (int i = 1; i <= maxHealth; i += 2) // Armor tied to health at the moment
		{
			if (i < armor)
				blit(armorX, armorY, 34, 9, 9, 9, 0, 0);

			if (i == armor)
				blit(armorX, armorY, 25, 9, 9, 9, 0, 0);

			if (i > armor)
				blit(armorX, armorY, 16, 9, 9, 9, 0, 0);

			armorX -= 8;
		}
	}
}

void Gui::renderHunger(bool topLeft)
{

}

void Gui::renderBubbles(bool topLeft)
{
	LocalPlayer* player = m_pMinecraft->m_pLocalPlayer;

	if (player->isUnderLiquid(Material::water))
	{
		int breathRaw = player->m_airCapacity;
		int breathFull = int(ceilf((float(breathRaw - 2) * 10.0f) / 300.0f));
		int breathMeter = int(ceilf((float(breathRaw) * 10.0f) / 300.0f)) - breathFull;

        int bubbleX;
		int bubbleY;
        
        if (topLeft)
        {
            bubbleX = -GuiWidth / 2 + 2;
            bubbleY = -GuiHeight + 12;
        }
        else if (m_bRenderHunger)
        {
            // @TODO
            bubbleX = 2;
            bubbleY = 12;
        }
        else
        {
            // Renders to the left of the hotbar, why?
            /*bubbleX = cenX - 191;
            bubbleY = height - 19;*/
            bubbleX = -91;
            bubbleY = -41;
        }
        
		//@NOTE: Not sure this works as it should

		for (int bubbleNo = 0; bubbleNo < breathFull + breathMeter; bubbleNo++)
		{
			if (bubbleNo < breathFull)
				blit(bubbleX, bubbleY, 16, 18, 9, 9, 0, 0);
			else
				blit(bubbleX, bubbleY, 25, 18, 9, 9, 0, 0);

			bubbleX += 8;
		}
	}
}

void Gui::renderProgressIndicator(int width, int height)
{
	Minecraft& mc = *m_pMinecraft;
	Textures& textures = *mc.m_pTextures;
    
    currentShaderColor = Color::WHITE;
    currentShaderDarkColor = Color::WHITE;

	if (m_pMinecraft->useSplitControls())
	{
		// draw crosshair
		textures.loadAndBindTexture("gui/icons.png");
		MatrixStack::Ref matrix = MatrixStack::World.push();
		matrix->translate(Vec3(width / 2, height / 2, 0));
		if (mc.getOptions()->getUiTheme() == UI_CONSOLE)
			matrix->scale(mc.getOptions()->m_hudSize.get());
		blit(-8, -8, 0, 0, 16, 16, 0, 0, &m_guiMaterials.ui_crosshair);
	}
	else
	{
		IInputHolder& input = *mc.m_pInputHolder;
		// if needed, draw feedback

		// NOTE: real Minecraft PE takes it directly from the gamemode as "current progress" and
		// "last progress". Well guess what? The game mode in question updates our m_sensitivity with
		// the pre-interpolated break progress! Isn't that awesome?!
		float breakProgress = m_progress;

		// don't know about this if-structure, it feels like it'd be like
		// if (m_bFoggy >= 0.0f && breakProgress <= 0.0f)
		//     that;
		// else
		//     this;
		if (breakProgress > 0.0f || input.m_feedbackAlpha < 0.0f)
		{
			if (breakProgress > 0.0f)
			{
				float xPos = input.m_feedbackX;
				float yPos = input.m_feedbackY;

				textures.loadAndBindTexture("gui/feedback_outer.png");
				currentShaderColor = Color::WHITE;
				blit(GuiScale * xPos - 44.0f, GuiScale * yPos - 44.0f, 0, 0, 88, 88, 256, 256, &m_guiMaterials.ui_overlay_textured);

				textures.loadAndBindTexture("gui/feedback_fill.png");

				// note: scale starts from 4.0f
				float halfWidth = (40.0f * breakProgress + 48.0f) / 2.0f;

				blit(GuiScale * xPos - halfWidth, GuiScale * yPos - halfWidth, 0, 0, halfWidth * 2, halfWidth * 2, 256, 256, &m_guiMaterials.ui_invert_overlay_textured);
			}
		}
		else
		{
			float xPos = input.m_feedbackX;
			float yPos = input.m_feedbackY;

			textures.loadAndBindTexture("gui/feedback_outer.png");
			currentShaderColor = Color(1.0f, 1.0f, 1.0f, Mth::Min(1.0f, input.m_feedbackAlpha));
			blit(GuiScale * xPos - 44.0f, GuiScale * yPos - 44.0f, 0, 0, 88, 88, 256, 256, &m_guiMaterials.ui_overlay_textured);
		}
	}
}

void Gui::renderExperience()
{

}

void Gui::renderToolBar(float f, float alpha)
{
	Minecraft* mc = m_pMinecraft;
	Textures* textures = mc->m_pTextures;
	LocalPlayer* player = mc->m_pLocalPlayer;

	currentShaderColor.a = alpha;

	textures->loadAndBindTexture("gui/gui.png");

	m_blitOffset = -90.0f;

	// chat
	//blit(width - 18, 0, 200, 82, 18, 18, 18, 18);

	int nSlots = getNumSlots();
	int hotbarWidth = 2 + nSlots * 20;

	mce::MaterialPtr* material = &m_materials.ui_textured_and_glcolor;

	// hotbar
	blit(-hotbarWidth / 2, -22, 0, 0, hotbarWidth - 2, 22, 0, 0, material);
	blit(hotbarWidth / 2 - 2, -22, 180, 0, 2, 22, 0, 0, material);

	Inventory* inventory = player->m_pInventory;

	// selection mark
	blit(-1 - hotbarWidth / 2 + 20 * inventory->m_selectedSlot, -23, 0, 22, 24, 22, 0, 0, material);

	textures->loadAndBindTexture(C_BLOCKS_NAME);

	int diff = mc->isTouchscreen();

	int slotX = -hotbarWidth / 2 + 3;
	for (int i = 0; i < nSlots - diff; i++)
	{
		renderSlot(i, slotX, -19, f);

		slotX += 20;
	}

	slotX = -hotbarWidth / 2 + 3;
	for (int i = 0; i < nSlots - diff; i++)
	{
		renderSlotOverlay(i, slotX, -19, f);

		slotX += 20;
	}

#undef DIFF

	field_A3C = false;

	// blit the "more items" button
	if (mc->isTouchscreen())
	{
		textures->loadAndBindTexture(C_TERRAIN_NAME);
		blit(hotbarWidth / 2 - 19, -19, 208, 208, 16, 16, 0, 0, material);
	}
}

int Gui::getNumSlots()
{
	if (m_pMinecraft->isTouchscreen())
		return 6;

	return 9;
}

int Gui::getNumUsableSlots()
{
	return getNumSlots() - m_pMinecraft->isTouchscreen();
}

RectangleArea Gui::getRectangleArea(bool b)
{
	float centerX = Minecraft::width / 2;
	float hotbarWidthHalf = (10 * getNumSlots() + 5) / GuiScale;
	return RectangleArea(
		b ? (centerX - hotbarWidthHalf) : 0,
		Minecraft::height - 24.0f / GuiScale,
		centerX + hotbarWidthHalf,
		Minecraft::height);
}
