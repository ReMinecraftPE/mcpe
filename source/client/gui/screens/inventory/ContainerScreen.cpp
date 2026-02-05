#include "client/gui/screens/inventory/ContainerScreen.hpp"
#include "client/locale/Language.hpp"
#include "client/renderer/Lighting.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"
#include "renderer/ShaderConstants.hpp"
#include <cfloat>

ContainerScreen::ContainerScreen(ContainerMenu* menu) :
    m_pMenu(menu),
    m_imageWidth(176),
    m_imageHeight(166),
    m_leftPos(0),
    m_topPos(0),
    m_timeSlotDragged(0)
{
    m_bRenderPointer = true;
}

void ContainerScreen::_renderSlot(Slot& slot)
{
    const SlotDisplay& display = getSlotDisplay(slot);

    if (!display.bVisible) return;

    if (display.bIconHolder)
    {
        MatrixStack::Ref matrix = MatrixStack::World.push();
        float off = 3 * display.size / 50.0f;
        matrix->translate(Vec3(-off, -off, 0.0f));
        blitSprite(*m_pMinecraft->m_pTextures, "consolegui/Graphics/IconHolder.png", display.x, display.y, display.size, display.size);
    }
    MatrixStack::Ref matrix = MatrixStack::World.push();
    matrix->translate(Vec3(display.x, display.y, 0));
    matrix->scale(display.size / 18.0f);
    ItemStack& item = slot.getItem();
    if (item.isEmpty())
    {
        const std::string& noItemSprite = display.noItemSprite;

        if (!noItemSprite.empty())
        {
            blitSprite(*m_pMinecraft->m_pTextures, noItemSprite, 0, 0, 16, 16);
            return;
        }

        int icon = display.noItemIcon;
        if (icon >= 0)
        {
            m_pMinecraft->m_pTextures->loadAndBindTexture(C_ITEMS_NAME);
            blit(0, 0, (icon % 16) * 16, (icon / 16) * 16, 16, 16, 0, 0);
        }

        return;
    }
    ItemRenderer::singleton().renderGuiItem(m_pFont, m_pMinecraft->m_pTextures, item, 0, 0, true);
    ItemRenderer::singleton().renderGuiItemOverlay(m_pFont, m_pMinecraft->m_pTextures, item, 0, 0);
}

Slot* ContainerScreen::_findSlot()
{
    return _findSlot(m_menuPointer.x, m_menuPointer.y);
}

Slot* ContainerScreen::_findSlot(int mouseX, int mouseY)
{
    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        Slot* slot = *it;
        if (_isHovering(*slot)) return slot;
    }
    return nullptr;
}

Slot* ContainerScreen::_findSlotInDirection(ScreenDirection dir, bool invert)
{
    if (m_slotDisplays.empty()) return nullptr;

    float bestScore = FLT_MAX;
    Slot* nearestSlot = nullptr;
    Slot* hovered = _findSlot();

    MenuPointer pointer = m_menuPointer;

    float dirX = 0, dirY = 0;
    switch (dir)
    {
    case UP:    dirY = -1; break;
    case DOWN:  dirY = 1; break;
    case LEFT:  dirX = -1; break;
    case RIGHT: dirX = 1; break;
    default: return nullptr;
    }
    
    if (invert)
    {
        if (dirX) pointer.x = dirX < 0 ? m_width : 0;
        else if (dirY) pointer.y = dirY < 0 ? m_height : 0;
    }

    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        constexpr float maxAngle = 75.0f;
        constexpr float penalty = 1.5f;
        
        Slot* slot = *it;
        if (slot == hovered) continue;

        const SlotDisplay& display = getSlotDisplay(*slot);

        if (!display.bVisible) continue;

        float off = 3 * display.size / 50.0f;
        float tx = (m_leftPos + display.x - off + display.size / 2) - pointer.x;
        float ty = (m_topPos + display.y - off + display.size / 2) - pointer.y;

        float distance = Mth::sqrt(tx * tx + ty * ty);
        if (distance < 1e-4f) continue;

        float len = distance;
        float nx = tx / len;
        float ny = ty / len;

        float cosTheta = nx * dirX + ny * dirY;

        if (cosTheta < Mth::cos(maxAngle * M_PI / 180.0f))
            continue;

        float projectedDist = distance * cosTheta;

        float lateralPenalty = distance * Mth::sqrt(1.0f - cosTheta * cosTheta) * penalty;

        float score = projectedDist + lateralPenalty;

        if (score < bestScore)
        {
            bestScore = score;
            nearestSlot = slot;
        }
    }

    return nearestSlot;
}

bool ContainerScreen::_isHovering(const Slot& slot) const
{
    return _isHovering(slot, m_menuPointer.x, m_menuPointer.y);
}

bool ContainerScreen::_isHovering(const Slot& slot, int mouseX, int mouseY) const
{
    const SlotDisplay& display = getSlotDisplay(slot);
    if (!display.bVisible) return false;
    mouseX -= m_leftPos;
    mouseY -= m_topPos;
    float off = 3 * display.size / 50.0f;
    return mouseX >= display.x - off && mouseX < display.x + display.size - off && mouseY >= display.y - off && mouseY < display.y + display.size - off;
}

