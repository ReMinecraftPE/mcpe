varying vec4 light;
varying vec4 fogColor;

#ifndef COLOR_BASED
	varying vec2 uv;
#endif

#ifdef USE_VERTEX_COLORS
	varying vec4 vertexColor;
#endif

#ifdef USE_OVERLAY
	varying vec4 overlayColor;
#endif

#ifdef USE_COLOR_MASK
	varying vec4 changeColor;
#endif

#ifdef GLINT
	varying vec2 layer1UV;
	varying vec2 layer2UV;
#endif