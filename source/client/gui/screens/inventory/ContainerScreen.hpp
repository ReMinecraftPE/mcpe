#pragma once

#include "client/gui/Screen.hpp"
#include "world/inventory/ContainerMenu.hpp"
#include "world/inventory/Slot.hpp"

class ContainerScreen : public Screen
{
public:
    ContainerScreen(ContainerMenu* menu);

private:
    void _renderSlot(Slot* slot);
    Slot* _findSlot();
    Slot* _findSlot(int mouseX, int mouseY);
    bool _isHovering(Slot* slot) const;
    bool _isHovering(Slot* slot, int mouseX, int mouseY) const;

protected:
    virtual void _renderLabels() = 0;
    virtual void _renderBg(float partialTick) = 0;
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

private:
    int m_timeSlotDragged;
};