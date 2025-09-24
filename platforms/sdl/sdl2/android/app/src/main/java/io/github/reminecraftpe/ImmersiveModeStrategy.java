package io.github.reminecraftpe;

import android.annotation.TargetApi;
import android.app.Activity;
import android.os.Build;
import android.view.View;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;

public abstract class ImmersiveModeStrategy {
    protected final Activity activity;
    private ImmersiveModeStrategy(Activity activity) {
        this.activity = activity;
    }

    public abstract void onCreate();
    public abstract void onWindowFocusChanged(boolean hasFocus);

    static ImmersiveModeStrategy create(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return new Modern(activity);
        } else {
            return new Legacy(activity);
        }
    }

    @TargetApi(Build.VERSION_CODES.R)
    private static class Modern extends ImmersiveModeStrategy {
        private Modern(Activity activity) {
            super(activity);
        }

        @Override
        public void onCreate() {
        }

        @Override
        public void onWindowFocusChanged(boolean hasFocus) {
            if (hasFocus) {
                activity.getWindow().setDecorFitsSystemWindows(false);
                WindowInsetsController controller = activity.getWindow().getInsetsController();
                if (controller != null) {
                    controller.hide(WindowInsets.Type.systemBars());
                    controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
                }
            }
        }
    }

    @SuppressWarnings("deprecation")
    private static class Legacy extends ImmersiveModeStrategy implements View.OnSystemUiVisibilityChangeListener {
        private Legacy(Activity activity) {
            super(activity);
        }

        @Override
        public void onCreate() {
            activity.getWindow().getDecorView().setOnSystemUiVisibilityChangeListener(this);
        }

        @Override
        public void onWindowFocusChanged(boolean hasFocus) {
            if (hasFocus) {
                Window window = activity.getWindow();
                window.getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_FULLSCREEN |
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                    View.INVISIBLE
                );
                window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
                window.clearFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
            }
        }

        @Override
        public void onSystemUiVisibilityChange(int visibility) {
            if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0 || (visibility & View.SYSTEM_UI_FLAG_HIDE_NAVIGATION) == 0) {
                onWindowFocusChanged(true);
            }
        }
    }
}