void ContainerScreen::_playInteractSound()
{
    m_pMinecraft->m_pSoundEngine->playUI(C_SOUND_UI_PRESS);
}

void ContainerScreen::_tryPlayInteractSound()
{
    if (_useController())
        _playInteractSound();
}

void ContainerScreen::_selectSlot(Slot* slot)
{
    if (!slot) return;
    const SlotDisplay& display = getSlotDisplay(*slot);
    if (!display.bVisible) return;
    int off = 3 * display.size / 50;
    handlePointerLocation(m_leftPos + display.x - off + display.size / 2, m_topPos + display.y - off + display.size / 2);
}

bool ContainerScreen::_selectSlotInDirection(ScreenDirection dir)
{
    Slot* toSelect = _findSlotInDirection(dir);
    if (!toSelect)
        toSelect = _findSlotInDirection(dir, true);

    if (toSelect)
    {
        _selectSlot(toSelect);
        return true;
    }

    return false;
}

void ContainerScreen::_controllerDirectionChanged(GameController::StickID stickId, GameController::StickState stickState)
{
    Screen::_controllerDirectionChanged(stickId, stickState);
    if (stickId == 1 && stickState == GameController::STICK_STATE_NONE)
    {
        _selectSlot(_findSlot());
    }
}

void ContainerScreen::_initMenuPointer()
{
    //@NOTE: Calling this as a fallback, if for some reason, there isn't a slot available
    Screen::_initMenuPointer();

    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        Slot* slot = *it;
        if (slot->m_slot == 0 && (!m_pMinecraft->m_pLocalPlayer || slot->m_pContainer == m_pMinecraft->m_pLocalPlayer->m_pInventory))
        {
            _selectSlot(slot);
            break;
        }
    }
}

void ContainerScreen::init()
{
    Screen::init();
    m_pMinecraft->m_pLocalPlayer->m_pContainerMenu = m_pMenu;
    m_leftPos = (m_width - m_imageWidth) / 2;
    m_topPos = (m_height - m_imageHeight) / 2;

    m_slotDisplays.clear();
    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        m_slotDisplays.push_back(_createSlotDisplay(*(*it)));
    }
}

void ContainerScreen::render(float partialTicks)
{
    renderBackground();
    _renderBg(partialTicks);

    MatrixStack::Ref matrix = MatrixStack::World.push();
    matrix->translate(Vec3(m_leftPos, m_topPos, 0.0f));
    currentShaderColor = Color::WHITE;

    Slot* hoveredSlot = nullptr;

    for (std::vector<Slot*>::iterator it = m_pMenu->m_slots.begin(); it != m_pMenu->m_slots.end(); ++it)
    {
        Slot* slot = *it;
        _renderSlot(*slot);
        if (_isHovering(*slot))
        {
            const SlotDisplay& display = getSlotDisplay(*slot);
            hoveredSlot = slot;
            //@NOTE: fillGradient is being used instead of fill, so the shader color won't be changed, I think the same happened on the original
            MatrixStack::Ref highlightMatrix = MatrixStack::World.push();
            highlightMatrix->translate(Vec3(display.x, display.y, 0));
            highlightMatrix->scale(display.size / 18.0f);
            fillGradient(0, 0, 16, 16, 0x80FFFFFF, 0x80FFFFFF);
        }
    }

    Inventory* inv = m_pMinecraft->m_pLocalPlayer->m_pInventory;
    if (!inv->getCarried().isEmpty())
    {
        matrix->translate(Vec3(0.0f, 0.0f, 200.0f));
        MatrixStack::Ref carriedMatrix = MatrixStack::World.push();
        carriedMatrix->translate(Vec3(m_menuPointer.x - m_leftPos - 8, m_menuPointer.y - m_topPos - 8, 0.0f));
        if (m_uiTheme == UI_CONSOLE)
            carriedMatrix->scale(3.0f); // 54 / 18.0f
        ItemRenderer::singleton().renderGuiItem(m_pFont, m_pMinecraft->m_pTextures, inv->getCarried(), 0, 0, true);
        ItemRenderer::singleton().renderGuiItemOverlay(m_pFont, m_pMinecraft->m_pTextures, inv->getCarried(), 0, 0);
    }

    Lighting::turnOff();
    _renderLabels();

    if (!inv->getCarried() && hoveredSlot && hoveredSlot->hasItem())
    {
        std::string name = Language::singleton().get(hoveredSlot->getItem().getDescriptionId() + ".name");
        if (!name.empty())
        {
            int w = m_pFont->width(name);
            int tx = m_menuPointer.x - m_leftPos + 12;
            int ty = m_menuPointer.y - m_topPos - 12;
            if (m_uiTheme == UI_CONSOLE)
            {
                blitNineSlice(*m_pMinecraft->m_pTextures, ScreenRenderer::POINTER_TEXT_PANEL_SLICES, tx - 6, ty - 6, w * 2 + 12, 28, 8);
                MatrixStack::Ref tooltipMatrix = MatrixStack::World.push();
                m_pFont->drawScalable(name, tx, ty, -1);
            }
            else
            {
                fillGradient(tx - 3, ty - 3, tx + w + 3, ty + 8 + 3, 0xC0000000, 0xC0000000);
                m_pFont->drawShadow(name, tx, ty, -1);
            }
        }
    }

    matrix.release();
    Screen::render(partialTicks);
}

