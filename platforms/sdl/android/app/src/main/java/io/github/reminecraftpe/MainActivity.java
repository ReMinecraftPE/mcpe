package io.github.reminecraftpe;

import android.os.Build;
import android.os.Bundle;
import android.view.WindowManager;

import org.libsdl.app.LimitedSDLActivity;

public class MainActivity extends LimitedSDLActivity {
    private final ImmersiveModeStrategy immersiveModeStrategy = ImmersiveModeStrategy.create(this);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Fullscreen
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }
        immersiveModeStrategy.onCreate();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        // Fullscreen
        immersiveModeStrategy.onWindowFocusChanged(hasFocus);
    }
}
