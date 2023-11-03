package org.libsdl.app;

// Prevent SDL From Controlling Immersive Mode
public class LimitedSDLActivity extends SDLActivity {
    @Override
    boolean sendCommand(int command, Object data) {
        if (command == COMMAND_CHANGE_WINDOW_STYLE) {
            return true;
        } else {
            return super.sendCommand(command, data);
        }
    }

    @Override
    public void onSystemUiVisibilityChange(int visibility) {
    }
}
