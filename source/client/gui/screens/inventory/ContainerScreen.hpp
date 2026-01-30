#pragma once

#include "client/gui/Screen.hpp"
#include "world/inventory/ContainerMenu.hpp"
#include "world/inventory/Slot.hpp"

struct SlotDisplay
{
    int m_x, m_y, m_size, m_noItemIcon;
    std::string m_noItemTexture;
    bool m_bVisible, m_bIconHolder;

    SlotDisplay() :
        m_x(0),
        m_y(0),
        m_size(0),
        m_noItemIcon(-1),
        m_noItemTexture(""),
        m_bVisible(false),
        m_bIconHolder(false)
    {
    }

    SlotDisplay(int x, int y, int size = 18, bool iconHolder = false, int noItemIcon = -1, const std::string& noItemTexture = "") :
        m_x(x),
        m_y(y),
        m_size(size),
        m_noItemIcon(noItemIcon),
        m_noItemTexture(noItemTexture),
        m_bVisible(true),
        m_bIconHolder(iconHolder)
    {
    }
};

class ContainerScreen : public Screen
{
public:
    ContainerScreen(ContainerMenu* menu);

private:
    void _renderSlot(Slot& slot);
    Slot* _findSlot();
    Slot* _findSlot(int mouseX, int mouseY);
    bool _isHovering(const Slot& slot) const;
    bool _isHovering(const Slot& slot, int mouseX, int mouseY) const;

protected:
    virtual void _renderLabels() = 0;
    virtual void _renderBg(float partialTick) = 0;
    virtual SlotDisplay _createSlotDisplay(const Slot&) = 0;
    virtual void _playInteractSound();
    virtual void _tryPlayInteractSound();

public:
    void init() override;
    void render(float partialTick) override;
    void onClose() override;
    void tick() override;
    bool isPauseScreen() override;
    void pointerPressed(int mouseX, int mouseY, MouseButtonType button) override;
    void pointerReleased(int mouseX, int mouseY, MouseButtonType button) override;
    void handlePointerPressed(bool isPressed) override;
    void keyPressed(int key) override;

    const SlotDisplay& getSlotDisplay(const Slot&) const;

public:
    virtual void slotsChanged(Container* container);
    virtual void slotClicked(int mouseX, int mouseY, MouseButtonType button);
    virtual void slotClicked(Slot* slot, int index, MouseButtonType button, bool quick);

public:
    ContainerMenu* m_pMenu;

protected:
    int m_imageWidth;
    int m_imageHeight;
    int m_leftPos;
    int m_topPos;
    std::vector<SlotDisplay> m_slotDisplays;

private:
    int m_timeSlotDragged;
};