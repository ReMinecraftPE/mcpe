#pragma once

// Here is a list of options that you can enable when building the project:

// ORIGINAL_CODE - Unpatches all bugs and allows building for Xperia Play only. (TODO)
// #define ORIGINAL_CODE

#ifdef  ORIGINAL_CODE
#define DEMO
#else

// Enhancements
#define ENH_ENTITY_SHADING      // Allows shading of entities
#define ENH_SHADE_HELD_TILES    // Allows shading of the item in hand
#define ENH_FIX_INVIS_STAIRS    // Fixes a bug wherein a 16x16x16 chunk in the world that contains only stairs is invisible
#define ENH_ALLOW_AO            // Allows using the F4 key to toggle ambient occlusion (buggy)
#define ENH_TRANSPARENT_HOTBAR  // Allows the hotbar to be transparent. Due to a bug in the code, it is not.
#define ENH_INSTA_BREAK         // Allows instant breaking of blocks. @TODO: Fix the mode without this
#define ENH_CAMERA_NO_PARTICLES // Hide particles from the view of a camera, such as smoke, that would otherwise render the resulting image useless.
#define ENH_USE_JAVA_LIGHT_RAMP // Use Java Beta 1.3 light ramp instead of flawed PE one
#define ENH_RUN_DAY_NIGHT_CYCLE // Allow the day/night cycle to run.
#define ENH_ENABLE_9TH_SLOT     // Enable the 9th hotbar slot, instead of it being a "..." placeholder
#define ENH_USE_OWN_AO          // Use own ambient occlusion engine - looks pretty much the same except it fixes the corners
#define ENH_ADD_OPTIONS_PAUSE   // Add an 'options' button in the pause menu
#define ENH_EXTRA_ITEMS_IN_INV  // Add extra items in a new 5th row in the inventory.
#define ENH_HIGHLIGHT_BY_HOVER  // Highlight buttons by hovering them instead of the usual way.
#define ENH_ALLOW_SAND_GRAVITY  // Allow sand to fall.
#define ENH_USE_GUI_SCALE_2     // Use a 2x GUI scale instead of 3x. Looks better on PC

// Mods
//#define MOD_USE_FLAT_WORLD         // Use a flat world instead of the regular world generation
//#define MOD_USE_BIGGER_SCREEN_SIZE // Use a bigger screen size instead of 854x480
//#define MOD_DONT_COLOR_GRASS       // Don't give the top of grass tiles a different color. (like Classic)

// Tests
//#define TEST_DROPPED_ITEMS      // Allow dropped items to be dropped and collected.

// Toggle Demo Mode
//#define DEMO

#endif