void ContainerScreen::pointerPressed(int mouseX, int mouseY, MouseButtonType button)
{
    Screen::pointerPressed(mouseX, mouseY, button);
    if (m_pMinecraft->isTouchscreen()) return;
    slotClicked(mouseX, mouseY, button);
}

void ContainerScreen::pointerReleased(int mouseX, int mouseY, MouseButtonType button)
{
    Screen::pointerReleased(mouseX, mouseY, button);
    if (m_pMinecraft->isTouchscreen() && m_timeSlotDragged < 5)
        slotClicked(mouseX, mouseY, button);
    m_timeSlotDragged = 0;
}

void ContainerScreen::handlePointerPressed(bool isPressed)
{
    Screen::handlePointerPressed(isPressed);
    if (isPressed && _findSlot())
        m_timeSlotDragged++;
    else m_timeSlotDragged = 0;

    if (m_pMinecraft->isTouchscreen() && m_timeSlotDragged % 5 == 0)
    {
        slotClicked(m_menuPointer.x, m_menuPointer.y, MOUSE_BUTTON_RIGHT);
    }
}

void ContainerScreen::slotClicked(int mouseX, int mouseY, MouseButtonType button, bool quick)
{
    if (button == MOUSE_BUTTON_LEFT || button == MOUSE_BUTTON_RIGHT)
    {
        Slot* slot = _findSlot(mouseX, mouseY);
        bool outside = mouseX < m_leftPos || mouseY < m_topPos || mouseX >= m_leftPos + m_imageWidth || mouseY >= m_topPos + m_imageHeight;
        int index = -1;
        if (slot) index = slot->m_index;
        if (outside) index = -999;
        if (index != -1)
            slotClicked(slot, index, button, index != -999 && quick);
    }
}

void ContainerScreen::slotClicked(Slot* slot, int index, MouseButtonType button, bool quick)
{
    _tryPlayInteractSound();
    m_pMinecraft->m_pGameMode->handleInventoryMouseClick(m_pMenu->m_containerId, index, button, quick, m_pMinecraft->m_pLocalPlayer);
}

void ContainerScreen::slotClicked(int mouseX, int mouseY, MouseButtonType button)
{
    slotClicked(mouseX, mouseY, button, m_pMinecraft->m_pPlatform->shiftPressed());
}

void ContainerScreen::keyPressed(int keyCode)
{
    if (m_pMinecraft->getOptions()->isKey(KM_CONTAINER_QUICKMOVE, keyCode) && _useController())
    {
        slotClicked(m_menuPointer.x, m_menuPointer.y, MOUSE_BUTTON_LEFT, true);
    }
    else if (m_pMinecraft->getOptions()->isKey(KM_CONTAINER_SPLIT, keyCode) && _useController())
    {
        slotClicked(m_menuPointer.x, m_menuPointer.y, MOUSE_BUTTON_RIGHT, false);
    }
    else
    {
        if (_useController() &&
                ((m_pMinecraft->getOptions()->isKey(KM_MENU_UP, keyCode) && _selectSlotInDirection(UP)) ||
                (m_pMinecraft->getOptions()->isKey(KM_MENU_DOWN, keyCode) && _selectSlotInDirection(DOWN)) ||
                (m_pMinecraft->getOptions()->isKey(KM_MENU_RIGHT, keyCode) && _selectSlotInDirection(RIGHT)) ||
                (m_pMinecraft->getOptions()->isKey(KM_MENU_LEFT, keyCode) && _selectSlotInDirection(LEFT))))
        {
            _playSelectSound();
            return;
        }

        Screen::keyPressed(keyCode);
    }
}

const SlotDisplay& ContainerScreen::getSlotDisplay(const Slot& slot) const
{
    return m_slotDisplays[slot.m_index];
}

void ContainerScreen::onClose()
{
    if (m_pMinecraft->m_pLocalPlayer)
        m_pMinecraft->m_pLocalPlayer->closeContainer();
}

void ContainerScreen::tick()
{
    Screen::tick();
    if (!m_pMinecraft->m_pLocalPlayer->isAlive() || m_pMinecraft->m_pLocalPlayer->m_bRemoved)
        m_pMinecraft->m_pLocalPlayer->closeContainer();
}

void ContainerScreen::slotsChanged(Container* container)
{
}

bool ContainerScreen::isPauseScreen()
{
    return false;
}
