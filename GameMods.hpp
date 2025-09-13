#pragma once

// Here is a list of options that you can enable when building the project:

//#define ORIGINAL_CODE				 // Unpatches all bugs and allows building for Xperia Play only. (TODO)

#ifdef  ORIGINAL_CODE
#define DEMO
#else

// Mods
//#define MOD_USE_FLAT_WORLD         // Use a flat world instead of the regular world generation
//#define MOD_USE_BIGGER_SCREEN_SIZE // Use a bigger screen size instead of 854x480
//#define MOD_DONT_COLOR_GRASS       // Don't give the top of grass tiles a different color. (like Classic) @TODO: This does not do shit anymore
//#define MOD_POPOUT_CONSOLE         // Open a separate console aside from the game window. By default only the debugger can see our logs. Win32 Debug only.

// Tests
//#define TEST_SURVIVAL_MODE      	 // Test survival mode.

// Features (major changes)

// Enhancements (minor changes)
//#define ENH_ENTITY_SHADING      	       // Allows shading of entities -- Currently we are abandoning this. Want to add normal support
#define ENH_SHADE_HELD_TILES    	       // Allows shading of the item in hand
#define ENH_FIX_INVIS_STAIRS    	       // Fixes a bug wherein a 16x16x16 chunk in the world that contains only stairs is invisible
#define ENH_ALLOW_AO_TOGGLE                // Allows using the F4 key to toggle ambient occlusion (buggy)
#define ENH_TRANSPARENT_HOTBAR  	       // Allows the hotbar to be transparent. Due to a bug in the code, it is not.
#define ENH_CAMERA_NO_PARTICLES 	       // Hide particles from the view of a camera, such as smoke, that would otherwise render the resulting image useless.
#define ENH_USE_JAVA_LIGHT_RAMP 	       // Use Java Beta 1.3 light ramp instead of flawed PE one
#define ENH_RUN_DAY_NIGHT_CYCLE 	       // Allow the day/night cycle to run.
#define ENH_USE_OWN_AO          	       // Use own ambient occlusion engine - looks pretty much the same except it fixes the corners
#define ENH_ADD_OPTIONS_PAUSE    	       // Add an 'options' button in the pause menu
#define ENH_ALLOW_SAND_GRAVITY  	       // Allow sand to fall.
#define ENH_USE_GUI_SCALE_2     	       // Use a 2x GUI scale instead of 3x. Looks better on PC
#define ENH_ALLOW_SCROLL_WHEEL  	       // Allow use of the scroll wheel to change selected inventory slots
#define ENH_3D_INVENTORY_TILES  	       // Uses 3D rendered inventory tiles, use with ENH_SHADE_HELD_TILES to render correctly.
#define ENH_MENU_BACKGROUND 		       // Renders a spinning panorama (if it's available) in the background of the main menu
#define ENH_GUI_ITEM_POP                   // Calls Inventory::tick() to create the "pop" animation for items that enter the hotbar. This function was not present on Pocket Edition.
//#define ENH_DISABLE_FORCED_SAVE_UPGRADES // Prevents the forced format-version upgrade of world/level saves, effectively opting-out of new save formats. See LEVEL_STORAGE_VERSION_DEFAULT in LevelData.hpp.
//#define ENH_FACED_TERRAIN_PARTICLES        // Sets the TerrainParticle's texture depending on the face the block is being hit from. This is something Notch never did for whatever reason.

// TODO: Implement this permanently?
#define ENH_IMPROVED_SAVING     	 // Improve world saving. The original Minecraft doesn't always really save for some reason

// Toggle Demo Mode
//#define DEMO

#endif
